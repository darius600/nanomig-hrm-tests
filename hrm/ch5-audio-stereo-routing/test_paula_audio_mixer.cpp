// Test purpose: Check the HRM stereo routing and retain a separate NanoMig mixer-arithmetic regression.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 5,
//   printed p. 137. The appendix E pin list, printed p. 326, contradicts
//   it, and three of the four independent implementations (WinUAE, Denise,
//   Copperline) take p. 326's side, vAmiga alone p. 137's - see
//   docs/findings/NM-HRM-002.md and docs/evidence/third-witness.md.
// Non-goals: This does not claim numeric DAC scaling for HRM volume-register
//   value 64.
// Claim ids: none. The four checks are named by the channel they route and by
//   NM-HRM-002, the finding they carry; the manual's two sentences are quoted
//   in the comment below.
//
// MUTATION ROUTE-TO-PARTNER stimulus green: the four routing checks are red at
//   baseline, so the proof runs the other way. MUTATE=ROUTE-TO-PARTNER drives
//   each sample on its partner channel (0<->1, 2<->3) with the expected side
//   unchanged, and all four go PASS -- 4 passed, 0 failed, against 0 passed,
//   4 failed without it. The checks measure which side a channel reaches.

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <verilated.h>
#include "Vpaula_audio_mixer.h"

struct Stereo {
    int left;
    int right;
};

// Counters for the summary line the runner and the coverage ledger read.
// They count only the four graded HRM routing checks below ("HRM PASS" /
// "HRM FAIL"); the arithmetic regression further down is not one of them.
// They do not gate anything: the exit code below still comes from the two
// booleans, so NM-HRM-002's four routing reds keep this package red.
static int g_total = 0, g_fail = 0;

