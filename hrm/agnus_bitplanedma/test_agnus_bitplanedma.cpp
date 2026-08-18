#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vagnus_bitplanedma.h"
#include <iostream>
#include <vector>
#include <map>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    Vagnus_bitplanedma* top = new Vagnus_bitplanedma;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("trace.vcd");
    
    int time_step = 0;
    int fail_count = 0;
    
    // Initialize
    top->clk = 0;
    top->clk7_en = 1;
    top->reset = 1;
    top->harddis = 0;
    top->aga = 0;
    top->ecs = 1;
    top->a1k = 0;
    top->sof = 0;
    top->dmaena = 0;
    top->vpos = 0;
    top->hpos = 0;
    top->reg_address_in = 0;
    top->data_in = 0;
    
    auto tick = [&]() {
        top->eval();
        top->clk = 1; top->eval();
        if (tfp) tfp->dump(time_step++);
        top->clk = 0; top->eval();
        if (tfp) tfp->dump(time_step++);
    };

    // Reset sequence
    for (int i = 0; i < 10; i++) tick();
    top->reset = 0;
    for (int i = 0; i < 10; i++) tick();

    auto write_reg = [&](uint16_t reg, uint16_t data) {
        top->reg_address_in = reg >> 1;
        top->data_in = data;
        top->eval();
        tick();
        top->reg_address_in = 0;
        top->data_in = 0;
        top->eval();
        tick();
    };

    auto run_dma_test = [&](uint16_t bplcon0, uint16_t ddfstrt, uint16_t ddfstop, bool ecs, const char* name, int expected_dmas) {
        std::cout << "DEBUG: " << name << " - start\n";
        
        top->ecs = ecs ? 1 : 0;
        
        write_reg(0x08E, 0x2C81); // DIWSTRT
        write_reg(0x090, 0x2CC1); // DIWSTOP
        write_reg(0x092, ddfstrt); // DDFSTRT
        write_reg(0x094, ddfstop); // DDFSTOP
        write_reg(0x100, bplcon0); // BPLCON0
        write_reg(0x1FC, 0x0000); // FMODE
        
        top->dmaena = 1;
        
        int dma_count = 0;
        std::map<int, int> plane_counts;

        // Run from vpos=0 to vpos=0x50, full lines
        for (int v = 0; v <= 0x50; v++) {
            top->vpos = v;
            for (int h = 0; h < 456; h++) {
                top->hpos = h;
                tick();
                
                // In the cycle where dma is asserted, it's valid
                // But we only count for the line 0x50
                if (v == 0x50 && top->dma) {
                    dma_count++;
                    int reg = (top->reg_address_out << 1);
                    plane_counts[reg]++;
                }
            }
        }
        
        top->dmaena = 0;
        
        std::cout << "DEBUG: " << name << " - DMA count = " << dma_count << "\n";
        for (auto const& [reg, count] : plane_counts) {
            std::cout << "  Reg 0x" << std::hex << reg << std::dec << " fetched " << count << " times\n";
        }
        
        if (dma_count != expected_dmas) {
            std::cout << "FAIL: " << name << " - Expected " << expected_dmas << " DMAs, got " << dma_count << "\n";
            fail_count++;
            return false;
        }
        
        std::cout << "PASS: " << name << "\n";
        return true;
    };

    // Lowres, 1 bitplane (BPU=1 -> 0x1200)
    // 0xD0 - 0x38 = 0x98 (152). 152 / 8 + 1 = 20 words per line.
    // 1 plane * 20 = 20 DMAs.
    run_dma_test(0x1200, 0x0038, 0x00D0, false, "Lowres 1 plane", 20);

    // Lowres, 4 bitplanes (BPU=4 -> 0x4200)
    // 4 planes * 20 = 80 DMAs.
    run_dma_test(0x4200, 0x0038, 0x00D0, false, "Lowres 4 planes", 80);

    // Lowres, 6 bitplanes (BPU=6 -> 0x6200)
    // 6 planes * 20 = 120 DMAs.
    run_dma_test(0x6200, 0x0038, 0x00D0, false, "Lowres 6 planes", 120);

    // Hires, 2 bitplanes (BPU=2, HIRES=1 -> 0xA200)
    // 0xD4 - 0x3C = 0x98 (152). 152 / 4 + 1 = 39 ?
    // Wait, DDF step is 4 CCKs in hires. 
    // Let's just run it and observe if it fails, then adjust expected if it makes sense according to HRM.
    run_dma_test(0xA200, 0x003C, 0x00D4, false, "Hires 2 planes", 40 * 2);

    if (fail_count == 0) {
        std::cout << "PASS: All Bitplane DMA cases tested successfully.\n";
    }
    
    top->final();
    tfp->close();
    delete top;
    delete tfp;
    return fail_count == 0 ? 0 : 1;
}
