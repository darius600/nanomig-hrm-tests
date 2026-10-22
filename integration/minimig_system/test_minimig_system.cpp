// Test purpose: Smoke-test CPU-bus access to selected Agnus and Paula registers through the full Minimig system.
// Test class: INTEGRATION_SMOKE
// Primary oracle: NanoMig 68000 bridge contract plus HRM register addresses and set/clear semantics.
// Non-goals: This does not prove complete bus timing, arbitration, or chipset conformance.

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

    std::cout << "Starting Top-Level System Bus Tests...\n";

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
    
    auto tick = [&]() {
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

    // The CPU bus cycle is slightly tricky.
    // It's a 68000 async bus.
    // Minimig top level expects standard 68000 cycles.
    // To write a register:
    // 1. Set address, r_w = 0, data
    // 2. Assert AS, UDS, LDS
    // 3. Wait for DTACK to be asserted (0)
    // 4. Deassert AS, UDS, LDS
    // 5. Wait for DTACK to be deasserted (1)
    
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

    // We will test if we can write to DMACON (Agnus) and read it back from DMACONR
    std::cout << "Test 1: Agnus DMACON Write & Read...\n";
    write_bus(0xDFF096, 0x8200); // Set DMAEN (bit 9)
    uint16_t dmaconr = read_bus(0xDFF002);
    if ((dmaconr & 0x0200) == 0) {
        std::cerr << "FAIL: DMACONR read did not match. Got 0x" << std::hex << dmaconr << "\n";
        pass = false;
    } else {
        std::cout << "PASS: Agnus DMACON accessed successfully. (DMACONR=0x" << std::hex << dmaconr << ")\n";
    }

    // Write to Paula INTENA (0xDFF09A) and read from INTENAR (0xDFF01C)
    std::cout << "Test 2: Paula INTENA Write & Read...\n";
    write_bus(0xDFF09A, 0xC000); // SET/CLR = 1, INTEN = 1
    uint16_t intenar = read_bus(0xDFF01C);
    if ((intenar & 0x4000) == 0) {
        std::cerr << "FAIL: INTENAR read did not match. Got 0x" << std::hex << intenar << "\n";
        pass = false;
    } else {
        std::cout << "PASS: Paula INTENA accessed successfully. (INTENAR=0x" << std::hex << intenar << ")\n";
    }

    if (pass) {
        std::cout << "All tests passed!\n";
    }

    tfp->close();
    delete dut;
    return pass ? 0 : 1;
}
