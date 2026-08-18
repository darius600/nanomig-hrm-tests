#include <iostream>
#include <verilated.h>
#include "Vagnus_diskdma.h"

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_diskdma* top = new Vagnus_diskdma;
    
    top->clk = 0;
    top->clk7_en = 0;
    top->dmal = 1; // Always request
    top->dmas = 1; // Read mode
    top->speed = 0; // standard speed
    top->reg_address_in = 0;
    top->data_in = 0;
    
    auto tick = [&]() {
        top->eval();
        top->clk = 1; top->eval();
        top->clk = 0; top->eval();
    };

    // Initialize pointer to 0x1000
    top->clk7_en = 1;
    
    top->reg_address_in = 0x020 >> 1; // DSKPTH
    top->data_in = 0;
    tick();
    
    top->reg_address_in = 0x022 >> 1; // DSKPTL
    top->data_in = 0x1000;
    tick();
    
    top->reg_address_in = 0;

    int dma_count = 0;
    int expected_ptr = 0x1000/2; // Word address (address_out is word-based)
    
    for(int hpos=0; hpos<456; hpos++) {
        top->hpos = hpos;
        top->eval();
        
        if (top->dma) {
            std::cout << "Disk DMA at hpos=" << hpos << " -> ptr=" << top->address_out << "\n";
            dma_count++;
            
            if (hpos != 25 && hpos != 29 && hpos != 33) {
                std::cout << "FAIL: Invalid disk DMA slot.\n";
                return 1;
            }
            
            if (top->address_out != expected_ptr) {
                std::cout << "FAIL: Expected ptr " << expected_ptr << " got " << top->address_out << "\n";
                return 1;
            }
            expected_ptr++; // Increments per word fetched
        }
        tick();
    }
    
    if (dma_count != 3) {
        std::cout << "FAIL: Expected 3 disk slots, got " << dma_count << "\n";
        return 1;
    }

    std::cout << "PASS: Disk DMA tests passed.\n";
    return 0;
}
