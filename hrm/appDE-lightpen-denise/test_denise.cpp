// appDE-lightpen-denise
//
// Grades 2 claims (1 proposition, claimed independently by readers sol and
// opus) against `denise` and every module it itself instantiates (compiled
// together -- Makefile modelled on hrm/ch8-appb-chip-column-denise, a
// package with the same fixture boundary and the same zero-expect()-claims
// shape), from Appendix E p.333's LIGHT PEN figure.
//
// uid -> proposition:
//   sol:E-lightpen.c17, opus:E-lightpen.c20  "BPLCON0 write only  DFF104"
//
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 333 (Appendix E, LIGHT PEN figure).
//
// THE DEFECT. p.333's figure prints 'BPLCON0 write only  DFF104', the SAME
//   address it prints two boxes below for 'POTINP read only'. One address,
//   two registers, on the same page. BOTH labels are wrong, and the rest of
//   the manual settles it: appA-playfield/p271.txt:8 and appB3/p291.txt:27
//   both print 'BPLCON0  100  W'; appA-playfield/p271.txt:13 prints
//   'BPLCON2  104  W'. $DFF104 is BPLCON2's address; BPLCON0 is $DFF100.
//   denise.v:59-60 agrees (`parameter BPLCON0 = 9'h100;`,
//   `parameter BPLCON2 = 9'h104;`, confirmed by direct read). So p.333
//   attaches BPLCON2's real address to BPLCON0, not to BPLCON2 -- neither
//   register printed on that line owns $DFF104 under this reading. Per the
//   build brief ("Where the manual contradicts itself, grade neither side")
//   and the NM-HRM-062 precedent: measure both readings, record both, grade
//   neither. Measured directly below (FIXTURE-LIVENESS / DFF104-PROBE).
//
// THE DIRECTION HALF. "Whether the access direction is a separable
//   proposition you can grade on its own axis" is this package's call to
//   make: it is separable in principle (an address claim and a direction
//   claim are different propositions), but CHECKED AND FOUND ALREADY
//   GRADED -- hrm/ch3-register-map-denise's BPLCON0-WO assertion (verified
//   read directly in that package's test_denise.cpp) grades exactly "a
//   distinct pattern written to BPLCON0's own address reads back $0000
//   there", at $100, citing 'BPLCON0 ... W' (p.271/291) and chapter 8's
//   'BPLCON0 ($DFF100) Bit 3 enables the light pen latch' (p.229) -- this
//   package's own LIGHT PEN topic, one page earlier. So even graded
//   separately from the disputed address, the direction half is a
//   re-grade, not new content. Both claims decline in full.
//
// FIXTURE LIVENESS / DFF104-PROBE. No claim id, no manual page: two BPLCON0
//   bits reach `denise`'s own top-level output ports directly -- bit 15 to
//   `hires` (denise.v:180) and bit 6 to `shres` (denise.v:187) -- so
//   "the write arrived" is observed on a primary output, not inferred from
//   a read multiplexer that could itself be the thing under test (denise's
//   own data_out has no case for BPLCON0 or BPLCON2 at all; both are
//   write-only with no readback path). Reused, trimmed, from
//   hrm/ch8-appb-chip-column-denise/test_denise.cpp's own A0 check (same
//   module, same technique, same reason for choosing it), then extended
//   here with a second write at $DFF104 -- the actual DFF104-PROBE
//   measurement this package's decline cites.
//
// MUTATION FIXTURE-LIVENESS stimulus red: the first write's data word was
//   changed from 0x8000 (hires bit set) to 0x0000, so `hires` never went
//   high and the first half of the liveness comparison found no change.
//   Built with `make MUT=FIXTURE_LIVENESS`, confirmed "0 passed, 1 failed",
//   reverted (obj_dir cleaned, rebuilt with no MUT flag) and confirmed
//   "1 passed, 0 failed" restored.

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include "Vdenise.h"
#include "verilated.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

std::string hexbit(bool b) { return b ? "1" : "0"; }

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vdenise* dut = new Vdenise;
    Checks checks;

    int clk7_cycle = 0;
    auto tick = [&]() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);
    };

    auto do_reset = [&]() {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->strhor = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->blank = 0;
        dut->a1k = 0;
        dut->ecs = 1;
        dut->aga = 1;
        for (int i = 0; i < 16; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; i++) tick();
    };

    auto write_reg = [&](int addr, uint16_t data) {
        while (!dut->clk7_en) tick();
        dut->reg_address_in = addr;
        dut->data_in = data;
        tick(); tick(); tick(); tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
    };

    // Register addresses: documented Amiga custom-chip byte offsets,
    // presented as word indices (byte address >> 1) on reg_address_in[8:1]
    // -- confirmed at denise.v:39 and denise.v:56-65's own address table.
    const int BPLCON0_ADDR = 0x80;  // write, byte $100 -- denise.v:59
    const int DFF104_ADDR  = 0x82;  // byte $104 -- p.333's printed (wrong)
                                     // BPLCON0 address; denise.v:60 names
                                     // this BPLCON2's real address instead.

    // ========================================================================
    // FIXTURE-LIVENESS / DFF104-PROBE (no claim id, no manual page).
    // ========================================================================
    do_reset();
    uint16_t first_word = 0x8000;  // hires bit (bit 15) set
