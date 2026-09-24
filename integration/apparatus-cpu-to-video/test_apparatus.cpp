// test_apparatus.cpp
//
// Apparatus check for the nanomig_harness: proves the harness can drive
// nanomig.v out of reset, place a small 68000 program where the reset
// vector fetch finds it, observe the CPU executing that program purely
// through the RAM-port access log, and observe its effect on the video
// output. Nothing here is an HRM conformance claim -- every check below
// is explicitly marked "not graded".
//
// See SPECIFICATION.md in this directory for what is and is not checked.

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <string>
#include <vector>
#include <optional>

#include "nanomig_harness.h"

// NANOMIG_CORE_NAME is passed by the Makefile as a bare identifier
// (-DNANOMIG_CORE_NAME=fx68k), not a string literal, to avoid multiple
// layers of shell/Makefile quoting (verilator re-splits -CFLAGS through
// its own generated Makefile) mangling embedded quote characters.
#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

int g_pass = 0;
int g_fail = 0;

void report(bool ok, const std::string& label) {
    std::printf("%s: %s [not graded -- apparatus check, no claim uid]\n",
                 ok ? "PASS" : "FAIL", label.c_str());
    if (ok) ++g_pass; else ++g_fail;
}

// True if `needle` occurs as an in-order subsequence of `haystack`.
bool is_subsequence(const std::vector<uint32_t>& needle, const std::vector<uint32_t>& haystack) {
    size_t j = 0;
    for (size_t i = 0; i < haystack.size() && j < needle.size(); ++i) {
        if (haystack[i] == needle[j]) ++j;
    }
    return j == needle.size();
}

} // namespace

