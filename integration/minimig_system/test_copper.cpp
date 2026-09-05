#include <iostream>
#include <iomanip>
#include <vector>
#include "Vtest_minimig_system_top.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    Vtest_minimig_system_top* dut = new Vtest_minimig_system_top;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("trace.vcd");

    std::cout << "Starting FPGA-047: Copper <-> Register Bus Test...\n";

    dut->clk_28 = 0;
    dut->reset_n = 0;
    
    dut->cpu_address = 0;
    dut->cpu_data_in = 0;
    dut->_cpu_as = 1;
    dut->_cpu_uds = 1;
    dut->_cpu_lds = 1;
    dut->cpu_r_w = 1;
    
    dut->ram_data_in = 0;

    uint64_t main_time = 0;

    uint16_t chip_ram[1024*1024] = {0}; // 2MB Chip RAM

    auto tick = [&]() {
        // Handle RAM Read
        if (dut->_ram_oe == 0) {
            uint32_t addr = dut->ram_address;
            if (addr < 1024*1024) {
                dut->ram_data_in = chip_ram[addr];
            } else {
                dut->ram_data_in = 0xFFFF;
            }
        } else {
            dut->ram_data_in = 0xFFFF; // Float bus
        }
        
        // Handle RAM Write
        if (dut->_ram_we == 0) {
            uint32_t addr = dut->ram_address;
            if (addr < 1024*1024) {
                if (dut->_ram_ble == 0) chip_ram[addr] = (chip_ram[addr] & 0xFF00) | (dut->ram_data_out & 0x00FF);
                if (dut->_ram_bhe == 0) chip_ram[addr] = (chip_ram[addr] & 0x00FF) | (dut->ram_data_out & 0xFF00);
            }
        }
        
        dut->eval();
        tfp->dump(main_time++);
        dut->clk_28 = 1;
        dut->eval();
        tfp->dump(main_time++);
        dut->clk_28 = 0;
        dut->eval();
        tfp->dump(main_time++);
    };

    // Reset sequence
    for(int i=0; i<32; i++) tick();
    dut->reset_n = 1;
    // Wait for internal syscontrol reset to release (needs 4 VBLANKs, approx 2.5 million ticks)
    std::cout << "Waiting for system reset to clear (approx 3M ticks)...\n";
    Verilated::traceEverOn(false); // Temporarily disable tracing
    for(int i=0; i<3000000; i++) {
        dut->eval();
        dut->clk_28 = 1;
        dut->eval();
        dut->clk_28 = 0;
        dut->eval();
    }
    Verilated::traceEverOn(true); // Re-enable tracing

    auto write_bus = [&](uint32_t addr, uint16_t data) {
        dut->cpu_address = addr >> 1;
        dut->cpu_r_w = 0;
        dut->cpu_data_in = data;
        tick(); tick();
        dut->_cpu_as = 0;
        dut->_cpu_uds = 0;
        dut->_cpu_lds = 0;
        
        // Wait for DTACK
        int timeout = 0;
        while (dut->_cpu_dtack != 0 && timeout < 100) {
            tick();
            timeout++;
        }
        if (timeout >= 100) std::cerr << "WARNING: DTACK timeout on WRITE\n";
        
        for(int i=0; i<8; i++) tick(); // HOLD _AS low
        
        dut->_cpu_as = 1;
        dut->_cpu_uds = 1;
        dut->_cpu_lds = 1;
        dut->cpu_r_w = 1;
        
        for(int i=0; i<8; i++) tick(); // RECOVERY       
        while (dut->_cpu_dtack == 0 && timeout < 200) {
            tick();
            timeout++;
        }
        for(int i=0; i<4; i++) tick(); // bus idle
    };

    auto read_bus = [&](uint32_t addr) -> uint16_t {
        dut->cpu_address = addr >> 1;
        dut->cpu_r_w = 1;
        tick(); tick();
        dut->_cpu_as = 0;
        dut->_cpu_uds = 0;
        dut->_cpu_lds = 0;
        
        // Wait for DTACK
        int timeout = 0;
        while (dut->_cpu_dtack != 0 && timeout < 100) {
            tick();
            timeout++;
        }
        if (timeout >= 100) std::cerr << "WARNING: DTACK timeout on READ\n";
        
        for(int i=0; i<8; i++) tick(); // HOLD _AS low for a few ticks to simulate 68k S5-S7
        
        for(int i=0; i<8; i++) tick();
        uint16_t val = dut->cpu_data_out;
        
        dut->_cpu_as = 1;
        dut->_cpu_uds = 1;
        dut->_cpu_lds = 1;
        for(int i=0; i<8; i++) tick(); // RECOVERY
        
        while (dut->_cpu_dtack == 0 && timeout < 200) {
            tick();
            timeout++;
        }
        for(int i=0; i<4; i++) tick(); // bus idle
        return val;
    };
    
    bool pass = true;

    std::cout << "Setting up Copper list in RAM...\n";
    // Place Copper list at 0x10000 (which is word offset 0x8000)
    uint32_t cop_addr_base = 0x10000;
    uint32_t cop_word_idx = cop_addr_base >> 1;
    
    // Instruction 1: MOVE 0xC000 to INTENA (offset 0x09A)
    chip_ram[cop_word_idx + 0] = 0x009A; 
    chip_ram[cop_word_idx + 1] = 0xC000;
    
    // Instruction 2: WAIT end of list
    chip_ram[cop_word_idx + 2] = 0xFFFF;
    chip_ram[cop_word_idx + 3] = 0xFFFE;

    std::cout << "Initializing registers...\n";
    // We must enable Copper DMA in DMACON!
    // DMAEN = bit 9, COPEN = bit 7
    write_bus(0xDFF096, 0x8280); // Set DMAEN and COPEN
    
    // Point COP1LCH / COP1LCL to 0x00010000
    write_bus(0xDFF080, 0x0001); // COP1LCH
    write_bus(0xDFF082, 0x0000); // COP1LCL
    
    // Strobe COPJMP1 to start Copper!
    write_bus(0xDFF088, 0x0000); // COPJMP1
    
    // Give the Copper time to fetch and execute the list
    std::cout << "Waiting for Copper execution...\n";
    for(int i=0; i<1000; i++) tick();
    
    std::cout << "Reading INTENAR to verify Copper MOVE...\n";
    uint16_t intenar = read_bus(0xDFF01C);
    if ((intenar & 0x4000) == 0) {
        std::cerr << "FAIL: INTENAR read did not match. Got 0x" << std::hex << intenar << "\n";
        pass = false;
    } else {
        std::cout << "PASS: Copper successfully wrote to INTENA! (INTENAR=0x" << std::hex << intenar << ")\n";
    }

    if (pass) {
        std::cout << "All tests passed!\n";
    }

    tfp->close();
    delete dut;
    return pass ? 0 : 1;
}
