// Test purpose: Validate Paula interrupt set/clear, request latching, and selected 68000 priority levels.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 7, printed pp. 217-222.
// Non-goals: This does not exhaust all interrupt sources or simultaneous-priority combinations.

#include <iostream>
#include <iomanip>
#include "Vpaula_intcontroller.h"
#include "verilated.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vpaula_intcontroller* dut = new Vpaula_intcontroller;

    std::cout << "Starting FPGA-038/039/040: Paula IntController Tests...\n";

    dut->clk = 0;
    dut->clk7_en = 0;
    dut->reset = 1;
    dut->reg_address_in = 0;
    dut->data_in = 0;
    
    // Clear all hardware interrupts
    dut->rxint = 0;
    dut->txint = 0;
    dut->vblint = 0;
    dut->int2 = 0;
    dut->int3 = 0;
    dut->int6 = 0;
    dut->blckint = 0;
    dut->syncint = 0;
    dut->audint = 0;

    int clk7_cycle = 0;
    
    auto tick = [&]() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);
    };

    for(int i=0; i<16; i++) tick();
    dut->reset = 0;
    for(int i=0; i<16; i++) tick();

    auto write_reg = [&](int addr, uint16_t data) {
        while(!dut->clk7_en) tick();
        
        dut->reg_address_in = addr;
        dut->data_in = data;
        
        tick(); tick(); tick(); tick();
        
        dut->reg_address_in = 0;
        dut->data_in = 0;
    };
    
    auto read_reg = [&](int addr) -> uint16_t {
        while(!dut->clk7_en) tick();
        dut->reg_address_in = addr;
        tick(); 
        uint16_t val = dut->data_out;
        tick(); tick(); tick();
        dut->reg_address_in = 0;
        return val;
    };

    // FPGA-038: INTENA and INTREQ Register Write Logic
    std::cout << "Testing Register Write Logic (FPGA-038)...\n";
    // INTENA = 0x09A -> 0x4D
    // INTREQ = 0x09C -> 0x4E
    // INTENAR = 0x01C -> 0x0E
    // INTREQR = 0x01E -> 0x0F
    
    // Set bits 14 (master), 5 (vblank), 7 (audio 0) -> 0xC0A0
    write_reg(0x4D, 0xC0A0);
    uint16_t intenar = read_reg(0x0E);
    if (intenar != 0x40A0) { // Note: Amiga read clears bit 15
        std::cerr << "FAIL: INTENAR readback failed. Expected 0x40A0, got 0x" << std::hex << intenar << "\n";
        return 1;
    }
    
    // Clear bit 5 (vblank) by writing with MSB=0 -> 0x0020
    write_reg(0x4D, 0x0020);
    intenar = read_reg(0x0E);
    if (intenar != 0x4080) { 
        std::cerr << "FAIL: INTENAR clear bit 5 failed. Expected 0x4080, got 0x" << std::hex << intenar << "\n";
        return 1;
    }
    
    // Set INTREQ bit 4 (Copper) -> 0x8010
    write_reg(0x4E, 0x8010);
    uint16_t intreqr = read_reg(0x0F);
    if (intreqr != 0x0010) { 
        std::cerr << "FAIL: INTREQR readback failed. Expected 0x0010, got 0x" << std::hex << intreqr << "\n";
        return 1;
    }
    
    // FPGA-039: Hardware Interrupt Triggering
    std::cout << "Testing Hardware Interrupt Trigger Latching (FPGA-039)...\n";
    
    while(!dut->clk7_en) tick();
    dut->vblint = 1;
    tick(); tick(); tick(); tick();
    dut->vblint = 0;
    
    intreqr = read_reg(0x0F);
    if ((intreqr & 0x0020) == 0) { 
        std::cerr << "FAIL: VBLANK hardware interrupt did not latch into INTREQ.\n";
        return 1;
    }
    
    // FPGA-040: IPL Priority Encoder
    std::cout << "Testing IPL Priority Encoder (FPGA-040)...\n";
    // Currently INTENA has master (14) and audio0 (7) enabled.
    // INTREQ has Copper (4) and VBLANK (5) pending.
    // So no interrupt should be active. _ipl should be 7.
    while(!dut->clk7_en) tick();
    tick(); // allow pipeline update
    if (dut->_ipl != 7) {
        std::cerr << "FAIL: _ipl should be 7 (inactive). Got: " << (int)dut->_ipl << "\n";
        return 1;
    }
    
    // Enable VBLANK (5) in INTENA -> 0x8020
    write_reg(0x4D, 0x8020);
    while(!dut->clk7_en) tick();
    tick();
    // VBLANK is Level 3 -> _ipl should be ~3 = 4 (actually, Amiga vblank is level 3, so active low IPL is 4).
    if (dut->_ipl != 4) {
        std::cerr << "FAIL: _ipl should be 4 (Level 3 VBLANK). Got: " << (int)dut->_ipl << "\n";
        return 1;
    }
    
    // Enable Copper (4) in INTENA -> 0x8010
    write_reg(0x4D, 0x8010);
    while(!dut->clk7_en) tick();
    tick();
    // VBLANK (5) and Copper (4) both enabled and requested. VBLANK has higher priority.
    // Level 3 is still expected.
    if (dut->_ipl != 4) {
        std::cerr << "FAIL: _ipl should be 4 (Level 3 VBLANK). Got: " << (int)dut->_ipl << "\n";
        return 1;
    }
    
    // Trigger EXTERNAL (int6)
    dut->int6 = 1;
    tick(); tick(); tick(); tick();
    dut->int6 = 0;
    // Enable EXTERNAL (13) in INTENA -> 0xA000
    write_reg(0x4D, 0xA000);
    while(!dut->clk7_en) tick();
    tick();
    // EXTERNAL is Level 6 -> _ipl should be ~6 = 1.
    if (dut->_ipl != 1) {
        std::cerr << "FAIL: _ipl should be 1 (Level 6 EXTERNAL). Got: " << (int)dut->_ipl << "\n";
        return 1;
    }
    
    std::cout << "PASS: All tests passed.\n";
    delete dut;
    return 0;
}
