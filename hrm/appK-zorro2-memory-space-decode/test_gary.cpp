// appK-zorro2-memory-space-decode -- p.386 Figure K-1 "Expansion Memory
// Map", `gary` alone. 3 claims, filtered from
// hrm-testpipeline/work/appK-build/appK-zorro2-memory-space-decode.jsonl.
//
// sol:K-0044 and opus:K-0051 are two independent extractions of the SAME
// band: Amiga chip memory, $000000 up to (not including) $200000. Graded
// against gary.v:159-162 (`sel_chip[0..3]`, cpu_address_in[23:19]==
// 00000/00001/00010/00011).
//
// opus:K-0056 is a second, independent band: motherboard ROM, $F00000 up
// to (not including) $1000000. Graded against gary.v:166 (`sel_kick`,
// cpu_address_in[23:19]==11111 -- which is $F80000-$FFFFFF, NOT
// $F00000-$FFFFFF: the printed lower boundary and the RTL's own lower
// boundary disagree by half the band. This is measured below, not
// asserted from reading the RTL comment alone.)
//
// BOUNDARY. The Zorro II expansion-RAM band the same figure prints
// ($200000-$9FFFFF) is decoded by cpu_wrapper.v's `sel_z2ram`, a different
// module -- out of scope for this one-top-module (`gary`) fixture. None of
// this package's 3 claims assert anything about that band, so no claim
// here is declined with record() for that reason; the note is carried in
// SPECIFICATION.md for orientation.
//
// `cpu_address_in` is gary's own `[23:1]` port: a byte address with bit 0
// dropped. Every address below is driven as `byte_address >> 1`.

#include "Vgary.h"
#include "verilated.h"
#include <iostream>
#include <string>
#include <cstdint>

static Vgary* top;
static vluint64_t sim_time = 0;
static int passed = 0, failed = 0;

// ---- the fixed harness (character-identical across the suite) ----
void expect(bool cond, const std::string& label) {
    if (cond) { std::cout << "PASS: " << label << "\n"; passed++; }
    else      { std::cout << "FAIL: " << label << "\n"; failed++; }
}
void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

// ---- clocking ----
void tick() {
    top->clk = 0; top->eval(); sim_time++;
    top->clk = 1; top->eval(); sim_time++;
}
void settle(int n) { for (int i = 0; i < n; i++) tick(); }

