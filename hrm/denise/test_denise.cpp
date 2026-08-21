// Test purpose: Smoke-test Denise register writes, display-window generation, and the final RGB pipeline together.
// Test class: INTEGRATION_SMOKE
// Primary oracle: NanoMig integration contract and the frozen FPGA-034/035/036 specification.
// Non-goals: The AGA BPLCON4 technique is not Third Edition HRM conformance evidence.

#include <iostream>
#include <iomanip>
#include "Vdenise.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vdenise* dut = new Vdenise;

    std::cout << "Starting FPGA-034/035/036: Denise Top-Level Test...\n";

    dut->clk = 0;
    dut->clk7_en = 0;
    dut->c1 = 0;
    dut->c3 = 0;
    dut->cck = 0;
    dut->reset = 1;
    dut->strhor = 0;
    dut->reg_address_in = 0;
    dut->data_in = 0;
    dut->chip48 = 0;
    dut->blank = 0;
    dut->a1k = 0;
    dut->ecs = 1; 
    dut->aga = 1; // Enable AGA to test BPLCON4 XOR

    int c1_c3_cck_cycle = 0;
    
    auto tick = [&]() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        
        c1_c3_cck_cycle = (c1_c3_cck_cycle + 1) % 4;
        dut->clk7_en = (c1_c3_cck_cycle == 0);
        dut->c1 = (c1_c3_cck_cycle == 1);
        dut->c3 = (c1_c3_cck_cycle == 3);
        dut->cck = (c1_c3_cck_cycle == 0 || c1_c3_cck_cycle == 1); 
    };

    for(int i=0; i<16; i++) tick();
    dut->reset = 0;
    for(int i=0; i<16; i++) tick();

    auto write_reg = [&](int addr, uint16_t data) {
        while(!dut->clk7_en) tick();
        
        dut->reg_address_in = addr;
        dut->data_in = data;
        
        tick(); tick(); tick(); tick();
        
        dut->reg_address_in = 0;
        dut->data_in = 0;
    };

    std::cout << "Testing Register Writes (FPGA-035)...\n";
    write_reg(0x47, 0x0040); // DIWSTRT (0x08E -> 0x47)
    write_reg(0x48, 0x0050); // DIWSTOP (0x090 -> 0x48)
    write_reg(0xF2, 0x0000); // DIWHIGH (0x1E4 -> 0xF2). Must write AFTER DIWSTOP to clear H9!
    
    write_reg(0xC0, 0x0F00); // Red
    write_reg(0xC1, 0x00F0); // Green
    write_reg(0x86, 0x0100); // BPLCON4 bplxor = 1 (bits 15-8)

    // Run one full line to let the window state machine sync up
    dut->strhor = 1;
    tick(); tick(); tick(); tick();
    dut->strhor = 0;
    for (int h = 2; h < 228; h++) {
        while(!dut->clk7_en) tick();
        tick();
    }
    
    std::cout << "Testing Video Output Pipeline (FPGA-036)...\n";
    
    while(!dut->clk7_en) tick();
    dut->strhor = 1;
    tick(); tick(); tick(); tick();
    dut->strhor = 0;
    
    bool saw_window = false;
    bool saw_border_before = false;
    bool saw_border_after = false;
    
    for (int h = 2; h < 0x60; h++) {
        while(!dut->clk7_en) tick();
        
        uint32_t rgb = (dut->red << 16) | (dut->green << 8) | dut->blue;
        std::cout << "hpos: " << h << " rgb: 0x" << std::hex << rgb << "\n";
        
        if (h < 0x40 && rgb == 0xFF0000) saw_border_before = true;
        if (h >= 0x43 && h < 0x50 && rgb == 0x00FF00) saw_window = true;
        if (h > 0x53 && rgb == 0xFF0000) saw_border_after = true;
        
        tick(); 
    }
    
    if (!saw_border_before) {
        std::cerr << "FAIL: Did not see COLOR00 (Red) in border before window.\n";
        return 1;
    }
    if (!saw_window) {
        std::cerr << "FAIL: Did not see COLOR01 (Green) inside window.\n";
        return 1;
    }
    if (!saw_border_after) {
        std::cerr << "FAIL: Did not see COLOR00 (Red) in border after window.\n";
        return 1;
    }

    std::cout << "PASS: All tests passed.\n";
    delete dut;
    return 0;
}
