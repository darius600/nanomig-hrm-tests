#include <iostream>
#include <iomanip>
#include "Vdenise_collision.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vdenise_collision* dut = new Vdenise_collision;

    std::cout << "Starting FPGA-031: Denise Collision Test...\n";

    dut->clk = 0;
    dut->clk7_en = 0;
    dut->reset = 1;
    dut->aga = 0;
    dut->reg_address_in = 0;
    dut->data_in = 0;
    dut->dblpf = 0;
    dut->bpldata = 0;
    dut->nsprite = 0;

    auto tick = [&]() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    };
    
    auto tick_7mhz = [&]() {
        dut->clk7_en = 1; tick();
        dut->clk7_en = 0; tick(); tick(); tick();
    };

    // Reset
    for(int i=0; i<4; i++) tick_7mhz();
    dut->reset = 0;
    for(int i=0; i<4; i++) tick_7mhz();

    // Helper to write register
    auto write_reg = [&](int addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick_7mhz();
        dut->reg_address_in = 0;
        tick_7mhz();
    };

    // Helper to read register
    auto read_reg = [&](int addr) -> uint16_t {
        dut->reg_address_in = addr;
        tick_7mhz();
        uint16_t out = dut->data_out;
        dut->reg_address_in = 0;
        tick_7mhz();
        return out;
    };

    std::cout << "Testing Sprite 0 to Sprite 2 Collision...\n";
    // We expect sprmatch[0] (Sprite 0/1) and sprmatch[1] (Sprite 2/3) to trigger cl[9].
    dut->nsprite = 0x05; // bits 0 (SPR0) and 2 (SPR2) set
    tick_7mhz();
    dut->nsprite = 0; // stop
    
    // Read CLXDAT (addr 0x00E = word addr 0x07, reg_address_in = 0x07)
    uint16_t clxdat = read_reg(0x07);
    if ((clxdat & 0x0200) == 0) { // bit 9 should be set
        std::cerr << "FAIL: Expected Sprite 0 to Sprite 2 collision (bit 9). CLXDAT = " << std::hex << clxdat << "\n";
        return 1;
    }
    
    // Ensure reading it cleared it
    tick_7mhz(); // Give it a cycle to clear
    clxdat = read_reg(0x07);
    if ((clxdat & 0x7FFF) != 0) { // top bit (15) is always 1 from CLXDAT? Actually data_out = {1'b1, clxdat[14:0]}
        std::cerr << "FAIL: CLXDAT did not clear after read! CLXDAT = " << std::hex << clxdat << "\n";
        return 1;
    }

    std::cout << "Testing Playfield 1 to Sprite 0 Collision...\n";
    // Odd bitplanes (Playfield 1) hit Sprite 0. That's cl[1].
    // CLXCON default is 0x0FFF.
    // This requires all 6 bitplanes to be 1 to match the default CLXCON.
    dut->bpldata = 0xFF; // all bitplanes hit
    dut->nsprite = 0x01; // SPR0 has hit
    tick_7mhz();
    dut->bpldata = 0;
    dut->nsprite = 0;
    
    clxdat = read_reg(0x07);
    if ((clxdat & 0x0002) == 0) { // bit 1 should be set
        std::cerr << "FAIL: Expected PF1 to SPR0 collision (bit 1). CLXDAT = " << std::hex << clxdat << "\n";
        return 1;
    }

    std::cout << "PASS: All tests passed.\n";
    delete dut;
    return 0;
}
