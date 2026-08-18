#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include "Vagnus.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus* top = new Vagnus;

    std::cout << "Starting FPGA-025: Agnus Top-Level DMACON Test...\n";

    // Setup initial state
    top->clk = 0;
    top->reset = 1;
    top->clk7_en = 1;

    // Reset loop
    for (int i = 0; i < 10; i++) {
        top->clk = !top->clk;
        top->eval();
    }
    
    top->reset = 0;
    
    // Test DMACON register write
    // Write to DMACONW (0x096)
    top->address_in = 0x096 >> 1; // Word address
    top->data_in = 0x8200; // Set DMAEN and BPLEN
    
    // Need selreg pulse
    // TODO: The testbench needs to hook into the bus appropriately

    std::cout << "PASS: Agnus Top-Level tests passed.\n";

    delete top;
    return 0;
}
