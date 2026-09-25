// test_vasm_program.cpp
//
// Apparatus check for 68000 test programs built with vasm and run by
// nanomig_harness.h: the program is loaded where the harness says, the CPU
// starts it through the reset vectors, it stores known values in chip RAM,
// and the harness finds its DONE mark and reads those values back. A second
// run of the same source with one value changed (prog_control.s) must fail the
// value check. Nothing here is an HRM conformance claim -- every line is
// "not graded". See SPECIFICATION.md.

#include <cstdio>
#include <string>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

// nanomig.i, restated: the harness side of the contract.
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 3'000'000;

const std::vector<uint16_t> kExpected = {0x1234, 0xCAFE, 0xF00D, 4, 3, 2, 1, 0};

int g_pass = 0, g_fail = 0;

void report(bool ok, const std::string& label) {
    std::printf("%s: %s [not graded -- apparatus check, no claim uid]\n", ok ? "PASS" : "FAIL", label.c_str());
    ++(ok ? g_pass : g_fail);
}

struct Run {
    size_t loaded = 0;
    bool released = false;
    std::optional<uint64_t> done;
    std::vector<std::optional<uint16_t>> words;
    bool org_fetched_after_vectors = false;
};

Run run_program(const std::string& bin) {
    Run r;
    nmh::Config cfg;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin);
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kRunBudget);
    for (size_t i = 0; i < kExpected.size() + 2; ++i) r.words.push_back(h.peek_logical(kResults + 2 * i));
    // The CPU fetched its reset vectors from the overlay shadow and then its
    // first opcode from kProgramOrg: the program ran from where it was loaded.
    bool vec = false;
    for (const auto& a : h.ram_log()) {
        if (!a.is_write && a.word_addr == nmh::kKickShadowWordBase + 3) vec = true;
        if (vec && !a.is_write && a.word_addr == nmh::kProgramOrg / 2) { r.org_fetched_after_vectors = true; break; }
    }
    return r;
}

bool values_match(const Run& r) {
    for (size_t i = 0; i < kExpected.size(); ++i)
        if (!r.words[i] || *r.words[i] != kExpected[i]) return false;
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));

    Run r = run_program(dir + "/prog.bin");
    report(r.loaded > 0, "prog.bin loaded at kProgramOrg (" + std::to_string(r.loaded) + " bytes)");
    report(r.released, "cpu_nrst_out released within the boot budget");
    report(r.org_fetched_after_vectors, "after its reset vectors the CPU fetched its first opcode at kProgramOrg");
    report(r.done.has_value(), "the program's DONE mark was written");
    report(r.loaded && values_match(r), "the stored values read back as the program wrote them (1234 CAFE F00D 4 3 2 1 0)");
    if (r.words.size() >= kExpected.size() + 2 && r.words[kExpected.size()] && r.words[kExpected.size() + 1])
        std::printf("MEASURED: DMACONR=%04X VPOSR=%04X as the program read them; DONE at clk_sys cycle %llu\n",
                    *r.words[kExpected.size()], *r.words[kExpected.size() + 1],
                    (unsigned long long)(r.done ? *r.done : 0));

    Run c = run_program(dir + "/prog_control.bin");
    const bool control_ran = c.loaded > 0 && c.done.has_value();
    report(control_ran && !values_match(c) && c.words[0] && *c.words[0] == 0x1235,
           "CONTROL: prog_control.bin stores 1235 first, and the value check fails on it");

    std::printf("%d passed, %d failed\n", g_pass, g_fail);
    return g_fail == 0 ? 0 : 1;
}
