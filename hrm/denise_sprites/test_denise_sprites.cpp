// Test purpose: Protect NanoMig sprite arming, horizontal positioning, shifting, and attachment behavior.
// Test class: RTL_REGRESSION
// Primary oracle: Frozen FPGA-030 specification and the NanoMig module interface contract.
// Non-goals: This does not prove complete sprite DMA or all priority interactions.

#include <iostream>
#include <iomanip>
#include <vector>
#include "Vdenise_sprites.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vdenise_sprites* dut = new Vdenise_sprites;

    std::cout << "Starting FPGA-030: Denise Sprites Test...\n";

    dut->clk = 0;
    dut->clk7_en = 0;
    dut->c1 = 0;
    dut->c3 = 0;
    dut->reset = 1;
    dut->aga = 0;
    dut->spres = 0; // OCS sprites
    dut->reg_address_in = 0;
    dut->data_in = 0;
    dut->chip48 = 0;
    dut->hpos = 0;
    dut->sprena = 1; // enable sprites
    dut->esprm = 0;
    dut->osprm = 0;

    auto tick = [&]() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    };

    int phase = 0;
    auto tick_phase = [&]() {
        phase = (phase + 1) % 4;
        switch(phase) {
            case 0: dut->c1 = 0; dut->c3 = 0; dut->clk7_en = 1; break; // c1=0, c3=0 -> shift/load
            case 1: dut->c1 = 1; dut->c3 = 0; dut->clk7_en = 0; break; 
            case 2: dut->c1 = 1; dut->c3 = 1; dut->clk7_en = 0; break; // clk7n_en = 1
            case 3: dut->c1 = 0; dut->c3 = 1; dut->clk7_en = 0; break; 
        }
        tick();
    };

    auto tick_7mhz = [&]() {
        tick_phase();
        tick_phase();
        tick_phase();
        tick_phase();
    };

    // Reset
    for(int i=0; i<4; i++) tick_7mhz();
    dut->reset = 0;
    for(int i=0; i<4; i++) tick_7mhz();

    // Helper to write to a register
    auto write_reg = [&](int addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick_7mhz();
        dut->reg_address_in = 0;
        tick_7mhz();
    };

    std::cout << "Testing Sprite 0 Output & Priority...\n";
    // SPR0POS = 0x140. byte address 0x140 -> reg_address_in = 0xA0
    // hstart = 0x80. Top 8 bits = 0x40.
    write_reg(0xA0, 0x0040); // hstart[8:1] = 0x40 -> hstart = 0x80
    write_reg(0xA1, 0x0000); // SPR0CTL, attach=0, lsb=0
    write_reg(0xA3, 0x0000); // SPR0DATB = 0
    write_reg(0xA2, 0x8000); // SPR0DATA = 0x8000. Arms the sprite!

    bool saw_sprite = false;
    for (int h = 0; h < 256; h++) {
        dut->hpos = h;
        tick_7mhz();
        
        if (dut->sprdata != 0) {
            saw_sprite = true;
            if (h != 0x80 && h != 0x81 && h != 0x82) {
                std::cerr << "FAIL: Sprite appeared at hpos " << std::hex << h << ", expected ~0x80-0x82\n";
                return 1;
            }
            if (dut->sprdata != 1) {
                std::cerr << "FAIL: Expected sprdata=1, got " << (int)dut->sprdata << "\n";
                return 1;
            }
        }
    }

    if (!saw_sprite) {
        std::cerr << "FAIL: Never saw sprite output!\n";
        return 1;
    }

    std::cout << "Testing Sprite Attachment (Sprite 0 + Sprite 1)...\n";
    // Write SPR1: same position, attach=1
    // SPR1 is at 0x148 -> 0xA4
    write_reg(0xA4, 0x0040); // SPR1POS
    write_reg(0xA5, 0x0080); // SPR1CTL: bit 7 (attach) = 1
    write_reg(0xA7, 0x8000); // SPR1DATB = 0x8000
    write_reg(0xA6, 0x8000); // SPR1DATA = 0x8000 (Arms SPR1)
    
    // Rearm SPR0
    write_reg(0xA3, 0x0000);
    write_reg(0xA2, 0x8000);
    
    saw_sprite = false;
    for (int h = 0; h < 256; h++) {
        dut->hpos = h;
        tick_7mhz();
        if (dut->sprdata != 0) {
            saw_sprite = true;
            if (dut->sprdata != 13) {
                 std::cerr << "FAIL: Attached sprite expected color 13, got " << (int)dut->sprdata << "\n";
                 return 1;
            }
        }
    }

    if (!saw_sprite) {
        std::cerr << "FAIL: Never saw attached sprite output!\n";
        return 1;
    }

    std::cout << "PASS: All tests passed.\n";
    delete dut;
    return 0;
}
