// Test purpose: Verify that a normal PAL non-interlaced field wraps after the 312 positions stated by the HRM.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 2 p. 24 and Chapter 3 p. 40.
// Non-goals: This does not cover NTSC, interlace, ECS programmable timing, or sync pulse widths.

#include <iostream>
#include <verilated.h>
#include "Vagnus_beamcounter.h"

int main(int argc, char **argv) {
    Verilated::commandArgs(argc, argv);
    auto *top = new Vagnus_beamcounter;

    top->clk = 0;
    top->clk7_en = 0;
    top->reset = 1;
    top->cck = 0;
    top->ntsc = 0;
    top->aga = 0;
    top->ecs = 0;
    top->a1k = 0;
    top->reg_address_in = 0;
    top->data_in = 0;

    auto tick = [&]() {
        top->eval();
        top->clk = 1;
        top->eval();
        top->clk = 0;
        top->eval();
    };

    for (int i = 0; i < 10; ++i) {
        top->clk7_en = 1;
        tick();
    }
    top->reset = 0;

    int max_hpos = 0;
    int max_vpos = 0;
    int eol_count = 0;
    bool previous_eol = false;
    bool eof_seen = false;

    // Stop at the first public EOF. The cap permits one erroneous 313-line
    // field plus margin, while guaranteeing a missing EOF fails.
    for (int i = 0; i < 314 * 456 * 2; ++i) {
        top->clk7_en = (i % 2 == 0);
        top->cck = (i % 4 < 2);
        tick();

        if (top->hpos > max_hpos) max_hpos = top->hpos;
        if (top->vpos > max_vpos) max_vpos = top->vpos;

        const bool eol_now = top->eol;
        if (eol_now && !previous_eol) {
            ++eol_count;
        }
        previous_eol = eol_now;

        if (top->eof) {
            eof_seen = true;
            break;
        }
    }

    std::cout << "PAL non-interlaced: EOF=" << eof_seen
              << " max_hpos=" << max_hpos
              << " max_vpos=" << max_vpos
              << " EOL_count=" << eol_count << "\n";

    bool pass = true;
    if (!eof_seen) {
        std::cerr << "FAIL: No EOF was observed within the bounded PAL field.\n";
        pass = false;
    }
    if (max_hpos != 453) {
        std::cerr << "FAIL: Expected public 140 ns hpos range 0..453, got maximum "
                  << max_hpos << ".\n";
        pass = false;
    }
    if (max_vpos != 311 || eol_count != 312) {
        std::cerr << "FAIL: HRM normal PAL baseline is 312 positions (vpos 0..311); got max_vpos="
                  << max_vpos << " and " << eol_count << " EOL pulses before EOF.\n";
        pass = false;
    }

    if (pass) {
        std::cout << "PASS: Normal PAL field contains exactly 312 lines.\n";
    }

    top->final();
    delete top;
    return pass ? 0 : 1;
}
