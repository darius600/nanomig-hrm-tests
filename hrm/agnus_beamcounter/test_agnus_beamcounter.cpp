#include <iostream>
#include <verilated.h>
#include "Vagnus_beamcounter.h"

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_beamcounter* top = new Vagnus_beamcounter;
    
    top->clk = 0;
    top->clk7_en = 0;
    top->reset = 1;
    top->cck = 0;
    top->ntsc = 0; // 0=PAL, 1=NTSC
    top->a1k = 0;
    top->lace = 0;
    top->reg_address_in = 0;
    top->data_in = 0;
    
    auto tick = [&]() {
        top->eval();
        top->clk = 1; top->eval();
        top->clk = 0; top->eval();
    };

    // Apply reset
    for(int i=0; i<10; i++) {
        top->clk7_en = 1;
        tick();
    }
    top->reset = 0;

    int max_hpos = 0;
    int max_vpos = 0;
    int hpos_wrap_count = 0;
    
    for(int i=0; i<313*456*2; i++) {
        top->clk7_en = (i % 2 == 0); // 7MHz clock enable
        top->cck = (i % 4 < 2); // 3.5MHz clock
        
        top->eval();
        if (top->clk7_en) {
            if (top->hpos > max_hpos) max_hpos = top->hpos;
            if (top->vpos > max_vpos) max_vpos = top->vpos;
            if (top->eol) hpos_wrap_count++;
        }
        
        tick();
    }
    
    std::cout << "PAL Non-Interlaced: Max hpos=" << max_hpos << " Max vpos=" << max_vpos << " EOL_count=" << hpos_wrap_count << "\n";
    
    if (max_hpos != 453 || max_vpos != 312) {
        std::cout << "FAIL: PAL limits incorrect.\n";
        return 1;
    }

    std::cout << "PASS: Beamcounter tests passed.\n";
    return 0;
}
