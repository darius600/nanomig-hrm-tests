// Test purpose: Protect NanoMig sprite-DMA pointer, control, and data-fetch sequencing.
// Test class: RTL_REGRESSION
// Primary oracle: Frozen FPGA-022 specification and the NanoMig module interface contract.
// Non-goals: This does not establish complete HRM sprite timing or all eight channels.

#include <iostream>
#include <verilated.h>
#include "Vagnus_spritedma.h"

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_spritedma* top = new Vagnus_spritedma;
    
    top->clk = 0;
    top->clk7_en = 0;
    top->reset = 1;
    top->aga = 0;
    top->ecs = 0;
    top->ackdma = 0;
    top->vbl = 0;
    top->vblend = 0;
    top->hpos = 0;
    top->vpos = 0;
    top->reg_address_in = 0;
    top->data_in = 0;
    
    auto tick = [&]() {
        top->eval();
        top->clk = 1; top->eval();
        top->clk = 0; top->eval();
    };

    auto tick_hpos = [&]() {
        top->clk7_en = 1;
        tick();
        top->clk7_en = 0;
        tick();
        tick();
        tick();
    };

    tick();
    top->reset = 0;
    
    // Setup Sprite 0
    top->reg_address_in = 0x120 >> 1; // SPR0PTH
    top->data_in = 0;
    tick_hpos();
    
    top->reg_address_in = 0x122 >> 1; // SPR0PTL
    top->data_in = 0x2000;
    tick_hpos();
    
    top->reg_address_in = 0x140 >> 1; // SPR0POS
    top->data_in = 0x2000; // vstart = 0x20 (32)
    tick_hpos();
    
    top->reg_address_in = 0x142 >> 1; // SPR0CTL
    top->data_in = 0x2200; // vstop = 0x22 (34)
    tick_hpos();
    
    top->reg_address_in = 0;

    int pos_ctl_fetches = 0;
    int data_fetches = 0;

    for (int line=31; line<=35; line++) {
        top->vpos = line;
        top->vblend = (line == 31) ? 1 : 0;
        
        for (int h=0; h<454; h++) {
            top->hpos = h;
            
            top->clk7_en = 1;
            top->eval();
            
            if (top->reqdma) {
                top->ackdma = 1;
                top->eval();
                int reg_out = top->reg_address_out << 1;
                if (reg_out == 0x140 || reg_out == 0x142) {
                    pos_ctl_fetches++;
                    std::cout << "Line " << line << " hpos " << h << ": POS/CTL fetch, reg=" << std::hex << reg_out << std::dec << "\n";
                } else if (reg_out == 0x144 || reg_out == 0x146) {
                    data_fetches++;
                    std::cout << "Line " << line << " hpos " << h << ": DATA fetch, reg=" << std::hex << reg_out << std::dec << "\n";
                }
            } else {
                top->ackdma = 0;
            }
            tick();
            
            top->clk7_en = 0;
            tick();
            tick();
            tick();
        }
    }
    
    // Expect:
    // Line 31 (vblend): POS/CTL fetch (2 regs)
    // Line 32 (vstart): DATA fetch (2 regs)
    // Line 33 (body): DATA fetch (2 regs)
    // Line 34 (vstop): POS/CTL fetch (2 regs)
    // Line 35: nothing (or nothing for sprite 0)
    
    if (pos_ctl_fetches != 4 || data_fetches != 4) {
        std::cout << "FAIL: Expected 4 POS/CTL fetches and 4 DATA fetches. Got " 
                  << pos_ctl_fetches << " and " << data_fetches << "\n";
        return 1;
    }

    std::cout << "PASS: Sprite DMA tests passed.\n";
    return 0;
}
