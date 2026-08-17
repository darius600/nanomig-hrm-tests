#include <verilated.h>
#include "Vagnus_blitter_mask.h"
#include <iostream>
#include <cstdint>
#include <vector>
#include <random>

uint16_t hrm_mask(bool isFirst, bool isLast, uint16_t fwm, uint16_t lwm, uint16_t a_in, uint16_t& expected_mask) {
    expected_mask = 0xFFFF;
    if (isFirst) expected_mask &= fwm;
    if (isLast)  expected_mask &= lwm;
    return a_in & expected_mask;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_blitter_mask* top = new Vagnus_blitter_mask;

    int pass_count = 0;
    int fail_count = 0;

    auto test_case = [&](bool first, bool last, uint16_t fwm, uint16_t lwm, uint16_t a_in) {
        top->first_word = first;
        top->last_word = last;
        top->bltafwm = fwm;
        top->bltalwm = lwm;
        top->bltadat = a_in;
        top->eval();

        uint16_t exp_mask;
        uint16_t exp_out = hrm_mask(first, last, fwm, lwm, a_in, exp_mask);

        if (top->mask_out != exp_mask || top->data_out != exp_out) {
            std::cout << "FAIL: first=" << first << " last=" << last 
                      << " fwm=" << std::hex << fwm << " lwm=" << lwm << " a_in=" << a_in
                      << " -> Expected mask=" << exp_mask << " out=" << exp_out
                      << " | Got mask=" << top->mask_out << " out=" << top->data_out << std::dec << "\n";
            fail_count++;
        } else {
            pass_count++;
        }
    };

    std::vector<uint16_t> a_patterns = {0x0000, 0xFFFF, 0x5555, 0xAAAA};
    std::vector<uint16_t> mask_patterns = {0x0000, 0xFFFF, 0x00FF, 0xFF00, 0x5555, 0xAAAA, 0x8000, 0x0001};

    // 1. Targeted Patterns
    for (int first = 0; first <= 1; first++) {
        for (int last = 0; last <= 1; last++) {
            for (uint16_t fwm : mask_patterns) {
                for (uint16_t lwm : mask_patterns) {
                    for (uint16_t a_in : a_patterns) {
                        test_case(first, last, fwm, lwm, a_in);
                    }
                }
            }
        }
    }

    // 2. Randomized Cases
    std::mt19937 rng(42);
    std::uniform_int_distribution<uint16_t> dist16(0, 0xFFFF);
    std::uniform_int_distribution<int> dist1(0, 1);

    for (int i = 0; i < 100000; i++) {
        test_case(dist1(rng), dist1(rng), dist16(rng), dist16(rng), dist16(rng));
    }

    if (fail_count == 0) {
        std::cout << "PASS: " << pass_count << " cases tested successfully.\n";
    } else {
        std::cout << "FAILED " << fail_count << " out of " << (pass_count + fail_count) << " cases.\n";
    }

    top->final();
    delete top;
    return fail_count == 0 ? 0 : 1;
}