#ifdef MUT_FIXTURE_LIVENESS
    first_word = 0x0000;
#endif
    write_reg(BPLCON0_ADDR, first_word);
    bool hires_after_100 = dut->hires;
    write_reg(BPLCON0_ADDR, 0x0040);  // shres bit (bit 6) set, hires cleared
    bool shres_after_100 = dut->shres;
    bool hires_cleared_after_100 = dut->hires;
    checks.expect(
        hires_after_100 == 1 && shres_after_100 == 1 && hires_cleared_after_100 == 0,
        "FIXTURE-LIVENESS: writing 0x8000 to BPLCON0's own address ($DFF100) drives "
        "`hires` (denise.v:180, BPLCON0 bit 15) to " + hexbit(hires_after_100) +
        ", and a second write of 0x0040 there drives `shres` (denise.v:187, bit 6) to " +
        hexbit(shres_after_100) + " while `hires` returns to " +
        hexbit(hires_cleared_after_100) + " -- the write-to-primary-output data path "
        "for BPLCON0 is alive at this boundary before any claim below is declined on "
        "the strength of what this fixture COULD measure.");

    do_reset();
    write_reg(BPLCON0_ADDR, 0x8000);
    bool hires_at_100 = dut->hires;
    checks.note(
        "Writing 0x8000 (BPLCON0's own hires bit, bit 15) to BPLCON0's printed-correct "
        "address $DFF100 drives `hires` high (" + hexbit(hires_at_100) + ") -- "
        "'BPLCON0  100  W' (appA-playfield/p271.txt:8, appB3/p291.txt:27), denise.v:59 "
        "`parameter BPLCON0 = 9'h100`.");

    do_reset();
    write_reg(DFF104_ADDR, 0x8000);
    bool hires_at_104 = dut->hires;
    checks.note(
        "Writing the SAME word (0x8000) to $DFF104 -- p.333's printed (wrong) address "
        "for 'BPLCON0 write only' -- leaves `hires` at " + hexbit(hires_at_104) +
        ": $DFF104 does not reach BPLCON0's own bit-15 wiring. denise.v:60 names $104 "
        "as BPLCON2's own address (`parameter BPLCON2 = 9'h104`), a different register "
        "entirely, confirmed against appA-playfield/p271.txt:13 ('BPLCON2  104  W') -- "
        "neither register p.333 labels with $DFF104 is the one that lives there.");

    // ================= sol:E-lightpen.c17, opus:E-lightpen.c20 =================
    checks.record(
        "sol:E-lightpen.c17, opus:E-lightpen.c20 (p.333 LIGHT PEN figure: 'BPLCON0 "
        "write only  DFF104'): p.333's own figure prints the SAME address for "
        "'POTINP read only' two boxes below. Both readings measured above via note(). "
        "Per the build brief and the NM-HRM-062 precedent: measure both, record both, "
        "grade neither. ADDRESS not graded either way -- BPLCON0's real, write-decoded "
        "address is $100 (measured above: writing there reaches `hires`), and $104 is "
        "BPLCON2's real address (measured above: writing there does not); a claim built "
        "from p.333's printed $DFF104 would red a correct decoder exactly as the "
        "POTINP side would (see appDE-lightpen-userio, sol:E-lightpen.c19/"
        "opus:E-lightpen.c22, the DFF104 half of this SAME page's other printed "
        "collision). DIRECTION: hrm/ch3-register-map-denise's BPLCON0-WO assertion "
        "already grades 'a distinct pattern written to BPLCON0's own address ($100) "
        "reads back $0000 there' in an accepted chapter (p.271/291 'BPLCON0 ... W', "
        "p.229 'BPLCON0 ($DFF100) Bit 3 enables the light pen latch' -- this same LIGHT "
        "PEN topic, one page earlier) -- the write-only half of the label is not in "
        "dispute and, graded separately from the disputed address, is a re-grade, not "
        "new content. Both uids decline in full: this package has zero graded "
        "assertions among its two claims, by design, not by omission -- see this "
        "package's build report for the reasoning trail.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
