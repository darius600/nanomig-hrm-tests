#include <verilated.h>
#include "Vagnus_blitter_zero.h"
#include <iostream>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_blitter_zero* top = new Vagnus_blitter_zero;

    int fail_count = 0;
    
    top->clk = 0;
    top->clk7_en = 1;
    top->enable = 0;
    top->init = 0;
    top->store_result = 0;
    top->fill_out = 0;
    
    auto tick = [&]() {
        top->clk = 1; top->eval();
        top->clk = 0; top->eval();
    };
    
    // Initialize inputs before ticking
    top->eval();

    // Test 1: Init to 1
    top->enable = 1; top->init = 1; tick();
    if (top->zero != 1) { std::cout << "FAIL: Init (zero=" << (int)top->zero << ")\n"; fail_count++; }
    top->init = 0;
    
    // Test 2: Store 0 does not change zero flag
    top->store_result = 1; top->fill_out = 0x0000; tick();
    if (top->zero != 1) { std::cout << "FAIL: Store 0 (zero=" << (int)top->zero << ")\n"; fail_count++; }
    
    // Test 3: Store non-zero clears zero flag
    top->store_result = 1; top->fill_out = 0x0001; tick();
    if (top->zero != 0) { std::cout << "FAIL: Store non-zero\n"; fail_count++; }
    
    // Test 4: Store 0 again does not restore zero flag
    top->store_result = 1; top->fill_out = 0x0000; tick();
    if (top->zero != 0) { std::cout << "FAIL: Store 0 after non-zero\n"; fail_count++; }
    
    if (fail_count == 0) {
        std::cout << "PASS: All zero flag cases tested successfully.\n";
    }
    
    top->final();
    delete top;
    return fail_count == 0 ? 0 : 1;
}