// Drive a plain CPU read cycle at `byte_addr` (dbr=0, cpu_rd=1, cpu_hlt=0)
// and settle. `ovl` is left as the caller last set it (default 0, flipped
// only by the MUT_ovl guard below).
void set_addr(uint32_t byte_addr) {
    top->cpu_address_in = byte_addr >> 1;
    settle(3);
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    top = new Vgary;

    // ---- initialize every port to a known value ----
    top->clk = 0;
    top->reset = 1;
    top->cpu_address_in = 0;
    top->dma_address_in = 0;
    top->cpu_data_out = 0;
    top->custom_data_out = 0;
    top->ram_data_out = 0;
    top->a1k = 0;
    top->bootrom = 0;
    top->cpu_rd = 1;      // plain CPU read cycle throughout
    top->cpu_hwr = 0;
    top->cpu_lwr = 0;
    top->cpu_hlt = 0;
    top->ovl = 0;          // Kickstart overlay off (normal post-boot state)
    top->dbr = 0;           // CPU cycle, not an Agnus DMA cycle
    top->dbwe = 0;
    top->memory_config = 0;
    top->ecs = 0;
    top->hdc_ena = 0;
    top->eval();

    settle(5);
    top->reset = 0;
    settle(5);

    // =====================================================================
    // Fixture liveness -- proven before any claim is graded.
    // =====================================================================
    set_addr(0x000000);
    uint32_t sel_chip_at_zero = top->sel_chip;
    set_addr(0x800000); // well outside any band gary decodes as chip/kick
    uint32_t sel_chip_at_mid = top->sel_chip;
    note("fixture-liveness sel_chip at $000000=0x" + std::to_string(sel_chip_at_zero) +
         " at $800000=0x" + std::to_string(sel_chip_at_mid));
    expect(sel_chip_at_zero != sel_chip_at_mid,
        "[not graded -- apparatus check, no claim uid] fixture-liveness: "
        "sel_chip responds to an address change");

    // =====================================================================
    // sol:K-0044 / opus:K-0051 -- Amiga chip memory, $000000 up to (not
    // including) $200000. gary.v:159-162.
    // =====================================================================
    set_addr(0x000000);
    bool chip_at_base = (top->sel_chip != 0);
    note("chip band base $000000: sel_chip=0x" + std::to_string((unsigned)top->sel_chip));

    set_addr(0x1FFFFE); // last word before the printed $200000 boundary
    bool chip_near_top = (top->sel_chip != 0);
    note("chip band near top $1FFFFE: sel_chip=0x" + std::to_string((unsigned)top->sel_chip));

#ifdef MUT_ovl
    top->ovl = 1; // MUTATION: kickstart overlay held on at the base address
    set_addr(0x000000);
    chip_at_base = (top->sel_chip != 0);
    top->ovl = 0;
#endif

    set_addr(0x200000); // the printed boundary itself
    bool chip_gone_at_boundary = (top->sel_chip == 0);
    note("chip band boundary $200000: sel_chip=0x" + std::to_string((unsigned)top->sel_chip) +
         " (expected 0 -- band claimed exclusive of this address)");

    bool chip_range_matches = chip_at_base && chip_near_top && chip_gone_at_boundary;

    // MUTATION sol:K-0044 stimulus red: hold ovl=1 during the $000000
    // check -- gary.v:159's `(!ovl || cpu_hlt)` term goes false with
    // cpu_hlt=0, sel_chip[0] drops to 0 at the base address even though
    // the address itself is unchanged, and chip_range_matches goes false.
    // Build with `make MUT=ovl` to replay it.
    expect(chip_range_matches, "sol:K-0044");
    // MUTATION opus:K-0051 stimulus red: same as sol:K-0044 -- ovl=1 at
    // the base address (`make MUT=ovl`).
    expect(chip_range_matches, "opus:K-0051");

    // =====================================================================
    // opus:K-0056 -- Motherboard ROM, $F00000 up to (not including)
    // $1000000. gary.v:166.
    // =====================================================================
#ifdef MUT_romrange
    // MUTATION: test an address safely inside gary's OWN $F80000-$FFFFFF
    // window instead of the page's own stated lower boundary $F00000 --
    // proves the red below is a genuine partial-range mismatch, not a
    // fixture that reads sel_kick wrong or a signal that is simply always
    // 0. Build with `make MUT=romrange` to replay it.
    set_addr(0xFC0000);
#else
    set_addr(0xF00000); // the claim's own stated lower boundary
#endif
    bool rom_at_claimed_base = (top->sel_kick != 0);
    note("ROM band claimed base " +
         std::string(
#ifdef MUT_romrange
             "$FC0000 (MUTATION: inside gary's actual window, not the page's own $F00000)"
#else
             "$F00000"
#endif
         ) + ": sel_kick=" + std::to_string((int)top->sel_kick));

    // Also measure gary's own actual lower boundary for context (not
    // graded): sel_kick's RTL condition is cpu_address_in[23:19]==11111,
    // i.e. $F80000-$FFFFFF by the comment at gary.v:166.
    set_addr(0xF7FFFE); // just below gary's own decode window
    note("gary's own window, just below ($F7FFFE): sel_kick=" +
         std::to_string((int)top->sel_kick));
    set_addr(0xF80000); // gary's own decode window, first address
    note("gary's own window, first address ($F80000): sel_kick=" +
         std::to_string((int)top->sel_kick));

    // MUTATION opus:K-0056 stimulus green: `make MUT=romrange` moves the
    // tested address from the page's own $F00000 to $FC0000, safely
    // inside gary's actual $F80000-$FFFFFF window -- sel_kick reads 1 and
    // this assertion flips from red to green.
    expect(rom_at_claimed_base, "opus:K-0056");

    std::cout << passed << " passed, " << failed << " failed\n";

    top->final();
    delete top;
    return failed == 0 ? 0 : 1;
}
