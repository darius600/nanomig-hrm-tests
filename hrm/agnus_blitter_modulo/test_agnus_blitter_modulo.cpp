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

    // Helper to write register
    auto write_reg = [&](uint8_t addr_high, uint16_t data) {
        top->enaptr = 0;
        top->reg_address_in = addr_high;
        top->data_in = data;
        tick();
        top->reg_address_in = 0;
    };

    // Helper to test modulo operation
    auto test_mod = [&](uint16_t mod_val, bool add, bool sub, uint32_t expected_word_addr, const char* name) {
        // Write Modulo A (0x064 = 100_0110_0100 -> reg_address_in[8:1] = 0x32)
        write_reg(0x32, mod_val);
        
        // Write Pointer A Low (0x052 -> 0x29) and High (0x050 -> 0x28)
        write_reg(0x28, 0x0000); // High = 0
        write_reg(0x29, 0x2000); // Low = 0x2000 (Byte addr), word addr = 0x1000

        // Enable ptr and apply modulo
        top->ptrsel = 2; // CHA = 2
        top->modsel = 2; // CHA = 2
        top->enaptr = 1;
        top->addmod = add;
        top->submod = sub;
        tick();
        
        // Read output by selecting CHA (enaptr=1, ptrsel=2) but without add/sub
        top->enaptr = 1;
        top->ptrsel = 2;
        top->addmod = 0;
        top->submod = 0;
        top->eval();

        // Read output
        uint32_t ptr_out = top->address_out;

        // Reset for next test
        top->enaptr = 0;
        tick();
        if (ptr_out != expected_word_addr) {
            std::cout << "FAIL: " << name << " Expected " << std::hex << expected_word_addr << " Got " << ptr_out << "\n";
            fail_count++;
        }
    };

    test_mod(0x000A, 1, 0, 0x1005, "Positive Add");
    test_mod(0xFFF6, 1, 0, 0x0FFB, "Negative Add");
    test_mod(0x000A, 0, 1, 0x0FFB, "Positive Sub");
    test_mod(0xFFF6, 0, 1, 0x1005, "Negative Sub");
    
    if (fail_count == 0) {
        std::cout << "PASS: All modulo cases tested successfully.\n";
    }
    
    top->final();
    delete top;
    return fail_count == 0 ? 0 : 1;
}
