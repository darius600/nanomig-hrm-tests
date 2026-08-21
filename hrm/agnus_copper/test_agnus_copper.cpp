// Test purpose: Execute a short Copper list and prove that a matching HRM SKIP suppresses only the following MOVE.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 2, printed pp. 22-24, 31-32, and 36.
// Non-goals: This does not prove every mask, blitter, rollover, or bus-contention case.

#include <cstdint>
#include <iostream>
#include <vector>
#include <verilated.h>
#include "Vagnus_copper.h"

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *top = new Vagnus_copper;

    top->clk = 0;
    top->clk7_en = 1;
    top->reset = 1;
    top->ecs = 0;
    top->enadma = 1;
    top->ackdma = 0;
    top->sof = 0;
    top->blit_busy = 0;
    top->hpos = 0;
    top->vpos = 0;
    top->data_in = 0;
    top->reg_address_in = 0;

    // WAIT until V=$20, MOVE COLOR00, matching SKIP, skipped MOVE COLOR01,
    // then an end-of-list WAIT.
    const std::vector<uint16_t> memory = {
        0x2009, 0xFFFE,
        0x0180, 0x0F00,
        0x2001, 0xFF01,
        0x0182, 0x00F0,
        0xFFFF, 0xFFFE
    };

    auto clock_edge = [&]() {
        top->clk = 1;
        top->eval();
        top->clk = 0;
        top->eval();
    };

    for (int i = 0; i < 8; ++i) {
        clock_edge();
    }
    top->reset = 0;

    int color00_writes = 0;
    int color01_writes = 0;
    int dma_cycles = 0;

    // Public-interface bus model: reqdma/ackdma transfers the word selected by
    // address_out. A MOVE is observable as its custom-register address on
    // reg_address_out during that accepted request.
    for (int cycle = 0; cycle < 0x26 * 454; ++cycle) {
        top->eval();

        if (top->reqdma) {
            ++dma_cycles;
            const uint32_t address = top->address_out;
            top->data_in = address < memory.size() ? memory[address] : 0xFFFF;
            top->ackdma = 1;
            top->reg_address_in = top->reg_address_out;
            top->eval();

            if (top->reg_address_out == (0x180 >> 1)) {
                ++color00_writes;
            }
            if (top->reg_address_out == (0x182 >> 1)) {
                ++color01_writes;
            }
        } else {
            top->ackdma = 0;
            top->data_in = 0;
            top->reg_address_in = 0;
        }

        clock_edge();

        if (++top->hpos == 454) {
            top->hpos = 0;
            ++top->vpos;
        }
    }

    const bool pass = color00_writes == 1 && color01_writes == 0;
    std::cout << "Observed DMA cycles=" << dma_cycles
              << " COLOR00 writes=" << color00_writes
              << " COLOR01 writes=" << color01_writes << "\n";

    if (!pass) {
        std::cerr << "FAIL: Expected COLOR00 once and the following COLOR01 MOVE to be skipped.\n";
    } else {
        std::cout << "PASS: WAIT released, COLOR00 executed, and matching SKIP suppressed COLOR01.\n";
    }

    top->final();
    delete top;
    return pass ? 0 : 1;
}
