#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "Vagnus.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus* top = new Vagnus;

    std::cout << "Starting FPGA-026: Agnus Top-Level Register Bus Test...\n";

    top->clk = 0;
    top->reset = 1;
    top->clk7_en = 1;
    top->aen = 0;
    top->rd = 0;
    top->hwr = 0;
    top->lwr = 0;
    top->data_in = 0;

    auto tick = [&]() {
        top->clk = 1;
        top->eval();
        top->clk = 0;
        top->eval();
    };

    for (int i = 0; i < 10; i++) tick();
    top->reset = 0;
    
    // --- TEST 1: CPU Write to Denise Register ---
    top->aen = 1;
    top->hwr = 1;
    top->lwr = 1;
    top->rd = 0;
    top->address_in = 0xC0; // 0x180 >> 1
    
    tick(); 
    
    std::cout << "Testing CPU Write to 0x180 (COLOR00)...\n";
    if (top->reg_address_out != 0xC0) {
        std::cerr << "FAIL: reg_address_out is " << std::hex << (int)top->reg_address_out 
                  << ", expected 0xC0\n";
        return 1;
    } else {
        std::cout << "PASS: CPU Write propagated to reg_address_out correctly.\n";
    }
    
    top->aen = 0; top->hwr = 0; top->lwr = 0;
    tick();

    if (top->reg_address_out != 0xFF) {
        std::cerr << "FAIL: reg_address_out is " << std::hex << (int)top->reg_address_out 
                  << ", expected 0xFF (idle)\n";
        return 1;
    }

    // --- TEST 2: Copper MOVE to Denise Register ---
    std::cout << "Testing Copper MOVE to 0x180 (COLOR00)...\n";
    
    auto write_reg = [&](int reg, int val) {
        top->aen = 1; top->hwr = 1; top->lwr = 1;
        top->address_in = reg >> 1;
        top->data_in = val;
        tick();
        top->aen = 0; top->hwr = 0; top->lwr = 0;
    };

    // Enable DMA
    write_reg(0x096, 0x8280); // DMAEN + COPEN
    write_reg(0x080, 0x0000); // COP1LCH
    write_reg(0x082, 0x1000); // COP1LCL
    write_reg(0x088, 0x0000); // COPJMP1

    bool fetched_ir1 = false;
    bool fetched_ir2 = false;
    bool moved = false;
    int timeout = 1000;
    
    while(timeout-- > 0) {
        // Agnus uses `cck` to allow odd slots (like Copper).
        // Let's toggle cck every 2 ticks (4 pixel ticks).
        // In Agnus, hpos increments by 1 every tick? No, hpos increments inside.
        // Actually we don't control hpos directly, Agnus does it.
        // But we DO control `cck`. `cck` is active whenever hpos[0] is high.
        // Wait, the input `cck` must be provided! We didn't provide it!
        // In NanoMig, `cck` toggles every clock cycle?
        top->cck = !top->cck; 
        
        tick();

        // Memory simulation
        if (top->address_out == (0x1000 >> 1)) {
            top->data_in = 0x0180; // MOVE to COLOR00
            fetched_ir1 = true;
        } else if (top->address_out == (0x1002 >> 1)) {
            top->data_in = 0x0FFF; // White
            fetched_ir2 = true;
        }
        
        // Check if reg_address_out is set to COLOR00 (0xC0) by Copper
        // Copper puts the address on the bus during the MOVE cycle
        if (top->reg_address_out == 0xC0) {
            moved = true;
            break;
        }
    }
    
    if (!fetched_ir1 || !fetched_ir2) {
        std::cerr << "FAIL: Copper never fetched instructions. ir1=" << fetched_ir1 << " ir2=" << fetched_ir2 << "\n";
        return 1;
    }
    
    if (!moved) {
        std::cerr << "FAIL: Copper never asserted reg_address_out for MOVE.\n";
        return 1;
    }

    std::cout << "PASS: Copper MOVE propagated to reg_address_out correctly.\n";
    std::cout << "PASS: All tests passed.\n";

    delete top;
    return 0;
}
