// Test purpose: a second measurement of the same OCS/ECS pointer-width claim
//   test_agnus_audiodma.cpp's assertion 9,
//   LCH-OCS-THREE-BIT-READING-DOES-NOT-HOLD, already grades -- this time at a
//   boundary where `ecs` is a real, drivable port. That sibling file's own
//   header explains why its own measurement cannot drive one:
//   agnus_audiodma.v alone has no `ecs` or chipset input anywhere in its 157
//   lines, so its assertion 9 tests the OCS wording as an absolute rather
//   than against a chipset selector. The follow-up entry for this finding
//   (NM-HRM-024; "LCH-OCS-THREE-BIT-READING-DOES-NOT-HOLD (024)" in
//   hrm-testpipeline/work/2026-09-27_findings-review/followups.md) asks for
//   the same claim measured at the full Agnus top, with `ecs` driven both
//   ways, the way hrm/ch6-blitter-ecs and
//   hrm/ch8-disk-pointer-ecs-and-copper measure their own OCS/ECS
//   pointer-width claims. This file is that second fixture. It does not
//   replace, weaken or touch test_agnus_audiodma.cpp's own ten assertions,
//   which stay exactly as committed; this file's own three assertions are
//   reported ALONGSIDE that file's, never instead of it -- see this
//   package's Makefile, which builds and runs both.
// Test class: HRM_CONFORMANCE
// Primary oracle: same pages test_agnus_audiodma.cpp's assertions 8/9 cite --
//   Amiga Hardware Reference Manual, Third Edition, printed pp.139/145
//   (chapter 5 body, unqualified "high three bits"), appendix A p.266 ("high
//   3 bits, 5 bits if ECS"), appendix C pp.298/309 ("high 5 bits was 3
//   bits"). No new claim ids: assertions 1 and 2 below carry the SAME ids as
//   test_agnus_audiodma.cpp's assertion 9 (see that file's own header for
//   the full list), measured a second time at a boundary that can actually
//   drive the chipset input the manual's own OCS/ECS reading depends on.
//   Assertion 0 is a fixture-liveness check, no claim id, no manual page.
// Fixture boundary: agnus.v, the full Agnus top (register bus, all six DMA
//   engines) -- built the way
//   hrm/ch8-disk-pointer-ecs-and-copper/test_ch8_disk_pointer_ecs_and_copper.cpp
//   brings up the SAME top module (its DUT init list, its CPU
//   write-and-wait-for-grant protocol, and its "watch the shared bus for
//   this engine's own register address" read-back technique, adapted here
//   from disk's single-bit disk_dmal/disk_dmas pair to audio's own 4-bit
//   audio_dmal/audio_dmas[0]) -- nothing is cited from that file as an
//   oracle, only reused as a proven harness pattern for this same top
//   module. Confirmed again directly in this reading: agnus.v instantiates
//   agnus_audiodma (agnus.v:344-357) with NO `.ecs(...)` connection at all,
//   unlike its bitplane and sprite DMA instantiations a few lines below,
//   which do wire `.ecs(ecs)` (agnus.v:369, :392). So `ecs`, though a real,
//   drivable port at THIS boundary, still never reaches the audio engine --
//   it is driven both ways here because the follow-up entry asks for it to
//   be driven, not because this reading expects the audio pointer's width to
//   respond to it.
// Non-goals: everything test_agnus_audiodma.cpp's own ten assertions already
//   cover -- this file adds nothing to that list and repeats no claim id
//   outside the two named above. It does not exercise any DMA engine other
//   than audio channel 0 (disk_dmal/disk_dmas stay low throughout, and the
//   Copper/blitter/sprite/bitplane engines are never armed), and it does not
//   grade the register-bus write-and-wait protocol itself beyond what
//   assertion 0 needs -- hrm/ch8-disk-pointer-ecs-and-copper's own COPLIVE
//   check already covers that at this same boundary.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vagnus.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_addr) { return static_cast<uint16_t>(byte_addr >> 1); }

