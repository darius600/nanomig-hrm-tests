// Test purpose: Protect NanoMig audio-channel DMA startup and high-byte/low-byte sample sequencing.
// Test class: RTL_REGRESSION
// Primary oracle: Frozen FPGA-042 specification; HRM Third Edition pp. 164-166 constrain the sample-state sequence.
// Non-goals: Exact internal hpos/strhor coupling is not an HRM oracle.

#include <iostream>
#include <iomanip>
#include <vector>
#include "Vpaula_audio_channel.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    Vpaula_audio_channel* dut = new Vpaula_audio_channel;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("trace.vcd");

    std::cout << "Starting FPGA-042: Paula Audio Channel Tests...\n";

    dut->clk = 0;
    dut->clk7_en = 0;
    dut->cck = 0;
    dut->reset = 1;
    dut->aen = 0;
    dut->dmaena = 0;
    dut->reg_address_in = 0;
    dut->data = 0;
    dut->intpen = 0;
    dut->strhor = 0;

    int clk7_cycle = 0;
    int cck_cycle = 0;
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
        
        if (dut->clk7_en) {
            cck_cycle = (cck_cycle + 1) % 2;
            dut->cck = (cck_cycle == 0);
        } else {
            dut->cck = 0;
        }
    };

    for(int i=0; i<32; i++) tick();
    dut->reset = 0;
    for(int i=0; i<32; i++) tick();

    auto write_reg = [&](int addr, uint16_t data) {
        while(!dut->clk7_en) tick();
        
        dut->aen = 1;
        dut->reg_address_in = addr >> 1; // pass bits [3:1]
        dut->data = data;
        
        tick(); tick(); tick(); tick();
        
        dut->aen = 0;
        dut->reg_address_in = 0;
        dut->data = 0;
    };
    
    // Set up channel:
    // AUDPER = 4 (Period = 4 CCK ticks per sample)
    write_reg(0x6, 4);
    
    // AUDVOL = 64
    write_reg(0x8, 64);
    
    // AUDLEN = 2 (2 words to avoid Minimig silence quirk for len=1)
    write_reg(0x4, 2);
    
    // Enable DMA
    dut->dmaena = 1;
    
    // We will simulate Agnus answering the DMA request
    bool dma_answered = false;
    std::vector<uint8_t> output_samples;
    
    int cck_count = 0;
    int hpos = 0;
    uint8_t last_sample = 0;
    
    // Run for enough ticks to see a few samples
    for(int i=0; i<3000; i++) {
        tick();
        
        // Simulate strhor at specific hpos (like Minimig Agnus does)
        if (dut->cck) {
            hpos++;
            if (hpos == 13) {
                dut->strhor = 1;
            } else {
                dut->strhor = 0;
            }
            if (hpos >= 227) {
                hpos = 0;
            }
            
            // Debug: print sample and state if it changes
            static int last_dmareq = -1;
            if (dut->sample != last_sample || dut->dmareq != last_dmareq) {
                std::cout << "CCK " << cck_count << " (i=" << i << "): sample=0x" 
                          << std::hex << (int)dut->sample << std::dec 
                          << " dmareq=" << (int)dut->dmareq << "\n";
                if (dut->sample != last_sample) {
                    output_samples.push_back(dut->sample);
                    last_sample = dut->sample;
                }
                last_dmareq = dut->dmareq;
                cck_count = 0;
            }
            cck_count++;
        }
        
        // Handle DMA request
        // In Agnus, DMA is answered at specific slots (like hpos=13 for Audio 0)
        // But for testing, we just answer it once we see it, IF we haven't already answered it this cycle.
        // Wait, Agnus clears its "I answered this" flag when strhor happens?
        // Let's just answer it on hpos=13 if it's asserted!
        if (dut->clk7_en && hpos == 13 && dut->dmareq) {
            std::cout << "DMA REQ at i=" << i << " (hpos 13), answering with 0x1234\n";
            write_reg(0xA, 0x1234);
        }
    }
    
    bool pass = true;
    
    // We expect the sample sequence to be 0x12, then 0x34
    if (output_samples.size() < 2) {
        std::cerr << "FAIL: Did not get enough samples. Size=" << output_samples.size() << "\n";
        pass = false;
    } else {
        if (output_samples[0] != 0x12) {
            std::cerr << "FAIL: First sample is not 0x12, got 0x" << std::hex << (int)output_samples[0] << "\n";
            pass = false;
        }
        if (output_samples[1] != 0x34) {
            std::cerr << "FAIL: Second sample is not 0x34, got 0x" << std::hex << (int)output_samples[1] << "\n";
            pass = false;
        }
    }
    
    if (pass) {
        std::cout << "PASS: All tests passed.\n";
    }

    tfp->close();
    delete dut;
    return pass ? 0 : 1;
}
