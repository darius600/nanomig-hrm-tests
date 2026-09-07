// Test purpose: Verify that a PAL non-interlaced field terminates with EOF and that the horizontal counter spans 0..453. The 312-line assertion was withdrawn on 07.09.2026 - see the note in main().
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, Chapter 2 p. 24 for the 140 ns horizontal position. The vertical line count is NOT asserted: the manual gives 312 in chapter 3 and sanctions 313 in Table 7-5.
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
    // A check asserting max_vpos==311 and exactly 312 EOL pulses stood here until
    // 07.09.2026, recorded as finding FPGA-050. It was withdrawn, not fixed: the
    // manual sanctions the value it called an error. Table 7-5, VPOSR bit 0 reads
    // "High bit of the vertical position (V8). Allows PAL line counts (313) to
    // appear in PAL versions of the Amiga", in both the printed and the
    // Developer-CD edition, and chapter 2 describes the alternation itself -
    // "in PAL, 312, then 313 lines". Chapter 3's "the beam produces 312 lines"
    // is about the visible display field. The check turned one sentence into a
    // rule the manual does not state, and it was the only thing making this
    // fixture red.
    //
    // Confirmed the same day against two emulators, which both make the count
    // depend on the long-frame bit rather than fixing it:
    //
    //   vAmiga, Core/Components/Agnus/Beam.h L84-L86 at commit 35de004:
    //     vMaxPal() { return lof ? 312 : 311; }  vCntPal() { return lof ? 313 : 312; }
    //     github.com/dirkwhoffmann/vAmiga/blob/35de004a2f1502c2811b9a8ebcde4d0979a98937/Core/Components/Agnus/Beam.h#L84-L86
    //
    //   WinUAE, custom.cpp L1422 at commit ccc407e:
    //     return maxvpos + (lof_store ? 1 : 0);   with MAXVPOS_PAL 312
    //     github.com/tonioni/WinUAE/blob/ccc407e9c636668b99b5f19d6e9aa03ea04b3052/custom.cpp#L1422
    //
    // agnus_beamcounter.v does the same: long_frame is set at reset and toggles
    // only in interlace. This fixture runs non-interlaced from reset, so 313 is
    // the correct answer for what it drives.
    //
    // The vertical count is left unasserted rather than re-asserted with the
    // other value: doing that properly means driving lof deliberately and
    // checking both cases, which is work nobody has done. See
    // docs/third-witness.md.
    std::cout << "vpos and EOL count are reported above, not asserted - see the note in the source\n";

    if (pass) {
        std::cout << "PASS: EOF arrives and the horizontal counter spans 0..453.\n";
    }

    top->final();
    delete top;
    return pass ? 0 : 1;
}
