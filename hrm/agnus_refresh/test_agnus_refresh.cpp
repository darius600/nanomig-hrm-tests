#include <iostream>
#include <verilated.h>
#include "Vagnus_refresh.h"

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_refresh* top = new Vagnus_refresh;
    
    int dma_count = 0;
    for(int hpos=0; hpos<456; hpos++) {
        top->hpos = hpos;
        top->eval();
        
        if (top->dma) {
            std::cout << "Refresh DMA at hpos=" << hpos << "\n";
            dma_count++;
            
            if (hpos != 9 && hpos != 13 && hpos != 17 && hpos != 21) {
                std::cout << "FAIL: Invalid refresh DMA slot.\n";
                return 1;
            }
        }
    }
    
    if (dma_count != 4) {
        std::cout << "FAIL: Expected 4 refresh slots, got " << dma_count << "\n";
        return 1;
    }

    std::cout << "PASS: Refresh DMA tests passed.\n";
    return 0;
}
