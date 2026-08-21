// Test purpose: Protect NanoMig palette expansion, lookup latency, and EHB output behavior.
// Test class: RTL_REGRESSION
// Primary oracle: Frozen FPGA-027 specification; OCS/EHB cases are HRM-constrained but the 24-bit AGA mapping is internal.
// Non-goals: This is not an AGA conformance test and does not cover all palette banks.

#include <iostream>
#include <iomanip>
#include "Vdenise_colortable.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vdenise_colortable* dut = new Vdenise_colortable;

    std::cout << "Starting FPGA-027: Denise Color Table Test...\n";

    dut->clk = 0;
    dut->clk7_en = 1;
    dut->rdram = 0;
    dut->bplxor = 0;
    dut->bank = 0;
    dut->loct = 0;
    dut->ehb_en = 0;
    dut->select = 0;
    
    auto tick = [&]() {
        dut->eval(); // Settle combinational logic before clock edge!
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    };

    // Test 1: Write OCS colors
    std::cout << "Testing Color Writes (OCS)...\n";
    dut->reg_address_in = 0xC1;
    dut->data_in = 0x456;
    tick(); 
    
    dut->reg_address_in = 0xC2;
    dut->data_in = 0xEEE;
    tick();

    dut->reg_address_in = 0x00; 
    tick();

    // Test 2: Read/Map Colors
    std::cout << "Testing Color Mapping...\n";
    dut->select = 1; // COLOR01
    tick();
    tick();

    if (dut->rgb != 0x445566) {
        std::cerr << "FAIL: COLOR01 expected 0x445566, got 0x" << std::hex << dut->rgb << "\n";
        return 1;
    }

    dut->select = 2; // COLOR02
    tick();
    tick();

    if (dut->rgb != 0xEEEEEE) {
        std::cerr << "FAIL: COLOR02 expected 0xEEEEEE, got 0x" << std::hex << dut->rgb << "\n";
        return 1;
    }

    // Test 3: EHB mode
    std::cout << "Testing EHB (Extra Half-Brite)...\n";
    dut->ehb_en = 1;
    dut->select = 2 + 32; // COLOR02 with EHB bit (5)
    tick();
    tick();

    if (dut->rgb != 0x777777) {
        std::cerr << "FAIL: EHB COLOR02 expected 0x777777, got 0x" << std::hex << dut->rgb << "\n";
        return 1;
    }

    std::cout << "PASS: All tests passed.\n";

    delete dut;
    return 0;
}
