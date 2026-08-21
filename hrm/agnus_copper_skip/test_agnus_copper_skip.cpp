// Test purpose: Validate that Copper SKIP suppresses exactly the next MOVE when its HRM condition matches.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 2, printed pp. 31-32 and 36.
// Non-goals: This does not prove complete Copper bus-cycle timing.

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

    bool did_move = false;

    auto tick = [&]() {
        if (top->ackdma) {
            top->reg_address_in = top->reg_address_out;
        } else {
            top->reg_address_in = 0;
        }
        top->eval(); // <--- Evaluate combinatorial logic before posedge!
        
        if (top->reg_address_out == 0x80 && top->reqdma == 1) {
            did_move = true;
        }
        
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
    
    auto provide_word = [&](uint16_t word) {
        int timeout = 50;
        while (top->reqdma == 0 && timeout > 0) {
            tick();
            timeout--;
        }
        if (timeout == 0) {
            std::cout << "FAIL: Timeout waiting for reqdma=1\n";
            return false;
        }
        top->ackdma = 1;
        top->data_in = word;
        tick();
        top->ackdma = 0;
        return true;
    };

    auto run_skip_test = [&](uint16_t word1, uint16_t word2, int wait_vpos, int wait_hpos, bool bfd, bool expect_skip, const char* name) {
        std::cout << "DEBUG: " << name << " - Reset Copper\n";
        did_move = false;
        top->reset = 1;
        top->ackdma = 0;
        top->enadma = 0;
        top->data_in = 0;
        for (int i=0; i<10; i++) tick();
        top->reset = 0;
        for (int i=0; i<10; i++) tick();
        
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

        // 1. Give SKIP word1 to COPINS via dma
        if (!provide_word(word1)) return false;
        
        // Set beam position before giving Word 2!
        top->vpos = wait_vpos;
        top->hpos = wait_hpos;
        top->blit_busy = !bfd ? 1 : 0; 
        
        // 2. Give SKIP word2 to COPINS via dma
        if (!provide_word(word2)) return false;
        
        // Copper goes to WAITSKIP1
        // We set enadma=1 so it can transition to WAITSKIP2
        top->enadma = 1;
        
        // Now Copper is in WAITSKIP2.
        // It does NOT request a dummy DMA cycle! It just transitions to FETCH1 
        // to fetch the next instruction.
        // We give it a valid MOVE instruction (MOVE to BPLCON0)
        if (!provide_word(0x0100)) return false;
        
        // Copper is in FETCH2 for the MOVE instruction
        // Give it the MOVE word2 (data). If skipped, this is treated as the next instruction.
        if (!provide_word(0x1234)) return false;
        
        // Just tick a few times to let it settle
        for (int i=0; i<10; i++) tick();
        
        bool passed = true;
        if (expect_skip) {
            if (did_move) {
                std::cout << "FAIL: " << name << " - Expected SKIP, but MOVE was executed!\n";
                passed = false;
            } else {
                std::cout << "PASS: " << name << " - Skipped successfully.\n";
            }
        } else {
            if (!did_move) {
                std::cout << "FAIL: " << name << " - Expected MOVE, but it was skipped!\n";
                passed = false;
            } else {
                std::cout << "PASS: " << name << " - Did not skip successfully.\n";
            }
        }
        
        if (!passed) fail_count++;
        return passed;
    };

    bool all_passed = true;

    // Test 1: Simple Skip (Beam is at V=60, H=0. Target V=50, H=0)
    // Target is V=50, H=0. Current is V=60, H=0. Current >= Target -> Match -> SKIP!
    // Word 1: VP=50 (0x3201)
    // Word 2: VM=0x7F, HM=0x7F, BFD=1, SKIP=1 -> 0xFFFF
    all_passed &= run_skip_test(0x3201, 0xFFFF, 60, 0, true, true, "Skip (Beam past target)");

    // Test 2: No Skip (Beam is at V=40, H=0. Target V=50, H=0)
    // Current < Target -> No Match -> NO SKIP!
    all_passed &= run_skip_test(0x3201, 0xFFFF, 40, 0, true, false, "No Skip (Beam before target)");

    // Test 3: Skip with BFD=0 and blitter busy
    // Beam is past target (V=60), but BFD=0 and blit_busy=1 -> NO SKIP!
    // Word 2: BFD=0 -> 0x7FFF
    all_passed &= run_skip_test(0x3201, 0x7FFF, 60, 0, false, false, "No Skip (Blitter busy)");

    if (fail_count == 0) {
        std::cout << "PASS: All Copper SKIP cases tested successfully.\n";
    } else {
        std::cout << "FAIL: " << fail_count << " Copper SKIP cases failed.\n";
    }

    if (tfp) {
        tfp->close();
    }
    top->final();
    return fail_count == 0 ? 0 : 1;
}
