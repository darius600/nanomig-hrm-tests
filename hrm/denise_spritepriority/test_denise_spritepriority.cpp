#include <iostream>
#include <iomanip>
#include "Vdenise_spritepriority.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vdenise_spritepriority* dut = new Vdenise_spritepriority;

    std::cout << "Starting FPGA-033: Denise Sprite Priority Test...\n";

    auto eval = [&]() {
        dut->eval();
    };
    
    // Verilator maps input [2:1] nplayfield to uint8_t where bit 0 is [1] and bit 1 is [2].
    uint8_t PF1_ON = 0x01;
    uint8_t PF2_ON = 0x02;

    std::cout << "Testing: No sprite data\n";
    dut->bplcon2 = 0x00;
    dut->nsprite = 0x00;
    dut->nplayfield = 0x00;
    eval();
    if (dut->sprsel != 0) {
        std::cerr << "FAIL: sprsel must be 0 if no valid sprite data.\n";
        return 1;
    }

    std::cout << "Testing: PF1 has priority over Sprite 2\n";
    dut->bplcon2 = 0x01; // PF1 prio=1 (behind SPR0/1, in front of others). PF2 prio=0.
    dut->nsprite = 0x04; // SPR2 active (sprgroup 1, code 2)
    dut->nplayfield = PF1_ON;
    eval();
    // sprcode = 2. pf1front = 2 > 1 = 1. sprsel = 0 (PF).
    if (dut->sprsel != 0) { 
        std::cerr << "FAIL: PF1 should have priority over SPR2.\n";
        return 1;
    }

    std::cout << "Testing: Sprite 0 has priority over PF1\n";
    dut->nsprite = 0x01; // SPR0 active (sprgroup 0, code 1)
    // PF1_ON is still set.
    eval();
    // sprcode = 1. pf1front = 1 > 1 = 0. sprsel = 1 (Sprite).
    if (dut->sprsel != 1) { 
        std::cerr << "FAIL: SPR0 should have priority over PF1 (prio=1).\n";
        return 1;
    }

    std::cout << "Testing: PF2 has priority over Sprite 4\n";
    // PF2 prio = 2 (in front of SPR4/5/6/7)
    // PF2 prio is in bits [5:3]. So 2 << 3 = 16 = 0x10.
    dut->bplcon2 = 0x10; 
    dut->nsprite = 0x10; // SPR4 active (sprgroup 2, code 3)
    dut->nplayfield = PF2_ON;
    eval();
    // sprcode = 3. pf2front = 3 > 2 = 1. sprsel = 0 (PF).
    if (dut->sprsel != 0) { 
        std::cerr << "FAIL: PF2 should have priority over SPR4.\n";
        return 1;
    }

    std::cout << "Testing: Sprite 2 has priority over PF2\n";
    dut->nsprite = 0x04; // SPR2 active (sprgroup 1, code 2)
    // PF2_ON is still set.
    eval();
    // sprcode = 2. pf2front = 2 > 2 = 0. sprsel = 1 (Sprite).
    if (dut->sprsel != 1) {
        std::cerr << "FAIL: SPR2 should have priority over PF2 (prio=2).\n";
        return 1;
    }
    
    std::cout << "Testing: Multiple sprites active (SPR0 and SPR6) with PF1 in between\n";
    dut->bplcon2 = 0x03; // PF1 prio = 3 (behind SPR0-5, in front of SPR6/7)
    dut->nsprite = 0x41; // SPR0 and SPR6 active
    dut->nplayfield = PF1_ON;
    eval();
    // sprcode = 1. pf1front = 1 > 3 = 0. sprsel = 1.
    if (dut->sprsel != 1) {
        std::cerr << "FAIL: SPR0 should take priority, hiding PF1.\n";
        return 1;
    }
    
    std::cout << "Testing: Only SPR6 active, PF1 in front\n";
    dut->nsprite = 0x40; // SPR6 active (sprgroup 3, code 4)
    eval();
    // sprcode = 4. pf1front = 4 > 3 = 1. sprsel = 0.
    if (dut->sprsel != 0) {
        std::cerr << "FAIL: PF1 should take priority over SPR6.\n";
        return 1;
    }

    std::cout << "PASS: All tests passed.\n";
    delete dut;
    return 0;
}
