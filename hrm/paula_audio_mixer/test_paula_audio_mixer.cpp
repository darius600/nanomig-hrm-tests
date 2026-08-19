#include <iostream>
#include <iomanip>
#include "Vpaula_audio_mixer.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vpaula_audio_mixer* dut = new Vpaula_audio_mixer;

    std::cout << "Starting Paula Audio Mixer Test...\n";

    bool pass = true;

    auto test_mix = [&](int s0, int s1, int s2, int s3, int v0, int v1, int v2, int v3, int expected_l, int expected_r) {
        dut->clk = 0;
        dut->sample0 = s0 & 0xFF;
        dut->sample1 = s1 & 0xFF;
        dut->sample2 = s2 & 0xFF;
        dut->sample3 = s3 & 0xFF;
        dut->vol0 = v0 & 0x7F;
        dut->vol1 = v1 & 0x7F;
        dut->vol2 = v2 & 0x7F;
        dut->vol3 = v3 & 0x7F;
        
        dut->eval();
        dut->clk = 1;
        dut->eval();
        
        // Output is 15-bit signed
        int got_l = dut->ldatasum;
        if (got_l & 0x4000) got_l |= ~0x3FFF; // sign extend 15-bit

        int got_r = dut->rdatasum;
        if (got_r & 0x4000) got_r |= ~0x3FFF; // sign extend 15-bit

        if (got_l != expected_l || got_r != expected_r) {
            std::cerr << "FAIL: s0=" << s0 << " v0=" << v0 
                      << " s3=" << s3 << " v3=" << v3 
                      << " -> Left expected " << expected_l << " got " << got_l << "\n";
            std::cerr << "FAIL: s1=" << s1 << " v1=" << v1 
                      << " s2=" << s2 << " v2=" << v2 
                      << " -> Right expected " << expected_r << " got " << got_r << "\n";
            pass = false;
        } else {
            std::cout << "PASS: Left=" << got_l << " Right=" << got_r << "\n";
        }
    };

    // Test cases
    // s0*v0 + s3*v3 = 100*63 + (-50)*32 = 6300 - 1600 = 4700
    // s1*v1 + s2*v2 = 50*32 + 10*63 = 1600 + 630 = 2230
    test_mix(100, 50, 10, -50, 64, 32, 64, 32, 4700, 2230);

    // Both positive max (127 * 63 = 8001; 8001 * 2 = 16002)
    test_mix(127, 127, 127, 127, 64, 64, 64, 64, 16002, 16002);

    // Both negative max (-128 * 63 = -8064; -8064 * 2 = -16128)
    test_mix(-128, -128, -128, -128, 64, 64, 64, 64, -16128, -16128);

    if (pass) {
        std::cout << "All tests passed!\n";
    }

    delete dut;
    return pass ? 0 : 1;
}
