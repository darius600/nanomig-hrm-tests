#include <iostream>
#include <iomanip>
#include "Vpaula_audio_volume.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vpaula_audio_volume* dut = new Vpaula_audio_volume;

    std::cout << "Starting FPGA-041: Paula Audio Volume Tests...\n";

    auto test_vol = [&](int8_t sample, uint8_t volume) -> bool {
        dut->sample = (uint8_t)sample;
        dut->volume = volume & 0x3F; // hardware input is 6 bits
        dut->eval();
        
        int16_t expected = (int16_t)sample * (int16_t)dut->volume;
        // The output is 14 bits signed. We sign-extend it to 16 bits for comparison.
        int16_t actual = dut->out;
        if (actual & 0x2000) { // bit 13 is sign bit
            actual |= 0xC000;
        }

        if (expected != actual) {
            std::cerr << "FAIL: Sample " << (int)sample << " * Volume " << (int)dut->volume 
                      << " = Expected " << expected << ", Got " << actual << "\n";
            return false;
        }
        return true;
    };

    bool pass = true;

    // Test max positive sample, max volume (63 because hardware input is 6 bits, wait! Amiga says 64)
    // Minimig passes volume[5:0] to this module, so max is 63. Let's test 63.
    pass &= test_vol(127, 63);
    pass &= test_vol(127, 0);
    pass &= test_vol(-128, 63);
    pass &= test_vol(-128, 0);
    
    pass &= test_vol(50, 32);
    pass &= test_vol(-50, 32);

    if (pass) {
        std::cout << "PASS: All tests passed.\n";
    }

    delete dut;
    return pass ? 0 : 1;
}
