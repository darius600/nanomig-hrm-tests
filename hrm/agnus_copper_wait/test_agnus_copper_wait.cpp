// Test purpose: Validate Copper WAIT position, mask, and blitter-finished behavior.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 2, printed pp. 22-24 and 35.
// Non-goals: This does not prove complete cycle accuracy or all vertical-counter rollover cases.
//
// REVIEWED 07.09.2026 - written 18.08.2026, before the pipeline existed, and
// measured case by case against agnus_copper_waitskip_range:
//
//   "Wait V=50"            -> covered by W1 plus the full EV walk, which asserts
//                             every one of the eight VP bits separately
//   "Wait V=60 (BFD=0)"    -> covered by B2 and B3, which separate "held while
//                             busy" from "released once finished"; this file
//                             only checked that it eventually released
//   "Wait V=50 (H-Masked)" -> covered by W1's masked variant and the EM walk
//
// Fully superseded, and superseded more strictly in all three cases. Kept as an
// independent second implementation, not as coverage: it still reports one
// collective PASS rather than a result per proposition.
// See docs/test-inventory.md.

#include <verilated.h>
#include <verilated_vcd_c.h>
#include "Vagnus_copper.h"
#include <iostream>

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Verilated::traceEverOn(true);
    Vagnus_copper* top = new Vagnus_copper;
    VerilatedVcdC* tfp = new VerilatedVcdC;
    top->trace(tfp, 99);
    tfp->open("trace.vcd");
    
    int time_step = 0;

    int fail_count = 0;
    
    // Initialize
    top->clk = 0;
    top->clk7_en = 1;
    top->reset = 1;
    top->ecs = 0;
    top->data_in = 0;
    top->ackdma = 0;
    top->enadma = 0;
    top->sof = 0;
    top->blit_busy = 1; // Prevent skip_flag from being falsely asserted during MOVE
    top->vpos = 0;
    top->hpos = 5;
    top->reg_address_in = 0;
    
    auto tick = [&]() {
        if (top->ackdma) {
            top->reg_address_in = top->reg_address_out;
        } else {
            top->reg_address_in = 0;
        }
        top->eval(); // <--- Evaluate combinatorial logic before posedge!
        
        std::cout << "TICK before clk=1: time=" << time_step << " hpos=" << top->hpos 
                  << " ackdma=" << (int)top->ackdma 
                  << " reqdma=" << (int)top->reqdma 
                  << " reg_address_out=" << std::hex << (int)top->reg_address_out
                  << " reg_address_in=" << std::hex << (int)top->reg_address_in 
                  << " data_in=" << std::hex << top->data_in << std::dec << "\n";
                  
        top->clk = 1; top->eval();
        if (tfp) tfp->dump(time_step++);
        top->clk = 0; top->eval();
        if (tfp) tfp->dump(time_step++);
        
        top->hpos++;
        if (top->hpos > 227) top->hpos = 0;
        top->eval(); // IMPORTANT: evaluate combinatorial logic so reqdma is accurate
    };

    tick();
    
    // We now increment hpos in tick() so bus_ena and clk_ena operate normally.
    
    auto run_wait_test = [&](uint16_t word1, uint16_t word2, int wait_vpos, int wait_hpos, bool bfd, bool expect_wait, const char* name) {
        std::cout << "DEBUG: " << name << " - Reset Copper\n";
        top->reset = 1;
        tick();
        top->reset = 0;
        
        // Wait for it to enter RESET and request dummy DMA
        int timeout = 50;
        while (top->reqdma == 0 && timeout > 0) {
            tick();
            timeout--;
        }

        // Give it an ackdma to pass the RESET state
        top->ackdma = 1;
        tick(); 
        top->ackdma = 0;

        // Wait for it to enter FETCH1
        timeout = 50;
        while (top->reqdma == 0 && timeout > 0) {
            tick();
            timeout--;
        }

        // We are currently AT a tick where `reqdma=1`.
        
        // 1. Give word1 to COPINS via dma
        top->ackdma = 1;
        top->data_in = word1;
        tick();
        top->ackdma = 0;
        
        // Copper should enter FETCH2 and reqdma should be 1
        timeout = 10;
        while (top->reqdma == 0 && timeout > 0) {
            tick();
            timeout--;
        }
        if (timeout == 0) {
            std::cout << "FAIL: " << name << " - Failed to enter FETCH2\n";
            fail_count++;
            return false;
        }

        // 2. Give word2 to COPINS via dma
        top->ackdma = 1;
        top->data_in = word2;
        tick();
        top->ackdma = 0;
        
        // Copper should enter WAITSKIP1, then WAITSKIP2
        // We will wait until reqdma=1 again, which means it finished WAIT and went to FETCH1
        // We also want to control the beam position.
        
        // Let's set beam position explicitly
        top->vpos = wait_vpos - 10; // Start earlier
        top->hpos = 0;
        top->blit_busy = !bfd ? 1 : 0; // If we want to test BFD
        top->enadma = 1; // Copper needs an empty DMA slot to wake up!
        
        bool finished_wait = false;
        int wait_cycles = 0;
        
        while (wait_cycles < 5000) { // simulate some time
            tick();
            
            // Note: in tick(), hpos increments up to 227, then wraps and we could increment vpos,
            // but we're controlling vpos manually for the test to speed things up
            if (top->hpos == 0) {
                top->vpos++;
            }
            
            if (!bfd && top->vpos > wait_vpos) {
                top->blit_busy = 0;
            }
            
            if (top->reqdma == 1) {
                finished_wait = true;
                break;
            }
            wait_cycles++;
        }
        
        // If it was supposed to wait until the beam reached the target, let's see where it reached
        std::cout << "DEBUG: " << name << " - Finished wait=" << finished_wait << " at vpos=" << (int)top->vpos << " hpos=" << (int)top->hpos << "\n";
        
        bool passed = true;
        if (expect_wait) {
            if (!finished_wait) {
                std::cout << "FAIL: " << name << " - Copper timed out waiting!\n";
                passed = false;
            }
            // It should have reached the target position
            if (top->vpos < wait_vpos || (top->vpos == wait_vpos && top->hpos < wait_hpos)) {
                std::cout << "FAIL: " << name << " - Copper finished wait prematurely!\n";
                passed = false;
            }
        }
        
        if (!passed) {
            fail_count++;
        }
        return passed;
    };

    bool all_passed = true;

    // Test 1: Simple Wait (Wait for V=50, H=0)
    // Word 1: VP=50 (0x32 -> VP bits = 0x32), HP=0 -> 0x3201
    // Word 2: VM=0x7F, HM=0x7F, BFD=1, WAIT=0 -> 0xFFFE
    all_passed &= run_wait_test(0x3201, 0xFFFE, 50, 0, true, true, "Wait V=50");

    // Test 2: Wait with BFD=0 and blit_busy=1
    // Word 1: VP=60, HP=0 -> 0x3C01
    // Word 2: VM=0x7F, HM=0x7F, BFD=0, WAIT=0 -> 0x7FFE
    all_passed &= run_wait_test(0x3C01, 0x7FFE, 60, 0, false, true, "Wait V=60 (BFD=0)");

    // Test 3: Wait with horizontal mask
    // Word 1: VP=50 (0x32), HP=100 (0x64 -> bits 7:1 = 0x32) -> 0x3232 | 1 = 0x3233
    // Word 2: VM=0x7F, HM=0x00 (Mask out horizontal), BFD=1, WAIT=0 -> 0xFF80
    // Copper should wake up when VP=50 regardless of HP, so it wakes at HP=0.
    all_passed &= run_wait_test(0x3233, 0xFF80, 50, 0, true, true, "Wait V=50 (H-Masked)");

    if (fail_count == 0) {
        std::cout << "PASS: All Copper WAIT cases tested successfully.\n";
    } else {
        std::cout << "FAIL: " << fail_count << " Copper WAIT cases failed.\n";
    }

    if (tfp) {
        tfp->close();
    }
    top->final();
    return fail_count == 0 ? 0 : 1;
}