static int sign_extend_15(int value) {
    return (value & 0x4000) ? (value | ~0x7FFF) : value;
}

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *dut = new Vpaula_audio_mixer;
    dut->clk7_en = 0;

    auto mix = [&](int s0, int s1, int s2, int s3,
                   int v0, int v1, int v2, int v3) -> Stereo {
        dut->clk = 0;
        dut->sample0 = static_cast<uint8_t>(s0);
        dut->sample1 = static_cast<uint8_t>(s1);
        dut->sample2 = static_cast<uint8_t>(s2);
        dut->sample3 = static_cast<uint8_t>(s3);
        dut->vol0 = v0;
        dut->vol1 = v1;
        dut->vol2 = v2;
        dut->vol3 = v3;
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        return {sign_extend_15(dut->ldatasum), sign_extend_15(dut->rdatasum)};
    };

    // The manual states the stereo assignment twice and contradicts itself:
    //
    //   chapter 5, p.137         channels 1+2 -> left, channels 0+3 -> right
    //   appendix E pin list,     "AUDO - Audio output from left (channels 0, 3) port",
    //   p.326                    "AUDI - Audio input to right (channels 1, 2) port"
    //
    // Both appear in both the printed and the Developer-CD edition, so the
    // manual alone cannot decide this. paula_audio_mixer.v follows p.326
    // (ldatasum = channels 0+3, rdatasum = channels 1+2), and so do three of
    // the four independent implementations:
    //
    //   WinUAE, audio.cpp L1153-L1165 at commit ccc407e, the stereo mix:
    //     data1 = datas[0] + datas[3];  data2 = datas[1] + datas[2];
    //     put_sound_word_right(data1);  put_sound_word_left(data2);
    //     The function is named for the right, but data1 is the FIRST sample
    //     of each stereo frame ("Always put the right word before the left
    //     word", L516), and the frame is played as KSAUDIO_SPEAKER_STEREO,
    //     whose first sample is the left speaker: channels 0 and 3 are heard
    //     on the left.
    //     github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/audio.cpp#L1153-L1165
    //
    //   Denise (emulation/libami/paula/paula.cpp L354-L355) and Copperline
    //   (src/chipset/paula.rs L2178-L2182) put channels 0 and 3 on the left
    //   too.
    //
    // vAmiga alone takes p.137's side:
    //
    //   vAmiga, Core/Ports/AudioPort.cpp L472-L473 at commit 35de004:
    //     r = sum(ch_i * pan_i), l = sum(ch_i * (1-pan_i)); pan is
    //     0.5*(sin(cfg*PI/200)+1) (L250) and the defaults are cfg =
    //     50/350/350/50 (Defaults.cpp L195-L198), giving pan =
    //     0.85/0.15/0.15/0.85 - channels 0 and 3 on the right.
    //     github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Ports/AudioPort.cpp#L472-L473
    //
    // NM-HRM-002 is filed against the page: the four checks below grade p.137
    // and stay red against it. Whether p.137 is a misprint is for an Amiga
    // specialist to say; see docs/findings/NM-HRM-002.md and
    // docs/evidence/third-witness.md.
    // MUTATION GUARD for four reds. An assertion red at baseline cannot be
    // made to fail harder; the proof that it measures the side a channel
    // reaches is a mechanism that turns it GREEN through the same comparator.
    // Under MUTATE=ROUTE-TO-PARTNER each sample is driven on its partner
    // channel (0<->1, 2<->3) -- the one this RTL puts on the side p.137
    // names -- and the expected side is left alone.
    const char *mut_env = std::getenv("MUTATE");
    auto mutating = [&](const char *id) {
        return mut_env != nullptr && std::strcmp(mut_env, id) == 0;
    };
    const bool to_partner = mutating("ROUTE-TO-PARTNER");

    bool hrm_pass = true;
    auto check_hrm = [&](const std::string &name,
                         int s0, int s1, int s2, int s3,
                         int expected_left, int expected_right) {
        const Stereo got = to_partner ? mix(s1, s0, s3, s2, 1, 1, 1, 1)
                                      : mix(s0, s1, s2, s3, 1, 1, 1, 1);
        ++g_total;
        if (got.left != expected_left || got.right != expected_right) {
            std::cerr << "HRM FAIL " << name
                      << ": expected L=" << expected_left << " R=" << expected_right
                      << ", got L=" << got.left << " R=" << got.right << "\n";
            hrm_pass = false;
            ++g_fail;
        } else {
            std::cout << "HRM PASS " << name << "\n";
        }
    };

    // All four are expected RED against p.137: this is NM-HRM-002, filed
    // against the page, not a broken assertion. See
    // docs/findings/NM-HRM-002.md and docs/evidence/third-witness.md - the
    // manual says both things, chapter 5 p.137 putting channels 1+2 left and
    // 0+3 right while the appendix pin list on p.326 says the opposite, and
    // three of the four independent implementations take p.326's side, as
    // this RTL does. The reds stay red against p.137: making these pass would
    // delete the finding, and no RTL change follows from it.
    check_hrm("channel 0 routes right [NM-HRM-002, expected red]", 17, 0, 0, 0, 0, 17);
    check_hrm("channel 1 routes left [NM-HRM-002, expected red]", 0, 23, 0, 0, 23, 0);
    check_hrm("channel 2 routes left [NM-HRM-002, expected red]", 0, 0, 31, 0, 31, 0);
    check_hrm("channel 3 routes right [NM-HRM-002, expected red]", 0, 0, 0, 47, 0, 47);

    // Secondary RTL regression: preserve the implementation's signed arithmetic
    // independently of whether its left/right labels satisfy the HRM. This is
    // not an HRM assertion -- the file header calls it "a separate NanoMig
    // mixer-arithmetic regression" -- so it prints as MEASURED:, which no
    // reader grades, and stays out of g_total/g_fail: a summary counts only
    // the graded lines the package printed.
    bool arithmetic_pass = true;
    const Stereo arithmetic = mix(100, 50, 10, -50, 63, 32, 63, 32);
    if (arithmetic.left != 4700 || arithmetic.right != 2230) {
        std::cerr << "MEASURED: RTL regression FAIL: expected current mixer sums L=4700 R=2230, "
                     "got L=" << arithmetic.left << " R=" << arithmetic.right << "\n";
        arithmetic_pass = false;
    } else {
        std::cout << "MEASURED: RTL regression PASS: signed scaling and summation vector.\n";
    }

    std::cout << "HRM routing=" << (hrm_pass ? "PASS" : "FAIL")
              << ", arithmetic regression=" << (arithmetic_pass ? "PASS" : "FAIL") << "\n";
    // The line hrm/run_all.sh and the coverage ledger read. The four routing
    // failures are NM-HRM-002 and are meant to be red; counting them here
    // publishes them instead of leaving this package with no assertions at
    // all in every total.
    std::cout << (g_total - g_fail) << " passed, " << g_fail
              << " failed.\n";

    delete dut;
    return hrm_pass && arithmetic_pass ? 0 : 1;
}
