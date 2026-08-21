// Test purpose: Check the HRM stereo routing and retain a separate NanoMig mixer-arithmetic regression.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 5, printed p. 137.
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

    bool hrm_pass = true;
    auto check_hrm = [&](const std::string &name,
                         int s0, int s1, int s2, int s3,
                         int expected_left, int expected_right) {
        const Stereo got = mix(s0, s1, s2, s3, 1, 1, 1, 1);
        if (got.left != expected_left || got.right != expected_right) {
            std::cerr << "HRM FAIL " << name
                      << ": expected L=" << expected_left << " R=" << expected_right
                      << ", got L=" << got.left << " R=" << got.right << "\n";
            hrm_pass = false;
        } else {
            std::cout << "HRM PASS " << name << "\n";
        }
    };

    // HRM p. 137: channels 1+2 feed LEFT; channels 0+3 feed RIGHT.
    check_hrm("channel 0 routes right", 17, 0, 0, 0, 0, 17);
    check_hrm("channel 1 routes left", 0, 23, 0, 0, 23, 0);
    check_hrm("channel 2 routes left", 0, 0, 31, 0, 31, 0);
    check_hrm("channel 3 routes right", 0, 0, 0, 47, 0, 47);

    // Secondary RTL regression: preserve the implementation's signed arithmetic
    // independently of whether its left/right labels satisfy the HRM.
    bool arithmetic_pass = true;
    const Stereo arithmetic = mix(100, 50, 10, -50, 63, 32, 63, 32);
    if (arithmetic.left != 4700 || arithmetic.right != 2230) {
        std::cerr << "RTL REGRESSION FAIL: expected current mixer sums L=4700 R=2230, got L="
                  << arithmetic.left << " R=" << arithmetic.right << "\n";
        arithmetic_pass = false;
    } else {
        std::cout << "RTL REGRESSION PASS: signed scaling and summation vector.\n";
    }

    std::cout << "HRM routing=" << (hrm_pass ? "PASS" : "FAIL")
              << ", arithmetic regression=" << (arithmetic_pass ? "PASS" : "FAIL") << "\n";

    delete dut;
    return hrm_pass && arithmetic_pass ? 0 : 1;
}