constexpr uint16_t AUD0LCH = to_reg(0x0A0);
constexpr uint16_t AUD0LCL = to_reg(0x0A2);
constexpr uint16_t AUD0DAT = to_reg(0x0AA);

// Fixture: the Agnus top level, brought up the way
// hrm/ch8-disk-pointer-ecs-and-copper/test_ch8_disk_pointer_ecs_and_copper.cpp
// brings up the same top (its DUT init list and CPU write/fetch-serving
// pattern); nothing is cited from it as an oracle.
class AgnusTopHarness {
public:
    AgnusTopHarness() : dut(new Vagnus) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->cck = 0;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->data_in = 0;
        dut->address_in = 0;
        dut->audio_dmal = 0;
        dut->audio_dmas = 0;
        dut->disk_dmal = 0;
        dut->disk_dmas = 0;
        dut->bls = 0;
        dut->ntsc = 0;
        dut->a1k = 0;
        dut->ecs = 0;
        dut->aga = 0;
        dut->floppy_speed = 0;

        run_cycles(24);
        dut->reset = 0;
        run_cycles(12);
    }

    ~AgnusTopHarness() {
        dut->final();
        delete dut;
    }

    AgnusTopHarness(const AgnusTopHarness&) = delete;
    AgnusTopHarness& operator=(const AgnusTopHarness&) = delete;

    void set_ecs(bool value) { dut->ecs = value ? 1 : 0; }

    void write_reg(uint16_t word_reg, uint16_t data) {
        for (int timeout = 0; timeout < 4000; ++timeout) {
            if (cycle(true, word_reg, data)) {
                cycle(false, 0, 0);
                return;
            }
        }
        std::cerr << "FAIL: CPU write timed out for register 0x"
                  << std::hex << word_reg << std::dec << "\n";
    }

    void run_cycles(int count) {
        for (int i = 0; i < count; ++i) cycle(false, 0, 0);
    }

    // Runs audio channel 0's own DMA slot to perform the requested grant
    // (restart: reload the working pointer from the location bank; advance:
    // step it by one word), then runs ONE MORE grant to read the result
    // back. Both grants are needed because this fixture cannot read
    // agnus_audiodma.v:132/143's working pointer, `audpt[channel]`,
    // directly the way the narrower test_agnus_audiodma.cpp fixture does
    // (there, address_out is a continuous, unclocked read of it -- see that
    // file's own header). At this top-level boundary address_out is instead
    // GATED by the arbitration mux (agnus.v:169-267): it only shows
    // audpt[channel] while dma_aud is actually asserted, which happens for
    // exactly one hpos value per line, so it cannot be re-sampled between
    // grants the way the narrower fixture re-samples between ticks. And a
    // capture taken on the SAME cycle as a grant reads address_out BEFORE
    // that cycle's own clock edge commits audpt[channel]'s update -- proven
    // by running this exact harness once with a single-grant capture: every
    // returned value was the PRECEDING grant's result, one grant behind
    // (measured directly, not assumed). So the requested grant's own
    // pre-edge capture is deliberately discarded, and what is returned is
    // the pre-edge capture of the FOLLOWING grant (forced to be a plain
    // advance, audio_dmas[0]=0) instead -- which reads address_out BEFORE
    // ITS OWN edge, i.e. exactly the value the requested grant's edge just
    // wrote. The same "watch the shared bus for this engine's own register
    // address" technique read_pointer_via_diskslot() in the sibling ch8
    // package's harness uses for DSKDATR, adapted from a single-bit
    // disk_dmal/disk_dmas pair to this port's own 4-bit
    // audio_dmal/audio_dmas[0], and extended with this second grant because
    // disk's own address_out has no separate location-vs-working-pointer
    // split to cross (test_agnus_audiodma.cpp's own
    // LOCATION-REGISTER-VS-WORKING-POINTER assertion is what establishes
    // audio does).
    uint32_t read_pointer_via_audioslot(bool restart, int max_cycles = 20000) {
        dut->audio_dmal = 0x1;  // channel 0 only
        dut->audio_dmas = restart ? 0x1 : 0x0;
        bool requested_grant_seen = false;
        for (int i = 0; i < max_cycles && !requested_grant_seen; ++i) {
            cycle(false, 0, 0);
            if (aud_capture_valid) requested_grant_seen = true;
        }
        dut->audio_dmas = 0x0;  // the follow-up read-back grant is always a plain advance
        uint32_t result = 0xFFFFFFFFu;
        bool readback_grant_seen = false;
        for (int i = 0; i < max_cycles && !readback_grant_seen; ++i) {
            cycle(false, 0, 0);
            if (aud_capture_valid) {
                result = aud_capture_value;
                readback_grant_seen = true;
            }
        }
        dut->audio_dmal = 0;
        dut->audio_dmas = 0;
        aud_capture_valid = false;
        return result;
    }

