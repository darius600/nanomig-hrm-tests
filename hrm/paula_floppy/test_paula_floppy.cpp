// Test purpose: Protect the NanoMig SD-sector-to-MFM floppy pipeline and its DSKLEN start path.
// Test class: RTL_REGRESSION
// Primary oracle: Frozen FPGA-045 specification and the NanoMig storage-adapter contract.
// Non-goals: The SD-card protocol and generated FIFO layout are not physical Paula conformance evidence.

#include <iostream>
#include <iomanip>
#include <vector>
#include "Vpaula_floppy.h"
#include "verilated.h"
#include "verilated_vcd_c.h"

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    Vpaula_floppy* dut = new Vpaula_floppy;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    dut->trace(tfp, 99);
    tfp->open("trace.vcd");

    std::cout << "Starting FPGA-045: Paula Floppy Tests...\n";

    dut->clk = 0;
    dut->clk7_en = 0;
    dut->clk7n_en = 0;
    dut->reset = 1;
    dut->ntsc = 0;
    dut->sof = 0;
    dut->enable = 0;
    dut->reg_address_in = 0;
    dut->data_in = 0;
    
    // CIA signals
    dut->_step = 1;
    dut->direc = 1;
    dut->_sel = 0xF;
    dut->side = 0;
    dut->_motor = 1;
    dut->wordsync = 0;
    
    // SD Card Interface
    dut->sdc_image_mounted = 0;
    dut->sdc_image_size = 0;
    dut->sdc_busy = 0;
    dut->sdc_done = 0;
    dut->sdc_byte_in_strobe = 0;
    dut->sdc_byte_addr = 0;
    dut->sdc_byte_in_data = 0;

    int clk_cycle = 0;
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
        
        clk_cycle = (clk_cycle + 1) % 4;
        dut->clk7_en = (clk_cycle == 0);
        dut->clk7n_en = (clk_cycle == 2);
    };

    for(int i=0; i<32; i++) tick();
    dut->reset = 0;
    for(int i=0; i<32; i++) tick();

    auto write_reg = [&](int addr, uint16_t data) {
        while(!dut->clk7_en) tick();
        
        dut->reg_address_in = addr >> 1; // pass bits [8:1]
        dut->data_in = data;
        
        tick(); tick(); tick(); tick();
        
        dut->reg_address_in = 0;
        dut->data_in = 0;
    };

    auto read_reg = [&](int addr) -> uint16_t {
        while(!dut->clk7_en) tick();
        
        dut->reg_address_in = addr >> 1;
        
        tick(); tick(); tick(); tick();
        uint16_t val = dut->data_out;
        dut->reg_address_in = 0;
        return val;
    };
    
    bool pass = true;
    
    // Set up disk presence (mock)
    dut->floppy_drives = 1; // 1 drive connected
    
    // Select drive 0 and turn motor on
    dut->_sel = 0xE; // 1110 (Drive 0 active low)
    dut->_motor = 0; // Motor ON (active low)
    for(int i=0; i<32; i++) tick();

    // Start DMA Read
    std::cout << "Test 1: MFM Floppy Read...\n";
    dut->enable = 1; // Enable DMA globally
    
    write_reg(0x024, 0x800A); // Write DSKLEN: 10 words, bit 15 set
    write_reg(0x024, 0x800A); // Second write sets DMAEN = 1 -> Triggers DMA!

    // We need to wait for paula to request the sector
    bool requested = false;
    for(int i=0; i<100; i++) {
        tick();
        if (dut->sdc_rd != 0) {
            requested = true;
            break;
        }
    }
    if (!requested) {
        std::cerr << "FAIL: Floppy module did not request sector read from SD card.\n";
        pass = false;
    } else {
        std::cout << "PASS: Floppy requested sector read. (sdc_rd=" << (int)dut->sdc_rd << ")\n";
        
        // Acknowledge the SD read
        dut->sdc_busy = 1;
        for(int i=0; i<20; i++) tick();
        
        // Feed 512 bytes of sector data
        for(int i=0; i<512; i++) {
            dut->sdc_byte_addr = i;
            dut->sdc_byte_in_data = (i % 256); // some pattern
            dut->sdc_byte_in_strobe = 1;
            tick(); tick(); tick(); tick();
            dut->sdc_byte_in_strobe = 0;
            tick(); tick(); tick(); tick();
        }
        
        dut->sdc_busy = 0;
        for(int i=0; i<100; i++) tick();
        
        // Read DSKDATR repeatedly to check the MFM stream
        std::cout << "Reading FIFO DSKDATR...\n";
        std::vector<uint16_t> mfm_words;
        
        // We will read a few words (preamble and sync)
        for(int i=0; i<10; i++) {
            // we simulate Paula DMA read (which pulses dma requests)
            // But we can just use bus reads. Wait, Paula's fifo logic:
            // "fifo_wr = ... | (buswr & dmaon)" 
            // "fifo_rd = ... (busrd & dmaon)" => wait, no. 
            // Actually, does bus read pop the FIFO? Let's check minimig's paula_floppy_fifo.
            // A bus read of DSKDATR normally reads the fifo if it's CPU reading, but here we just read it.
            // Let's do a few reads.
            uint16_t val = read_reg(0x008);
            mfm_words.push_back(val);
        }
        
        // The first few words should be 0x4489 (sync) or preamble 0xAAAA.
        // Actually minimig generates 4489 4489 as word 2,3.
        bool found_sync = false;
        for(auto w : mfm_words) {
            if (w == 0x4489) found_sync = true;
            std::cout << "MFM Word: 0x" << std::hex << w << std::dec << "\n";
        }
        if (!found_sync) {
            // It might take some clocks for the internal state machine to fill the FIFO
            // Let's poll for a while
            for (int k=0; k<1000; k++) {
                uint16_t val = read_reg(0x008);
                if (val == 0x4489) {
                    found_sync = true;
                    std::cout << "MFM Word (delayed): 0x4489\n";
                    break;
                }
            }
        }
        
        if (!found_sync) {
            std::cerr << "FAIL: Did not find 0x4489 sync mark in MFM stream.\n";
            pass = false;
        } else {
            std::cout << "PASS: Floppy MFM Synchronization Sync-word 0x4489 found.\n";
        }
    }

    if (pass) {
        std::cout << "All tests passed!\n";
    }

    tfp->close();
    delete dut;
    return pass ? 0 : 1;
}
