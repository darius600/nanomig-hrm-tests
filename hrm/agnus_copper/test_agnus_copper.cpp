#include <iostream>
#include <vector>
#include <verilated.h>
#include "Vagnus_copper.h"

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_copper* top = new Vagnus_copper;
    
    top->clk = 0;
    top->clk7_en = 0;
    top->reset = 1;
    // Note: In some versions of the code, clk_ena is not an input.
    // If it's not an input, just don't set it. I will omit it.
    top->ecs = 0;
    // top->aga = 0; // Not an input in the viewed port list
    top->enadma = 1;
    top->ackdma = 0;
    top->hpos = 0;
    top->vpos = 0;
    top->blit_busy = 0;
    // top->cdang = 0; // Not an input
    
    // We will supply some instructions
    // WAIT for vpos=0x20, hpos=0x10.
    // Word 1: VP=0x20, HP=0x10. hpos is [8:2]. 0x10 is 16. 16/4 = 4. So HP=4. 
    // Word 1: 0x20 << 8 | (4 << 1) | 1 = 0x2009
    // Word 2: VE=0xFF, HE=0x7F, bit0=0 => 0xFFFF & ~1 = 0xFFFE
    // Then a MOVE to 0x180 (COLOR00), value 0x0F00
    // Word 1: 0x0180
    // Word 2: 0x0F00
    // Then a SKIP for vpos=0x21
    // Word 1: 0x2101
    // Word 2: 0xFFFF
    
    std::vector<uint16_t> memory = {
        0x2009, 0xFFFE, // WAIT v=0x20
        0x0180, 0x0F00, // MOVE COLOR00
        0x2001, 0xFF01, // SKIP if V >= 0x20 (mask out H bits)
        0x0182, 0x00F0, // MOVE COLOR01
        0xFFFF, 0xFFFE  // WAIT end of screen
    };
    int pc = 0;
    
    auto tick = [&]() {
        top->reg_address_in = top->reg_address_out;
        top->eval();
        top->clk = 1; top->eval();
        top->clk = 0; top->eval();
    };

    tick();
    top->reset = 0;
    
    int move_count = 0;
    
    for (int v=0; v<0x25; v++) {
        top->vpos = v;
        for (int h=0; h<454; h++) {
            top->hpos = h;
            
            top->clk7_en = 1;
            top->eval();
            top->reg_address_in = top->reg_address_out;
            top->eval();
            
            if (top->reqdma) {
                top->ackdma = 1;
                top->eval();
                
                int state = top->copper_state_dbg;
                
                if (top->reqdma && top->bus_ena_dbg) { // valid DMA cycle
                    uint32_t addr = top->address_out;
                    if (addr < memory.size()) {
                        top->data_in = memory[addr];
                    }
                    
                    if (state == 4 || state == 6) { // FETCH1, FETCH2
                        if (state == 6 && (memory[(addr - 1)] & 1) == 0) {
                            int reg = (memory[(addr - 1)] & 0x1FE);
                            // Verify that reg_address_out matches the register we're writing to!
                            // If it doesn't match, the write was skipped!
                            // wait! reg_address_out is a 10-bit vector from [10:1]. So (top->reg_address_out << 1)
                            if ((top->reg_address_out << 1) == reg) {
                                std::cout << "MOVE to reg " << std::hex << reg << " value " << std::hex << top->data_in << std::dec << "\n";
                                move_count++;
                            } else {
                                std::cout << "SKIPPED MOVE to reg " << std::hex << reg << "\n";
                            }
                        }
                    }
                }
            } else {
                top->ackdma = 0;
            }
            
            tick();
            
            top->clk7_en = 0;
            tick(); tick(); tick();
        }
    }
    
    if (move_count != 1) { // Only COLOR00 should be written, COLOR01 is skipped!
        std::cout << "FAIL: Expected 1 MOVE due to SKIP, got " << move_count << "\n";
        return 1;
    }

    std::cout << "PASS: Copper tests passed.\n";
    return 0;
}
