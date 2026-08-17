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
    
    auto run_move_test = [&](uint16_t word1, bool cdang, bool expect_valid, const char* name) {
        std::cout << "DEBUG: " << name << " - Reset Copper\n";
        top->reset = 1;
        tick();
        top->reset = 0;
        
        // Write cdang
        // COPCON is 9'h02e in verilog, so COPCON[8:1] is 0x17.
        // top->reg_address_in in C++ maps to the 8-bit [8:1] port!
        top->reg_address_in = 0x017; 
        top->data_in = cdang ? 2 : 0; 
        top->eval();
        top->clk = 1; top->eval();
        if (tfp) tfp->dump(time_step++);
        top->clk = 0; top->eval();
        if (tfp) tfp->dump(time_step++);
        top->hpos++;
        if (top->hpos > 227) top->hpos = 0;
        top->reg_address_in = 0;
        top->data_in = 0;
        top->eval();
        
        // Wait for it to enter RESET and request dummy DMA
        int timeout = 50;
        while (top->reqdma == 0 && timeout > 0) {
            tick();
            timeout--;
        }
        std::cout << "DEBUG: " << name << " - Dummy wait ended. timeout=" << timeout << " reqdma=" << (int)top->reqdma << "\n";

        // Give it an ackdma to pass the RESET state
        top->ackdma = 1;
        tick(); // This tick processes the ack since reqdma=1 implies enable=1 at this posedge!
        top->ackdma = 0;

        // Wait for it to enter FETCH1 and request DMA
        timeout = 50;
        while (top->reqdma == 0 && timeout > 0) {
            tick();
            timeout--;
        }
        std::cout << "DEBUG: " << name << " - FETCH1 wait ended. timeout=" << timeout << " reqdma=" << (int)top->reqdma << "\n";

        // Copper should now be in FETCH1, dma_req = 1
        if (top->reqdma != 1) {
            std::cout << "FAIL: " << name << " - Expected reqdma=1 in FETCH1\n";
            fail_count++;
            return;
        }

        // We are currently AT a tick where `reqdma=1`.
        // So `enable=1` at the upcoming posedge!
        std::cout << "DEBUG: " << name << " - Before ack, reqdma=" << (int)top->reqdma 
                  << " reg_address_out=" << std::hex << (int)top->reg_address_out << std::dec 
                  << " state=" << (int)top->copper_state_dbg 
                  << " dma_req=" << (int)top->dma_req_dbg 
                  << " bus_ena=" << (int)top->bus_ena_dbg 
                  << " clk_ena=" << (int)top->clk_ena_dbg 
                  << "\n";
                  
        // Acknowledge DMA for FETCH1, provide Word 1
        top->ackdma = 1;
        top->data_in = word1;
        top->eval();
        tick();
        
        top->ackdma = 0;
        top->data_in = 0;
        top->eval();
        
        // Now Copper should be in FETCH2.
        // It will assert reqdma=1 if it's a valid MOVE. If it's an illegal MOVE, dma_req=0.
        timeout = 50;
        while (top->reqdma == 0 && timeout > 0) {
            tick();
            timeout--;
        }

        if (expect_valid) {
            if (top->reqdma != 1 || top->reg_address_out != (word1 >> 1)) {
                std::cout << "FAIL: " << name << " - Valid MOVE failed. reqdma=" << (int)top->reqdma 
                          << " reg_address_out=" << std::hex << (int)top->reg_address_out << "\n";
                fail_count++;
            } else {
                // Acknowledge DMA for FETCH2 (MOVE payload)
                top->ackdma = 1;
                top->data_in = 0x1234;
                top->eval();
                tick();
                top->ackdma = 0;
                top->data_in = 0;
                top->eval();
            }
        } else {
            if (top->reqdma != 0) {
                std::cout << "FAIL: " << name << " - Illegal MOVE failed. Expected halt (reqdma=0), got reqdma=" 
                          << (int)top->reqdma << "\n";
                fail_count++;
            }
        }
    };

    // Test 1: Valid MOVE to BPLCON0 (0x100 -> word offset 0x80)
    // Word 1: 0x0100 -> bit 0 = 0 (MOVE), word offset = 0x80
    run_move_test(0x0100, false, true, "Valid MOVE BPLCON0");

    // Test 2: Illegal MOVE to 0x040 (word offset 0x20), cdang = 0
    // Word 1: 0x0040 -> bit 0 = 0 (MOVE), word offset = 0x20
    run_move_test(0x0040, false, false, "Illegal MOVE (cdang=0)");

    // Test 3: Valid MOVE to 0x040 (word offset 0x20), cdang = 1
    // Word 1: 0x0040 -> bit 0 = 0 (MOVE), word offset = 0x20
    run_move_test(0x0040, true, true, "Valid MOVE (cdang=1)");

    if (fail_count == 0) {
        std::cout << "PASS: All Copper MOVE cases tested successfully.\n";
    }
    
    top->final();
    tfp->close();
    delete top;
    delete tfp;
    return fail_count == 0 ? 0 : 1;
}
