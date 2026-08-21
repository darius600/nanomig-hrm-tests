// Test purpose: Exhaustively protect the six-bit NanoMig signed-sample times unsigned-volume helper.
// Test class: RTL_REGRESSION
// Primary oracle: The paula_audio_volume module contract; HRM Third Edition pp. 139-140 constrain only register-level volume.
// Non-goals: This helper cannot represent or prove the HRM volume-register value 64.

#include <cstdint>
#include <iostream>
#include <verilated.h>
#include "Vpaula_audio_volume.h"

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *dut = new Vpaula_audio_volume;

    int failures = 0;
    int cases = 0;

    for (int sample = -128; sample <= 127; ++sample) {
        for (int volume = 0; volume <= 63; ++volume) {
            dut->sample = static_cast<uint8_t>(sample);
            dut->volume = volume;
            dut->eval();

            const int expected = sample * volume;
            int actual = dut->out;
            if (actual & 0x2000) {
                actual |= ~0x3FFF;
            }

            ++cases;
            if (actual != expected) {
                if (failures < 8) {
                    std::cerr << "FAIL: sample=" << sample
                              << " volume=" << volume
                              << " expected=" << expected
                              << " actual=" << actual << "\n";
                }
                ++failures;
            }
        }
    }

    std::cout << "Checked " << cases
              << " helper-arithmetic combinations; failures=" << failures << ".\n";
    if (failures == 0) {
        std::cout << "PASS: Six-bit helper arithmetic is exhaustive and correct.\n";
    }

    delete dut;
    return failures == 0 ? 0 : 1;
}
