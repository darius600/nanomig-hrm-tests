#include <verilated.h>
#include "Vagnus_blitter_adrgen.h"
#include <iostream>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_blitter_adrgen* top = new Vagnus_blitter_adrgen;

    int fail_count = 0;
    
    // Initialize
    top->clk = 0;
    top->clk7_en = 1;
    top->reset = 0;
    top->ptrsel = 0;
    top->modsel = 0;
    top->enaptr = 0;
    top->incptr = 0;
    top->decptr = 0;
    top->addmod = 0;
    top->submod = 0;
    top->data_in = 0;
    top->reg_address_in = 0;
    
    top->eval();
    
    auto tick = [&]() {
        top->clk = 1; top->eval();
        top->clk = 0; top->eval();
    };

    auto write_reg = [&](uint8_t addr_high, uint16_t data) {
        top->enaptr = 0;
        top->reg_address_in = addr_high;
        top->data_in = data;
        tick();
        top->reg_address_in = 0;
    };

    auto test_ptr = [&](bool inc, bool dec, uint32_t expected_word_addr, const char* name) {
        // Write Pointer A Low (0x052 -> 0x29) and High (0x050 -> 0x28)
        write_reg(0x28, 0x0000); // High = 0
        write_reg(0x29, 0x2000); // Low = 0x2000 (Byte addr), word addr = 0x1000

        // Enable ptr and apply inc/dec
        top->ptrsel = 2; // CHA = 2
        top->enaptr = 1;
        top->incptr = inc;
        top->decptr = dec;
        tick();
        
        // Read output
        top->enaptr = 1;
        top->ptrsel = 2;
        top->incptr = 0;
        top->decptr = 0;
        top->eval();

        uint32_t ptr_out = top->address_out;

        top->enaptr = 0;
        tick();
        
        if (ptr_out != expected_word_addr) {
            std::cout << "FAIL: " << name << " Expected " << std::hex << expected_word_addr << " Got " << ptr_out << "\n";
            fail_count++;
        }
    };

    test_ptr(1, 0, 0x1001, "Increment");
    test_ptr(0, 1, 0x0FFF, "Decrement");
    
    if (fail_count == 0) {
        std::cout << "PASS: All pointer cases tested successfully.\n";
    }
    
    top->final();
    delete top;
    return fail_count == 0 ? 0 : 1;
}
