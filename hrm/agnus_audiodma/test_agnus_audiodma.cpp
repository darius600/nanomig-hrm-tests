#include <iostream>
#include <verilated.h>
#include "Vagnus_audiodma.h"

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_audiodma* top = new Vagnus_audiodma;
    
    top->clk = 0;
    top->clk7_en = 0;
    top->audio_dmal = 0;
    top->audio_dmas = 0;
    top->hpos = 0;
    top->reg_address_in = 0;
    top->data_in = 0;
    
    auto tick = [&]() {
        top->eval();
        top->clk = 1; top->eval();
        top->clk = 0; top->eval();
    };
    
    auto write_reg = [&](int addr, int data) {
        top->reg_address_in = addr >> 1;
        top->data_in = data;
        top->clk7_en = 1;
        tick();
        top->clk7_en = 0;
        top->reg_address_in = 0;
        tick();
    };
    
    // Write AUD0LCH (0x0A0) and AUD0LCL (0x0A2)
    write_reg(0x0A0, 0x0001);
    write_reg(0x0A2, 0x1234);
    
    top->audio_dmas = 0xF; 
    top->audio_dmal = 0xF; 
    
    bool ch0_seen = false;
    bool ch1_seen = false;
    
    // First line - will output 0 and reload pointer
    top->clk7_en = 1;
    for(int h=0; h<456; h++) {
        top->hpos = h;
        top->eval();
        top->clk = 1; top->eval();
        top->clk = 0; top->eval();
    }
    
    // Second line - should output the loaded pointer (0x11234) and then increment it
    top->audio_dmas = 0x0; 
    for(int h=0; h<456; h++) {
        top->hpos = h;
        top->eval();
        
        if (top->dma) {
            std::cout << "DMA active at hpos=" << h << " address_out=0x" << std::hex << (top->address_out << 1) << std::dec << " reg_out=0x" << std::hex << (top->reg_address_out << 1) << std::dec << "\n";
            if (h == 37) ch0_seen = true;
            if (h == 41) ch1_seen = true;
        }
        
        top->clk = 1; top->eval();
        top->clk = 0; top->eval();
    }
    
    if (ch0_seen && ch1_seen) {
        std::cout << "PASS: Audio DMA triggers correctly.\n";
        return 0;
    } else {
        std::cout << "FAIL: Audio DMA not triggered.\n";
        return 1;
    }
}