int main() {
    auto wall_start = std::chrono::steady_clock::now();

    const char* mutate = std::getenv("MUTATE");
    const bool mutate_nocolour = mutate && std::string(mutate) == "nocolour";

    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    std::printf("MUTATE: %s\n", mutate_nocolour ? "nocolour" : "(none)");

    nmh::Config cfg; // defaults: 2.0 MB chip RAM, OCS, 68000 behaviour, no turbo -- see nanomig_harness.h
    nmh::NanomigHarness h(cfg);

    // ---- lay out the test program --------------------------------------
    //
    // Reset vector (SSP, PC) at logical $000000/$000004, read through the
    // overlay while ovl==1 (the reset default), physically stored at
    // kKickShadowWordBase (see nanomig_harness.h for the derivation).
    // The program itself follows right after the vectors, in the same
    // overlay-mapped window, so every fetch keeps landing on the shadow.
    constexpr uint32_t kSspLogical   = 0x00081000; // even, inside identity-mapped chip RAM
    constexpr uint32_t kEntryLogical = 0x00000008; // right after the two vector longs
    constexpr uint32_t kSrcLogical   = 0x00080010; // identity-mapped, NOT inside the overlay window
    constexpr uint32_t kDstLogical   = 0x00080014;
    constexpr uint32_t kColor00      = 0x00DFF180; // Denise COLOR00, a chip register -- never touches ram_address
    constexpr uint16_t kMarker       = 0x1234;
    constexpr uint16_t kExpectedColour = 0x0F52; // R=0xF G=0x5 B=0x2, distinct guns -- what a passing run must show
    const uint16_t kColour = mutate_nocolour ? 0x0000 : kExpectedColour; // what the program actually writes

    h.poke_long(nmh::kKickShadowWordBase + 0, kSspLogical);
    h.poke_long(nmh::kKickShadowWordBase + 2, kEntryLogical);

    nmh::NanomigHarness::Program prog(h, nmh::kKickShadowWordBase + kEntryLogical / 2);
    std::vector<uint32_t> expected_reads;
    expected_reads.push_back(nmh::kKickShadowWordBase + 0); // SSP high
    expected_reads.push_back(nmh::kKickShadowWordBase + 1); // SSP low
    expected_reads.push_back(nmh::kKickShadowWordBase + 2); // PC high
    expected_reads.push_back(nmh::kKickShadowWordBase + 3); // PC low

    auto mark_opcode = [&](uint32_t addr) { expected_reads.push_back(addr); };

    mark_opcode(prog.here());
    prog.move_w_imm_abs(kMarker, kSrcLogical);         // MOVE.W #$1234,$00080010

    mark_opcode(prog.here());
    prog.move_w_abs_abs(kSrcLogical, kDstLogical);     // MOVE.W $00080010,$00080014

    mark_opcode(prog.here());
    prog.move_w_imm_abs(kColour, kColor00);            // MOVE.W #kColour,$00DFF180

    mark_opcode(prog.here());
    prog.bra_s_self();                                  // loop:  BRA.S loop

    // ---- reset and boot --------------------------------------------------
    //
    // The design's internal reset (minimig_syscontrol, minimig.v:851-857)
    // counts whole frames (sof pulses), not clk_sys cycles, before
    // releasing sys_reset -- which also gates the CPU reset
    // (minimig.v:1050-1057). boot() polls cpu_nrst_out rather than
    // assuming a cycle count; the budget here is a generous safety cap.
    constexpr uint64_t kBootBudget = 6'000'000; // comfortably more than a handful of PAL frames
    auto boot = h.boot(kBootBudget);
    // cpu_nrst_out is the fx68k/tg68k core's own reset-out pin
    // (cpu_wrapper.v's reset_out); it can go high well before the design
    // does anything useful on the bus, because the chipset's own reset
    // (minimig_syscontrol.v, gating cpu_rst too, see minimig.v:1050-1057)
    // is held for whole frames, not clk_sys cycles. The MEASURED line
    // below reports when the first real RAM-port read was actually seen.
    report(boot.cpu_released,
           "cpu_nrst_out went high (" + std::to_string(boot.cycles_to_release) + " clk_sys cycles after reset deasserted)");

    // ---- run until the program has executed, then two more frames ------
    //
    // Poll in chunks rather than assuming a fixed cycle budget: this is
    // driven purely by what the RAM-port log and vs edges actually show.
    constexpr uint64_t kChunk = 250'000;
    constexpr uint64_t kMaxAfterBoot = 4'000'000;
    bool executed = false;
    uint64_t frames_since_exec = 0;
    size_t video_scan_pos = 0;
    bool prev_vs = false;

    auto program_executed = [&]() -> bool {
        for (const auto& a : h.ram_log()) {
            if (a.is_write && a.word_addr == (kDstLogical / 2) && a.data == kMarker) return true;
        }
        return false;
    };

    uint64_t after_boot_cycles = 0;
    while (after_boot_cycles < kMaxAfterBoot) {
        h.run_cycles(kChunk);
        after_boot_cycles += kChunk;

        // count vs rising edges seen so far in the newly captured samples
        const auto& v = h.video();
        for (; video_scan_pos < v.size(); ++video_scan_pos) {
            bool cur = v[video_scan_pos].vs;
            if (cur && !prev_vs && executed) ++frames_since_exec;
            prev_vs = cur;
        }

        if (!executed && program_executed()) executed = true;
        if (executed && frames_since_exec >= 2) break;
    }

    report(executed, "CPU stores the value it computed into RAM where the test can inspect it");

    // ---- RAM-port bus-trace checks --------------------------------------

    auto reads = nmh::coalesce(h.ram_log());
    std::vector<uint32_t> read_addrs;
    for (const auto& a : reads) if (!a.is_write) read_addrs.push_back(a.word_addr);

    // A strict "first four reads" check would break on any RAM-port read
    // that legitimately precedes the CPU's own reset-vector fetch (e.g.
    // agnus refresh reads, which run before the CPU comes out of reset --
    // see the boot-timing note below); a subsequence check still proves
    // the vector words were fetched, in order, without assuming they are
    // the very first thing on the bus.
    std::vector<uint32_t> vector_reads(expected_reads.begin(), expected_reads.begin() + 4);
    bool vectors_ok = is_subsequence(vector_reads, read_addrs);
    report(vectors_ok, "CPU fetched its initial SSP and PC from the overlay-mapped reset vector (ram_address " +
                        std::to_string(nmh::kKickShadowWordBase) + ".." + std::to_string(nmh::kKickShadowWordBase + 3) + ")");

    // Diagnostic, not graded: when the first vector word was actually
    // seen on the bus, in clk_sys cycles from t=0 -- useful to relate to
    // the "CPU released from reset" line below, since cpu_nrst_out and
    // the first real bus access do not have to coincide.
    for (const auto& a : h.ram_log()) {
        if (!a.is_write && a.word_addr == nmh::kKickShadowWordBase) {
            std::printf("MEASURED: first RAM-port read (expected: SSP high word) observed at clk_sys cycle %llu\n",
                        (unsigned long long)a.cycle);
            break;
        }
    }

    std::vector<uint32_t> opcode_reads(expected_reads.begin() + 4, expected_reads.end());
    bool opcodes_ok = is_subsequence(opcode_reads, read_addrs);
    report(opcodes_ok, "CPU fetched and executed the program's opcodes in the order they were placed");

    bool src_write_ok = false;
    for (const auto& a : h.ram_log())
        if (a.is_write && a.word_addr == (kSrcLogical / 2) && a.data == kMarker) { src_write_ok = true; break; }
    report(src_write_ok, "CPU wrote its immediate operand to RAM (MOVE.W #imm,abs.L, word " + std::to_string(kSrcLogical / 2) + ")");

    uint64_t src_read_cycle = UINT64_MAX, dst_write_cycle = UINT64_MAX;
    for (const auto& a : h.ram_log()) {
        if (!a.is_write && a.word_addr == (kSrcLogical / 2) && a.data == kMarker && src_read_cycle == UINT64_MAX)
            src_read_cycle = a.cycle;
        if (a.is_write && a.word_addr == (kDstLogical / 2) && a.data == kMarker && dst_write_cycle == UINT64_MAX)
            dst_write_cycle = a.cycle;
    }
    bool copy_ok = src_read_cycle != UINT64_MAX && dst_write_cycle != UINT64_MAX && src_read_cycle <= dst_write_cycle;
    report(copy_ok, "CPU read the value back and copied it to another RAM location (MOVE.W abs.L,abs.L, word " +
                     std::to_string(kSrcLogical / 2) + " -> word " + std::to_string(kDstLogical / 2) + ")");

    // ---- video check -------------------------------------------------
    //
    // COLOR00 ($DFF180) is a chip register: the CPU write to it goes
    // through minimig's custom-register bus and never touches
    // ram_address/ram_data, so it cannot appear in the RAM-port log --
    // it can only be observed on r/g/b. Sample deterministically at the
    // middle of the middle line of the last fully captured frame: this
    // works the same way whether the colour under test is bright or
    // black (a nonzero-colour search would not find "black" in the
    // MUTATE=nocolour control run).
    auto frames = nmh::find_frames(h.video());
    bool colour_ok = false;
    uint8_t got_r = 0, got_g = 0, got_b = 0;
    bool sampled = false;
    if (frames.size() >= 2) {
        nmh::Span frame = frames[frames.size() - 2]; // last fully closed frame
        auto all_lines = nmh::find_lines(h.video());
        std::vector<nmh::Span> lines_in_frame;
        for (const auto& l : all_lines)
            if (l.begin >= frame.begin && l.end <= frame.end) lines_in_frame.push_back(l);
        if (!lines_in_frame.empty()) {
            nmh::Span mid_line = lines_in_frame[lines_in_frame.size() / 2];
            size_t mid_offset = (mid_line.end - mid_line.begin) / 2;
            if (auto s = nmh::sample_at(h.video(), mid_line, mid_offset)) {
                sampled = true;
                got_r = s->r; got_g = s->g; got_b = s->b;
                colour_ok = (got_r == ((kExpectedColour >> 8) & 0xF)) &&
                            (got_g == ((kExpectedColour >> 4) & 0xF)) &&
                            (got_b == (kExpectedColour & 0xF));
            }
        }
    }
    report(sampled && colour_ok,
           "video shows the written COLOR00 guns at a sampled visible pixel (expected r=" +
           std::to_string((kExpectedColour >> 8) & 0xF) + " g=" + std::to_string((kExpectedColour >> 4) & 0xF) +
           " b=" + std::to_string(kExpectedColour & 0xF) + ", sampled r=" + std::to_string(got_r) +
           " g=" + std::to_string(got_g) + " b=" + std::to_string(got_b) +
           (sampled ? "" : ", no sample -- not enough frames captured") + ")");

    // ---- MEASURED lines (not graded) -------------------------------------
    //
    // hs is doubled by the always-on Amber scan doubler (nanomig.v:1577,
    // Amber.v:124/128/182): this reports the scan-doubler's own line
    // period, not the Amiga chipset's native horizontal line length. vs
    // passes straight through the doubler (Amber.v:183) so its period is
    // the real field/frame length.
    {
        auto lines = nmh::find_lines(h.video());
        if (lines.size() >= 3) {
            uint64_t sum = 0; size_t n = 0;
            for (size_t i = 1; i + 1 < lines.size(); ++i) { sum += lines[i].end - lines[i].begin; ++n; }
            std::printf("MEASURED: hs-to-hs period (scan-doubler output, not the Amiga's native line) = %.2f clk_sys cycles (n=%zu)\n",
                        n ? double(sum) / double(n) : 0.0, n);
        }
        auto fr = nmh::find_frames(h.video());
        if (fr.size() >= 3) {
            uint64_t sum = 0; size_t n = 0;
            for (size_t i = 1; i + 1 < fr.size(); ++i) { sum += fr[i].end - fr[i].begin; ++n; }
            std::printf("MEASURED: vs-to-vs period (frame length, vs is not doubled) = %.2f clk_sys cycles (n=%zu)\n",
                        n ? double(sum) / double(n) : 0.0, n);
            // lines-per-frame using the doubled hs count, reported as measured, not asserted
            if (fr.size() >= 2 && lines.size() >= 1) {
                nmh::Span f = fr[fr.size() - 2];
                size_t cnt = 0;
                for (const auto& l : lines) if (l.begin >= f.begin && l.end <= f.end) ++cnt;
                std::printf("MEASURED: scan-doubler output lines in the last full frame = %zu\n", cnt);
            }
        }
    }

    auto wall_end = std::chrono::steady_clock::now();
    double wall_seconds = std::chrono::duration<double>(wall_end - wall_start).count();

    std::printf("%d passed, %d failed\n", g_pass, g_fail);
    std::printf("RUNTIME: %.2f s wall clock (core=%s, mutate=%s, clk_sys cycles=%llu)\n",
                 wall_seconds, NMH_XSTR(NANOMIG_CORE_NAME), mutate_nocolour ? "nocolour" : "none",
                 (unsigned long long)h.cycles());

    return g_fail == 0 ? 0 : 1;
}
