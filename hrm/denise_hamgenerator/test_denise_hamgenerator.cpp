#include <iostream>
#include <iomanip>
#include "Vdenise_hamgenerator.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vdenise_hamgenerator* dut = new Vdenise_hamgenerator;

    std::cout << "Starting FPGA-032: Denise HAM Generator Test...\n";

    dut->clk = 0;
    dut->clk7_en = 0;
    dut->reg_address_in = 0;
    dut->data_in = 0;
    dut->select = 0;
    dut->bplxor = 0;
    dut->bank = 0;
    dut->loct = 0;
    dut->ham8 = 0;

    auto tick = [&]() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    };
    
    // Reset pipeline
    for(int i=0; i<10; i++) tick();

    // Helper to write color RAM (simulating CPU write)
    auto write_color = [&](int index, uint16_t color12) {
        // COLORBASE = 0x180 (word address 0xC0)
        dut->reg_address_in = 0xC0 + index;
        dut->data_in = color12;
        dut->clk7_en = 1;
        tick();
        dut->clk7_en = 0;
        dut->reg_address_in = 0;
        for(int i=0; i<4; i++) tick(); // give it time to settle
    };

    std::cout << "Loading Color Palette...\n";
    // Color 0 = 0x000 (Black)
    // Color 1 = 0xFFF (White)
    write_color(0, 0x0000);
    write_color(1, 0x0FFF);
    
    // Simulate setting select and advancing pixel clock (4 28MHz ticks per pixel)
    auto set_pixel = [&](uint8_t sel) {
        dut->select = sel;
        tick();
        tick();
        tick();
        tick();
    };
    
    std::cout << "Testing OCS HAM Mode...\n";
    dut->ham8 = 0;
    
    // Start with Color 1 (Base color: White)
    // Note: color palette out is duplicated to 24 bits (e.g. 0xFFF -> 0xFFFFFF)
    set_pixel(0x01);
    if (dut->rgb != 0xFFFFFF) {
        std::cerr << "FAIL: Base Color 1 expected 0xFFFFFF, got 0x" << std::hex << dut->rgb << "\n";
        return 1;
    }
    
    // Modify Blue with 0x5 (0101)
    // command 01, data 0101 -> select = 0x15
    set_pixel(0x15);
    // Red, Green held from previous (FF). Blue modified to 55.
    if (dut->rgb != 0xFFFF55) {
        std::cerr << "FAIL: Modify Blue expected 0xFFFF55, got 0x" << std::hex << dut->rgb << "\n";
        return 1;
    }
    
    // Modify Red with 0xA (1010)
    // command 10, data 1010 -> select = 0x2A
    set_pixel(0x2A);
    // Green, Blue held from previous (FF, 55). Red modified to AA.
    if (dut->rgb != 0xAAFF55) {
        std::cerr << "FAIL: Modify Red expected 0xAAFF55, got 0x" << std::hex << dut->rgb << "\n";
        return 1;
    }
    
    // Modify Green with 0x0 (0000)
    // command 11, data 0000 -> select = 0x30
    set_pixel(0x30);
    // Red, Blue held (AA, 55). Green modified to 00.
    if (dut->rgb != 0xAA0055) {
        std::cerr << "FAIL: Modify Green expected 0xAA0055, got 0x" << std::hex << dut->rgb << "\n";
        return 1;
    }
    
    std::cout << "Testing AGA HAM8 Mode...\n";
    dut->ham8 = 1;
    
    // Start with base color 1 (White)
    // In HAM8, base color uses top 6 bits, control is bottom 2 bits.
    // Index 1 (000001), control 00 -> 0x04
    set_pixel(0x04);
    if (dut->rgb != 0xFFFFFF) {
        std::cerr << "FAIL: Base Color 1 expected 0xFFFFFF, got 0x" << std::hex << dut->rgb << "\n";
        return 1;
    }
    
    // In HAM8:
    // Modify Blue: control 01. Data 0x15 (010101).
    // select = (0x15 << 2) | 01 = 0x54 | 01 = 0x55.
    set_pixel(0x55);
    // The previous blue was 0xFF. The new blue will be {select_r[7:2], rgb_prev[1:0]}
    // {0x15, 0x3} = 0x57.
    if (dut->rgb != 0xFFFF57) {
        std::cerr << "FAIL: HAM8 Modify Blue expected 0xFFFF57, got 0x" << std::hex << dut->rgb << "\n";
        return 1;
    }
    
    std::cout << "PASS: All tests passed.\n";
    delete dut;
    return 0;
}
