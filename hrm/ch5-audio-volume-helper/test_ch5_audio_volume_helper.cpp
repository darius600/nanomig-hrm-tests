// Test purpose: Assert the HRM's stated audio volume-scaling facts - AUDxDAT
//   bytes read as two's complement, and AUDxVOL bits 5-0 selecting a linear
//   64-level field including its "no output" zero level - against the
//   six-bit paula_audio_volume multiply helper, citing Appendix A
//   quotations by page and claim id.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Appendix
//   A, printed pp. 265-266.
// Non-goals: VOLUME 64 IS OUT OF SCOPE AT THIS BOUNDARY, AND THAT IS
//   LOAD-BEARING, NOT AN OVERSIGHT. paula_audio_volume takes volume[5:0] -
//   six bits, 0-63. Appendix A printed p.266 describes 65 linear levels
//   (0-64) with bit 6 forcing maximum, and chapter 5's own printed p.140
//   gives a volume range of 0-64 (see BOUNDARIES.md item 1). Writing 64 into
//   a six-bit field is writing 0 (64 mod 64 = 0); driving 64 into this
//   Verilated port unmasked gives 127*64=8128, a product this hardware
//   cannot construct - the fixture being wrong, not the design (see
//   VOLUME-64-OUT-OF-SCOPE below). Volume 64 is tested, red, at the
//   mixer/PWM boundary in hrm/ch5-audio-volume-scale; it is not asserted,
//   attempted or worked around here. This package also does not touch
//   sample_okk, the PWM-gated second volume path on paula_audio_channel
//   (BOUNDARIES.md item 0) - a different port of a different module,
//   entirely outside paula_audio_volume's boundary.
// Claim ids: ids such as `5-appA-p265.c40` and `A-audxdat.c3` name one
//   sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch5.json lists under `inventories`.
// RTL AND EXPECTED VALUES: paula_audio_volume.v (24 lines) supplies the
//   wiring the labels cite; every expected value below is taken from an
//   Appendix A quotation named by page, never from what the RTL happens to
//   do. Where the manual's range
//   (0-64) and the port's width (0-63) disagree, the disagreement is
//   declared out of scope above, not silently re-based on the RTL.
//
// Claims covered (work/ch5_packages/ch5-audio-volume-helper.jsonl, all 6):
//   TWOS-COMPLEMENT-SAMPLE  : 5-appA-p265.c40, A-audxdat.c3
//   VOLUME-64-LEVELS-LINEAR : 5-appA-p266.c35, A-audxvol.b05_00
//   VOLUME-ZERO-NO-OUTPUT   : 5-appA-p266.c36, A-audxvol.b05_00.zero
//
// FIXTURE. paula_audio_volume is purely combinational (no clk, no clk7_en) -
// every input drives out directly through eval(), so there is no DMA/latch
// handshake for a silence trap to hide in, the way paula_audio_channel's
// fixtures have one; every assertion below IS its own
// data-delivery proof (a wrong or dead wiring would show up as a wrong `out`
// on the very first case), so no separate fixture-liveness block is added.
//
// Every mutation below was actually applied (edit -> rebuild -> run ->
// revert -> rebuild-clean) and the FAIL/PASS lines quoted are what the run
// printed, not predictions.
//
// MUTATION TWOS-COMPLEMENT-SAMPLE stimulus red: the first stimulus sample
//   changed from -100 to -99 (the hardcoded expected -100 left untouched) ->
//   FAIL, actual -99 != expected -100; every other assertion PASS.
// MUTATION VOLUME-64-LEVELS-LINEAR stimulus red: the fixed sample fed to the
//   sweep changed from 127 to 126 (the expected table, computed as
//   127*volume, left untouched) -> FAIL, mismatches at every volume above 0;
//   every other assertion PASS.
// MUTATION VOLUME-ZERO-NO-OUTPUT stimulus red: the volume fed for one of the
//   zero-volume samples (-1) changed from 0 to 1 (the expected 0 for that
//   sample left untouched) -> FAIL, actual -1 != expected 0; every other
//   assertion PASS.
//
// VOLUME-64-OUT-OF-SCOPE is a note(), not an assertion: a real DUT
//   observation (eval_case(127, 64) against the Verilated module, not host
//   arithmetic such as `64 & 0x3F == 0`), printed as MEASURED, carrying no
//   claim id, no pass/fail, and no mutation record -- a row the manual
//   does not rule on gets a recorded number, not an assertion. See the block's own
//   comment, just above it, for why neither candidate expected value (the
//   0 a real 6-bit register write would truncate to, or the 8128 this
//   fixture's unmasked port actually produces) has an HRM oracle behind it.

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vpaula_audio_volume.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    auto* dut = new Vpaula_audio_volume;
    Checks checks;

    auto eval_case = [&](int sample, int volume) -> int {
        dut->sample = static_cast<uint8_t>(sample);
        dut->volume = static_cast<uint8_t>(volume);
        dut->eval();
        int actual = dut->out;
        if (actual & 0x2000) actual |= ~0x3FFF;  // sign-extend the 14-bit product
        return actual;
    };

    // ---------------- TWOS-COMPLEMENT-SAMPLE (appendix A p.265) ----------------
    {
        const int neg100 = eval_case(-100, 1);
        const int pos100 = eval_case(100, 1);
        const int neg1 = eval_case(-1, 1);
        checks.expect(neg100 == -100 && pos100 == 100 && neg1 == -1,
            "TWOS-COMPLEMENT-SAMPLE: sample=-100 (byte 0x9C) with volume=1 reads "
            "out=-100, not +156 (which is what reading 0x9C as an UNSIGNED byte would "
            "give); its mirror sample=+100 reads out=+100; sample=-1 (byte 0xFF) "
            "reads out=-1, not +255 - 'It contains 2 bytes of data that are each 2's "
            "complement and are outputted sequentially (with digital-to-analog "
            "conversion) to the audio output pins' (5-appA-p265.c40, A-audxdat.c3, "
            "appendix A, printed p.265) - paula_audio_volume.v:16 "
            "`sesample[13:0] = {{6{sample[7]}},sample[7:0]}` sign-extends the sample "
            "byte's own bit 7 before the multiply, which is exactly what reading it "
            "as two's complement means.");
    }

    // ---------------- VOLUME-64-LEVELS-LINEAR (appendix A p.266) ----------------
    {
        bool linear_ok = true;
        std::vector<int> results;
        for (int volume = 0; volume <= 63; ++volume) {
            const int actual = eval_case(127, volume);
            const int expected = 127 * volume;
            if (actual != expected) linear_ok = false;
            results.push_back(actual);
        }
        std::vector<int> distinct = results;
        std::sort(distinct.begin(), distinct.end());
        distinct.erase(std::unique(distinct.begin(), distinct.end()), distinct.end());
        checks.expect(linear_ok && distinct.size() == 64,
            "VOLUME-64-LEVELS-LINEAR: sweeping volume[5:0] over its full range, "
            "000000 through 111111, against a fixed sample=127 produces out == "
            "127*volume at every one of the 64 settings, and all 64 results are "
            "numerically distinct from each other - 'Sets one of 64 levels "
            "(000000=no output' (5-appA-p266.c35, A-audxvol.b05_00, appendix A, "
            "printed p.266) - paula_audio_volume.v:17,20 scale sample linearly by "
            "the plain unsigned value of volume[5:0]. Volume 64 itself is out of "
            "scope at this six-bit port - see file header and "
            "hrm/ch5-audio-volume-scale.");
    }

    // ---------------- VOLUME-ZERO-NO-OUTPUT (appendix A p.266) ----------------
    {
        const int samples[] = {-128, -1, 0, 1, 127};
        bool zero_ok = true;
        for (int s : samples) {
            if (eval_case(s, 0) != 0) zero_ok = false;
        }
        bool control_ok = true;
        for (int s : samples) {
            if (s == 0) continue;
            if (eval_case(s, 1) != s) control_ok = false;
        }
        checks.expect(zero_ok && control_ok,
            "VOLUME-ZERO-NO-OUTPUT: with volume[5:0]=000000, samples -128, -1, 0, 1 "
            "and 127 all read out=0 - 'Sets one of 64 levels (000000=no output' "
            "(5-appA-p266.c36, A-audxvol.b05_00.zero, appendix A, printed p.266); the "
            "same non-zero samples read out==sample at volume=1 (the control case, "
            "same measurement basis), so the zero reading above is volume acting on "
            "the signal, not a stuck-low fixture.");
    }

    // ---------------- VOLUME-64-OUT-OF-SCOPE (boundary demonstration, a note, not an assertion) ----------------
    // Appendix A gives no ruling for register values above 64 at this
    // six-bit port, so there is no manual oracle for a pass/fail here, and a
    // row the manual does not rule on gets a recorded number, not a
    // pass/fail. Host arithmetic (`64 & 0x3F` computed in C++) would never
    // apply 64 to the Verilated module at all, and its mutation would change
    // only a host constant. The note below instead drives volume=64 into the
    // DUT's volume[5:0] port via eval_case() and reports what
    // paula_audio_volume.v itself produces, which is informative on its
    // own: the Verilated harness does NOT itself truncate an out-of-range
    // value to the port's declared width (unlike a real hardware register
    // write, which would), so 64 passes through unmasked and multiplies as
    // if it were 64, not 0 -- the 127*64=8128 artifact, not a finding.
    // Asserting either 0 (the truncation a real
    // register write would perform) or 8128 (what this fixture happens to
    // pass through) would both be asserting a fact with no HRM oracle behind
    // it, so this stays a note(), with no claim id and no mutation record --
    // matching hrm/ch4-attached-sprites/'s own convention for note()s.
    {
        const int at_64 = eval_case(127, 64);
        const int at_0 = eval_case(127, 0);
        checks.note(
            "VOLUME-64-OUT-OF-SCOPE [boundary demonstration, not an HRM "
            "claim, not asserted - volume 64 is not asserted at this "
            "boundary]: driving volume=64 directly into paula_audio_volume's "
            "six-bit volume[5:0] port (a real Verilated eval() of the DUT, "
            "not host arithmetic) with sample=127 gives out=" +
            std::to_string(at_64) + " (== 127*64, i.e. unmasked - compare "
            "out=" + std::to_string(at_0) + " for volume=0, which is what a "
            "real six-bit hardware register write would have reduced 64 to "
            "before this module ever saw it). The Verilated harness does not "
            "enforce the port's declared width the way a real register write "
            "does, so this is the 127*64=8128 artifact, not a finding - a "
            "fixture boundary fact, not a "
            "hardware behaviour the manual rules on. Volume 64 is asserted, "
            "red, at the mixer/PWM boundary in hrm/ch5-audio-volume-scale "
            "instead, where the DUT's own OR-collapse (not fixture leakage) "
            "is what is measured.");
    }

    std::cout << "Checked " << (checks.passed + checks.failed)
              << " assertions; passed=" << checks.passed
              << " failed=" << checks.failed << ".\n";
    // The line the coverage ledger and hrm/run_all.sh read. The line above
    // says the same thing for a human; this one is the spelling the tools
    // grep for, and without it this package reports no assertions at all.
    std::cout << checks.passed << " passed, " << checks.failed
              << " failed.\n";

    delete dut;
    return checks.failed == 0 ? 0 : 1;
}
