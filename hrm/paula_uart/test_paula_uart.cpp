// Test purpose: Validate selected Paula UART baud, frame, receive-data, interrupt, and SERDATR status behavior.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 8, printed pp. 255-259.
// Non-goals: This does not cover nine-bit mode, overrun, break, or every SERPER value.

#include <iostream>
#include <iomanip>
#include <vector>
#include "Vpaula_uart.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    Vpaula_uart* dut = new Vpaula_uart;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("trace.vcd");

    std::cout << "Starting FPGA-044: Paula UART Tests...\n";

    dut->clk = 0;
    dut->clk7_en = 0;
    dut->reset = 1;
    dut->rga_i = 0;
    dut->data_i = 0;
    dut->uartbrk = 0;
    dut->rbfmirror = 0;
    dut->rxd = 1; // Idle high

    int clk7_cycle = 0;
    uint64_t main_time = 0;
    
    auto tick = [&]() {
        dut->eval();
        tfp->dump(main_time++);
        dut->clk = 1;
        dut->eval();
        tfp->dump(main_time++);
        dut->clk = 0;
        dut->eval();
        tfp->dump(main_time++);
        
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);
    };

    for(int i=0; i<32; i++) tick();
    dut->reset = 0;
    for(int i=0; i<32; i++) tick();

    auto write_reg = [&](int addr, uint16_t data) {
        while(!dut->clk7_en) tick();
        
        dut->rga_i = addr >> 1; // pass bits [8:1]
        dut->data_i = data;
        
        tick(); tick(); tick(); tick();
        
        dut->rga_i = 0;
        dut->data_i = 0;
    };

    auto read_reg = [&](int addr) -> uint16_t {
        while(!dut->clk7_en) tick();
        
        dut->rga_i = addr >> 1;
        
        tick(); tick(); tick(); tick();
        uint16_t val = dut->data_o;
        dut->rga_i = 0;
        return val;
    };
    
    bool pass = true;

    // --- TEST 1: Transmit ---
    std::cout << "Test 1: Transmit...\n";
    // SERPER=10 selects 11 color clocks per bit. NanoMig exposes two
    // clk7-enable intervals per color clock, so the test samples a 22-enable bit cell.
    // Bit 15 remains clear for eight data bits.
    write_reg(0x32, 10); 
    
    // Write SERDAT: bit 8 is a stop bit usually set to 1 if we're sending 8 data bits (Wait, Amiga SERDAT data is bits [7:0] + stop bits [8:9]?)
    // HRM says: "Data bits 0-7, plus an optional 9th data bit (bit 8). Bit 8 must be set to 1 to generate the stop bit if sending 8 data bits."
    // Let's send 0x55. We set bit 8 to 1. So 0x155.
    write_reg(0x30, 0x0155);

    // Now monitor TXD
    std::vector<int> tx_bits;
    int ticks_since_start = 0;
    bool transmitting = false;
    int last_txd = 1;
    
    // Wait for start bit (txd goes 0)
    for(int i=0; i<3000; i++) {
        tick();
        if (dut->clk7_en) {
            if (!transmitting && last_txd == 1 && dut->txd == 0) {
                transmitting = true;
                ticks_since_start = 0;
            }
            if (transmitting) {
                if (ticks_since_start % 22 == 11) { // sample in the middle of the bit period
                    tx_bits.push_back(dut->txd);
                }
                ticks_since_start++;
                if (tx_bits.size() == 10) break; // Start + 8 data + Stop = 10 bits
            }
            last_txd = dut->txd;
        }
    }
    
    if (tx_bits.size() != 10) {
        std::cerr << "FAIL: Did not transmit 10 bits. Got " << tx_bits.size() << "\n";
        pass = false;
    } else {
        std::cout << "Transmitted bits: ";
        for(int b : tx_bits) std::cout << b;
        std::cout << "\n";
        
        // Expected: Start=0, LSB first for 0x55 = 1 0 1 0 1 0 1 0, Stop=1
        // So: 0 1 0 1 0 1 0 1 0 1
        std::vector<int> expected_tx = {0, 1, 0, 1, 0, 1, 0, 1, 0, 1};
        if (tx_bits != expected_tx) {
            std::cerr << "FAIL: Transmitted bits do not match expected 0x55.\n";
            pass = false;
        } else {
            std::cout << "PASS: Transmit OK.\n";
        }
    }
    
    // --- TEST 2: Receive ---
    std::cout << "Test 2: Receive...\n";
    // Set SERPER to 10 again, 8-bit mode.
    write_reg(0x32, 10);
    
    // We will simulate RXD signal sending 0xA3 (Start=0, LSB first=1 1 0 0 0 1 0 1, Stop=1)
    std::vector<int> rx_bits = {0, 1, 1, 0, 0, 0, 1, 0, 1, 1};
    int rx_index = 0;
    int rx_ticks = 0;
    
    bool rx_int_seen = false;
    
    for(int i=0; i<4000; i++) {
        if (dut->clk7_en) {
            if (rx_index < rx_bits.size()) {
                dut->rxd = rx_bits[rx_index];
                rx_ticks++;
                if (rx_ticks == 22) {
                    rx_ticks = 0;
                    rx_index++;
                }
            } else {
                dut->rxd = 1; // Idle
            }
            
            if (dut->rxint) {
                rx_int_seen = true;
            }
        }
        tick();
    }
    
    if (!rx_int_seen) {
        std::cerr << "FAIL: Did not see rxint.\n";
        pass = false;
    } else {
        // rbfmirror is Paula's external receive-buffer-full state. Keep it
        // asserted through the SERDATR read so bit 14 is actually observable.
        dut->rbfmirror = 1;
        tick(); tick(); tick(); tick();

        const uint16_t serdatr = read_reg(0x18);
        std::cout << "SERDATR = 0x" << std::hex << serdatr << std::dec << "\n";

        auto expect_set = [&](uint16_t mask, const char *name) {
            if ((serdatr & mask) == 0) {
                std::cerr << "FAIL: SERDATR " << name << " flag is clear.\n";
                pass = false;
            }
        };
        auto expect_clear = [&](uint16_t mask, const char *name) {
            if (serdatr & mask) {
                std::cerr << "FAIL: SERDATR " << name << " flag is set.\n";
                pass = false;
            }
        };

        expect_clear(0x8000, "OVRUN");
        expect_set(0x4000, "RBF");
        expect_set(0x2000, "TBE");
        expect_set(0x1000, "TSRE");
        expect_set(0x0800, "RXD");
        expect_clear(0x0400, "unused bit 10");
        expect_set(0x0100, "8-bit stop");

        if ((serdatr & 0x00FF) != 0x00A3) {
            std::cerr << "FAIL: Received data is 0x" << std::hex
                      << (serdatr & 0x00FF) << ", expected 0xA3.\n" << std::dec;
            pass = false;
        }

        dut->rbfmirror = 0;
        tick(); tick(); tick(); tick();

        if (pass) {
            std::cout << "PASS: Receive byte, interrupt, stop bit, and SERDATR flags match the selected HRM case. opus:7-int.c56, opus:7-int.c58, opus:7-fig7-4.c9, opus:7-fig7-4.c36 .\n";
        }
    }

    if (pass) {
        std::cout << "All tests passed!\n";
    }

    tfp->close();
    delete dut;
    return pass ? 0 : 1;
}