private:
    Vagnus* dut;
    bool aud_capture_valid = false;
    uint32_t aud_capture_value = 0;

    // is_write drives a CPU register write for this one cycle; otherwise the
    // cycle is idle apart from whatever DMA request lines the caller has set
    // beforehand (audio_dmal/audio_dmas here, matching how
    // read_pointer_via_diskslot's own idle cycles carry disk_dmal/disk_dmas
    // in the sibling ch8 harness). Returns true if a CPU write this cycle
    // was actually granted.
    bool cycle(bool is_write, uint16_t reg, uint16_t data) {
        dut->cck = !dut->cck;
        dut->aen = is_write ? 1 : 0;
        dut->rd = 0;
        dut->hwr = is_write ? 1 : 0;
        dut->lwr = is_write ? 1 : 0;
        dut->address_in = is_write ? reg : 0;
        dut->data_in = is_write ? data : 0;
        dut->eval();

        const bool granted = is_write && dut->cpu_custom && dut->reg_address_out == reg;

        aud_capture_valid = false;
        if (dut->dbr && dut->reg_address_out == AUD0DAT) {
            aud_capture_valid = true;
            aud_capture_value = dut->address_out;
        }

        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();

        if (is_write) {
            dut->aen = 0;
            dut->hwr = 0;
            dut->lwr = 0;
            dut->data_in = 0;
        }

        return granted;
    }
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- 0. TOP-LEVEL-LIVENESS (fixture check) ----------------
    {
        AgnusTopHarness h;
        h.set_ecs(false);
        // Same stimulus/expected pair hrm/ch8-disk-pointer-ecs-and-copper's
        // own LIVENESS check uses for DSKPTH/DSKPTL (0x0015 written to the
        // high-bits register, 0x2A2A to the low-bits register), reused
        // unchanged: AUDxLCH/AUDxLCL and DSKPTH/DSKPTL share the identical
        // 5-high/15-low pointer layout, so the same hand-derived word
        // address applies -- (0x15 << 15) | (0x2A2A >> 1) = 0xA9515.
        constexpr uint16_t kHighStim = 0x0015;
        constexpr uint16_t kLowStim = 0x2A2A;
        h.write_reg(AUD0LCH, kHighStim);
        h.write_reg(AUD0LCL, kLowStim);
        const uint32_t got = h.read_pointer_via_audioslot(/*restart=*/true);
        constexpr uint32_t kExpected = 0xA9515u;
        char buf[512];
        std::snprintf(buf, sizeof(buf),
            "TOP-LEVEL-LIVENESS [fixture check, no claim id, no manual "
            "page]: a known word (AUD0LCH=0x0015, "
            "AUD0LCL=0x2A2A) written through the real CPU register bus at "
            "the full Agnus top arrives, correctly assembled, at "
            "address_out on channel 0's own audio-DMA slot -- measured "
            "0x%05X, expected 0x%05X -- proving this second fixture's data "
            "path is alive before either claim below is trusted.",
            got, kExpected);
        checks.expect(got == kExpected, buf);
    }

    // ---------------- 1. LCH-ECS-PORT-CHANGES-WHAT-LOADS ----------------
    {
        AgnusTopHarness h0;
        h0.set_ecs(false);
        h0.write_reg(AUD0LCH, 0x001F);
        h0.write_reg(AUD0LCL, 0xFFFF);
        const uint32_t r_ocs = h0.read_pointer_via_audioslot(true);

        AgnusTopHarness h1;
        h1.set_ecs(true);
        h1.write_reg(AUD0LCH, 0x001F);
        h1.write_reg(AUD0LCL, 0xFFFF);
        const uint32_t r_ecs = h1.read_pointer_via_audioslot(true);

        char buf[1024];
        std::snprintf(buf, sizeof(buf),
            "LCH-ECS-PORT-CHANGES-WHAT-LOADS (appendix A p.266 \"(high 3 "
            "bits, 5 bits if ECS)\"; appendix C p.298/309 \"high 5 bits was "
            "3 bits\"): the SAME AUD0LCH/AUD0LCL write (all five high bits "
            "and all fifteen low bits set), presented once at a real ecs=0 "
            "and once at a real ecs=1 driven at THIS boundary -- the full "
            "Agnus top, where ecs is an actual, drivable port, unlike "
            "agnus_audiodma.v alone -- measured word address 0x%05X at "
            "ecs=0 and 0x%05X at ecs=1. [same claims as "
            "test_agnus_audiodma.cpp's LCH-OCS-THREE-BIT-READING-DOES-NOT-"
            "HOLD, measured a second time at a boundary with a real ecs "
            "port: 5-p139.c1, 5-sum.c1, 5-loc.c1, 5-appA-p266.c5, "
            "5-appA-p266.c11, 5-appB-p290.c57, 5-appB-p290.c92, "
            "5-appB-p290.c127, 5-appB-p290.c148, B-aud1lch.width, "
            "B-aud2lch.width, B-aud3lch.width, A-audxlc.c1, 5-appC-p298.c6, "
            "5-appC-p309.c6, A-audxlch.w1, C-map.lch.width, "
            "B-aud0lch.width]",
            r_ocs, r_ecs);
        checks.expect(r_ocs != r_ecs, buf);
    }

    // -------------- 2. LCH-OCS-HOLDS-ONLY-THREE-BITS-AT-REAL-ECS --------------
    {
        AgnusTopHarness h;
        h.set_ecs(false);
        h.write_reg(AUD0LCL, 0x0000);
        // The two "ECS-only" bit positions of AUDxLCH: bits 3 and 4
        // (word-address bits 19 and 20) -- same probe
        // test_agnus_audiodma.cpp's own assertion 9 uses. A genuine OCS
        // reading would give these NO effect at all.
        h.write_reg(AUD0LCH, 1u << 3);
        const uint32_t p_bit3 = h.read_pointer_via_audioslot(true);
        h.write_reg(AUD0LCH, 1u << 4);
        const uint32_t p_bit4 = h.read_pointer_via_audioslot(true);
        const bool ocs_reading_holds = (p_bit3 == 0) && (p_bit4 == 0);

        char buf[1024];
        std::snprintf(buf, sizeof(buf),
            "LCH-OCS-HOLDS-ONLY-THREE-BITS-AT-REAL-ECS (chapter 5 pp.139/145, "
            "unqualified, \"contain the high three bits\"; appendix A "
            "p.266's non-ECS half; appendix C's \"was 3 bits\"): with a "
            "real ecs=0 driven at the full Agnus top, a genuine OCS reading "
            "would give AUDxLCH's two \"ECS-only\" bit positions (bits 3 "
            "and 4) no effect on the fetch address. MEASURED: bit 3 alone "
            "gives word address 0x%X, bit 4 alone gives 0x%X -- same "
            "claims as test_agnus_audiodma.cpp's assertion 9 (see that "
            "assertion's own text for the full id list), measured a second "
            "time with a real ecs=0 port driven rather than no port at "
            "all.",
            p_bit3, p_bit4);
        checks.expect(ocs_reading_holds, buf);
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// Machine-readable records for tools/check_gates.py --tests. One line per
// mutation actually performed; assertions with no line here have never been
// mutation-tested. Every mutation below is a STIMULUS mutation to this
// file's OWN inputs, applied by editing this file, rebuilding, running,
// reverting, and rebuilding clean again to confirm the baseline returned --
// the same technique test_agnus_audiodma.cpp's own MUTATION section
// documents, reused here rather than an env-var/#ifdef scheme, since this
// file has no other mutation infrastructure to share it with. Baseline: 1
// passed, 2 failed (assertion 0 passes; assertions 1 and 2 are RED, matching
// test_agnus_audiodma.cpp's own required-red assertion 9 measured at this
// second boundary).
//
// Each mutation below was actually built (a scratch --Mdir, never obj_dir)
// and run before being reverted; the baseline quoted for each (1 passed, 2
// failed) is what a clean rebuild of the committed file prints.
//
// MUTATION TOP-LEVEL-LIVENESS stimulus red: kHighStim changed from 0x0015 to
//   0x0014 (kLowStim and kExpected left untouched) -> FAIL, measured
//   0xA1515 against the unchanged expected 0xA9515 (the written high half's
//   own bit 0, which DOES land at address_out per
//   test_agnus_audiodma.cpp's own WORD-ALIGNED-FETCH-AND-LC-PAIR, no longer
//   assembles to the expected constant); 0 passed, 3 failed. Reverted,
//   clean rebuild reproduced 1 passed/2 failed.
//   (A first attempt changed kLowStim's own bit 0, 0x2A2A -> 0x2A2B, and
//   measured NO change at all -- agnus_audiodma.v:89 reads only
//   data_in[15:1], so that bit is structurally never sampled, exactly the
//   fact test_agnus_audiodma.cpp's own POINTER-LSB-ALWAYS-ZERO assertion
//   measures. Diagnosed rather than reported as a false pass: the mutation
//   probed a bit this module never stores, not a broken comparator. The
//   kHighStim mutation above replaces it.)
// MUTATION LCH-ECS-PORT-CHANGES-WHAT-LOADS stimulus green: h1's own AUD0LCH
//   stimulus changed from 0x001F to 0x0007 (h1 still driven at ecs=true,
//   h0 unchanged at ecs=false with 0x001F) -> measured r_ocs=0xFFFFF,
//   r_ecs=0x3FFFF -- differ for an unrelated reason (different data, not
//   the ecs port), and `r_ocs != r_ecs` reads true -> PASS, confirming the
//   comparator is not a stuck fixture that always reports "equal"
//   regardless of what is fed to it; 2 passed, 1 failed. Reverted, clean
//   rebuild reproduced 1 passed/2 failed.
// MUTATION LCH-OCS-HOLDS-ONLY-THREE-BITS-AT-REAL-ECS stimulus green: the
//   probed bit positions changed from 3 and 4 (the genuinely ECS-only bits
//   agnus_audiodma.v:82 samples as part of data_in[4:0]) to 5 and 6, bits
//   outside that 5-bit field entirely -- same diagnostic
//   test_agnus_audiodma.cpp's own assertion-9 mutation already ran at the
//   narrower boundary. MEASURED: both probes now read back as word address
//   0x0, ocs_reading_holds flips to true -> PASS; 2 passed, 1 failed,
//   confirming the original bit choice (3 and 4) was load-bearing and this
//   assertion's red genuinely depends on probing bits inside the field an
//   OCS reading would exclude. Reverted, clean rebuild reproduced 1
//   passed/2 failed.
