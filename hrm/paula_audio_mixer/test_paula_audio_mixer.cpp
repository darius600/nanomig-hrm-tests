// Test purpose: Guard the NanoMig mixer arithmetic. The HRM stereo routing check was withdrawn on 07.09.2026 - see the note in main().
// Test class: RTL_REGRESSION
// Primary oracle: the current NanoMig mixer arithmetic. The manual contradicts itself on the stereo assignment, so it is not an oracle here.
// Non-goals: This does not claim numeric DAC scaling for HRM volume-register value 64.

#include <cstdint>
#include <iostream>
#include <string>
#include <verilated.h>
#include "Vpaula_audio_mixer.h"

struct Stereo {
    int left;
    int right;
};

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

    // Four stereo-routing checks stood here until 07.09.2026, recorded as finding
    // FPGA-051. They asserted chapter 5 p.137: "Channels 1 and 2 are connected to
    // the left-side stereo output jack. Channels 0 and 3 are connected to the
    // right-side output jack."
    //
    // Withdrawn, not fixed. The manual states the opposite in its appendix pin
    // list: "AUDO - Audio output from left (channels 0, 3) port" and "AUDI -
    // Audio input to right (channels 1, 2) port". Both statements appear in both
    // the printed and the Developer-CD edition, so this is a contradiction inside
    // the oracle, not a deviation from it. paula_audio_mixer.v implements the pin
    // list.
    //
    // Re-asserting either side means picking a page. A third witness is needed
    // first - the same shape as the chapter-2 conflict between p.21 and p.26,
    // which appendix B's marker column settled. Until then the routing is left
    // unasserted rather than asserted wrongly, which is what made this fixture
    // red for three weeks.
    //
    // The mixer's arithmetic is a separate question from its left/right labels
    // and is unaffected; it is kept below.
    bool arithmetic_pass = true;
    const Stereo arithmetic = mix(100, 50, 10, -50, 63, 32, 63, 32);
    if (arithmetic.left != 4700 || arithmetic.right != 2230) {
        std::cerr << "RTL REGRESSION FAIL: expected current mixer sums L=4700 R=2230, got L="
                  << arithmetic.left << " R=" << arithmetic.right << "\n";
        arithmetic_pass = false;
    } else {
        std::cout << "RTL REGRESSION PASS: signed scaling and summation vector.\n";
    }

    std::cout << "HRM routing=not asserted (see the note above), "
              << "arithmetic regression=" << (arithmetic_pass ? "PASS" : "FAIL") << "\n";

    delete dut;
    return arithmetic_pass ? 0 : 1;
}
