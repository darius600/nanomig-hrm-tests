// Test purpose: the disk INDEX pulse's whole downstream journey to the 68000
//   -- paula_floppy's `index` output, across ciab's 8520 FLAG pin, through
//   the shared `int6` line, to a level-6 interrupt request -- per printed
//   pp.245 and 250. RIG over three independently-instantiated Verilated tops
//   (no shared Verilog parent): paula_floppy (+ paula_floppy_fifo), ciab (+
//   cia_int, cia_timera, cia_timerb, cia_timerd), and paula_intcontroller.
//   Full derivation in SPECIFICATION.md (committed aeca195, before this file
//   or any RTL assigned to this fixture was opened).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, printed page 250 ("INDEX
//   (level 6, 8520 Flag pin) -- index sensor triggered.") and printed page
//   245 ("FLAG DSKINDEX* Disk index pulse ($BFDD00, bit 4). Can be used to
//   create a level 6 interrupt. See Appendix F for details."); committed
//   specification aeca195 (SPECIFICATION.md, this package). Claims file:
//   hrm-testpipeline/work/ch8-s2-build/ ch8-disk-index-interrupt-path.jsonl
//   (7 claims, all `classify.status: testable`, all 7 cited on a graded line
//   below -- none declined).
// Non-goals: anything outside the `paula_floppy` fixture this package
//   builds, and any claim it does not cite. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in the chapter's
//   claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// THE RIG'S JOINS (SPECIFICATION.md's own section, reproduced here against
//   the now-open RTL):
//   1. paula_floppy.index -> ciab.flag, reproducing minimig.v:647
//      (`.flag(index)`) combined with minimig.v:526 (`.index(index)`,
//      confirming the wire's origin is paula_floppy's own INDEX port).
//      Made in propagate_joins() below: `cb->flag = pf->index;` before every
//      shared clock edge.
//   2. ciab.irq -> paula_intcontroller.int6, reproducing minimig.v:646
//      (`.irq(int6)`) combined with minimig.v:511 (the same wire on paula's
//      own `.int6(int6)` port; paula_intcontroller's own `int6` port is one
//      level inside `paula`, not independently confirmed by opening
//      paula.v, which is outside this fixture). Made in propagate_joins()
//      below: `pic->int6 = cb->irq;`.
//   No other signal is shared between the three DUTs.
//
// RTL READ FOR STEP 2, after SPECIFICATION.md was committed (aeca195), in
//   this order: paula_floppy.v (full, 1041 lines) and paula_floppy_fifo.v
//   (not opened for content, only confirmed as the sole sub-instance);
//   ciab.v (full) and cia_int.v (full) -- cia_timera.v (full, for the
//   PROP-NOT-OWN-BIT control mechanism) -- cia_timerb.v/cia_timerd.v not
//   opened for content, only present so Verilator can elaborate `ciab`;
//   paula_intcontroller.v (full). Also read, for plumbing only, not for any
//   conformance fact: hrm/ch8-ciabprb-disk-control-bits's test file (ciab
//   bus-write/ICR-read protocol), hrm/ch7-interrupt-priority-encoder's
//   test_paula_intcontroller.cpp (paula_intcontroller's address-shift
//   convention `A(byte_addr)=byte_addr>>1`, the `_ipl` 7-minus-level
//   decoding, already independently confirmed there by three methods), and
//   hrm/ch8-drive-select-motor-led's test file (paula_floppy's Rig
//   conventions: default port values, the _sel/_motor turn-on-edge idiom
//   for latching motor_on, and the fresh-model-per-trial rig hazard for
//   `dsktrack`).
//
// RTL FACTS ESTABLISHED BY THAT READING (none shaped SPECIFICATION.md,
//   which was committed first):
//   - `index` (paula_floppy.v:576): `assign index = |(~_sel & motor_on) &
//     ~|rpm_pulse_cnt & sof;` -- purely combinational. `rpm_pulse_cnt`
//     (paula_floppy.v:563-573) advances only on a clk7_en tick where `sof`
//     is asserted, and has NO reset term at all (like `dsktrack`) -- a
//     second instance of the same hazard SPECIFICATION.md and the build
//     brief name for `dsktrack`. Verilator zero-initialises it, so on a
//     FRESH model it reads 0 until the harness itself asserts `sof` across
//     a real tick(). This is what makes a single, deterministic index pulse
//     cheap to produce: select a drive, latch its motor on via a genuine
//     `_sel` turn-on edge (paula_floppy.v:624-659), then assert `sof` for
//     exactly one shared-clock tick while rpm_pulse_cnt is still 0.
//   - ciab.v:17/185 wires the top-level `flag` port straight into cia_int's
//     own `flag` input with no registering at the ciab level; cia_int.v:49
//     (`icr[4] <= icr[4] | flag`, sticky-OR while ICR is not being read)
//     and cia_int.v:62-66 (`irq = |(icrmask[k] & icr[k])`) are exactly the
//     mechanism already measured once before this package existed (ICR
//     mask write 0x90 unmasks FLG; a `flag` pulse then makes a
//     non-clocked ICR read return 0x90 -- irq=1, icr[4]=1). Reading ICR
//     (icrs && !wr) is combinational into `data_out`; the actual latch-clear
//     only happens on a clocked read (icrs && !wr sampled AT a posedge), so
//     a plain aen/rd assert + eval() (no tick()) peeks without clearing --
//     the same pattern hrm/ch8-ciabprb-disk-control-bits's read_reg() uses.
//   - paula_intcontroller.v:132 (`intreq[13] <= tmp[13] | int6`) and the
//     priority-encoder casez (lines 150-172, confirmed independently by
//     hrm/ch7-interrupt-priority-encoder's own exhaustive 32768-pattern
//     sweep): with ONLY bit 13 (EXTER/int6) reaching intreqena, `_ipl` <= 1,
//     and 68000 level = 7 - _ipl = 6. paula_intcontroller.v:4-24 declares
//     rxint, txint, vblint, int2, int3, int6, blckint, syncint, audint --
//     no port named for the disk index specifically, confirming
//     PROP-NOT-OWN-BIT's premise directly from the port list.
//   - cia_int.v's five ICR sources are ta, tb, alrm, flag, ser; ciab.v:186
//     hardwires `.ser(1'b0)`, so `ser` can never be used as an
//     independent-of-`flag` control mechanism. Timer A (cia_timera.v) is
//     the simplest of the remaining three: writing its latch to 0x0000 and
//     force-loading it (CRA bit4 strobe) makes `zero` (`~|tmr`) true
//     immediately after the load, so a single further `eclk`-qualified tick
//     with `start` set produces `underflow` (`= zero & start & count`) --
//     no actual decrement-to-zero counting needed. This is
//     PROP-NOT-OWN-BIT's control mechanism: a real, live, non-FLG ciab
//     interrupt source, reaching the identical int6/bit13/level6 outcome
//     through the identical join 2, while `flag`/`index` are never touched.
//
// MUTATION METHODOLOGY: every expect() below is guarded by a runtime check,
//   mutating("ID"), reading the MUTATE environment variable at startup --
//   one build serves the real run (MUTATE unset) and every mutation run
//   (MUTATE=<id>). No string literal naming an id appears inside an
//   expect() CONDITION anywhere below -- every such value is hoisted into a
//   local `bool mut = mutating("...")` first, per the build task's own
//   warning about the gate mis-attributing the assertion id otherwise.
//   Nothing in this slice's finding candidates names this package or any of
//   its 7 claim ids as expected red, so -- like
//   hrm/ch8-ciabprb-disk-control-bits and hrm/ch8-adkcon-disk-control-bits
//   -- every assertion here is expected GREEN at baseline, and every
//   mutation is the ordinary ("stimulus ... red") form: change what a DUT
//   is fed, never the expected value, confirm the assertion goes red,
//   revert. See the MUTATION comments at the end of the file for what was
//   actually run and observed (filled in after the real measurement, not
//   before -- per this suite's own rule against writing a record before
//   taking it).
//
// MUTATION RUN LOG (measured, via `MUTATE=<id> ./obj_dir/Vrig` for each id
//   in turn, and `make test MUT=<id>` confirmed to wire the same thing
//   through the Makefile): Baseline (MUTATE unset): 4 checks.expect()
//   calls, 4 passed, 0 failed -- this package carries no expected red.
//   Each of MUTATE=A0-FIXTURE-LIVE-INDEX-CHAIN, opus:8-cia.c56,
//   opus:8-dskint.c8 and opus:8-dskint.c9 was run in turn and each
//   produced exactly 3 passed / 1 failed -- ONLY that run's own targeted
//   label flipped from PASS to FAIL; the other three stayed PASS, in every
//   run. Measured FAIL detail per id: A0-FIXTURE-LIVE-INDEX-CHAIN ->
//   condition(b) collapsed to identical to condition(a), both level=0/
//   bit13=0; opus:8-cia.c56 -> ICR read 0x10 instead of 0x90 (irq never
//   rose, FLG stayed masked); opus:8-dskint.c8 -> level stayed 0 instead
//   of 6 (INTREQR still showed bit 13 set, confirming join 2 itself was
//   unaffected -- only INTENA gated the level); opus:8-dskint.c9 -> ICR
//   read 0x01 (TA still latched internally) but INTREQR read 0x0000 and
//   level stayed 0 (Timer A's real underflow never reached int6). No
//   MUTATE run was left applied afterward; the binary described here is
//   the reverted, baseline (MUTATE-unset) build.
//
// REPAIR / Lot F (independent validation, 2026-09-15): opus:8-dskint.c9's
//   PROP-NOT-OWN-BIT/CONTROL grew a second, always-run condition -- a real
//   index pulse must change EXACTLY bit 13 of INTREQR, no other bit -- a
//   falsifiable check for "index has no additional INTREQ bit of its own"
//   that the original Timer A half alone could not provide (it showed bit
//   13 is shared, not that nothing else moves when index itself pulses).
//   Re-measured baseline (MUTATE unset): still 4 passed, 0 failed;
//   real-index changed_bits=0x2000 (exactly bit 13). MUTATE=opus:8-dskint.c9
//   re-run after this change: still exactly 3 passed / 1 failed, only this
//   label flipped (Timer A's own half still drives the FAIL, since the ICR
//   mask redirection leaves level=0; the new second condition is unaffected
//   by this particular mutation and stays true on its own). No other label
//   moved.

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vpaula_floppy.h"
#include "Vciab.h"
#include "Vpaula_intcontroller.h"

namespace {
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
std::string hex8(uint32_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "0x%02X", (unsigned)(v & 0xFF));
    return std::string(buf);
}
std::string hex16(uint32_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "0x%04X", (unsigned)(v & 0xFFFF));
    return std::string(buf);
}
}  // namespace

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// ciab register-select constants (ciab.v's address decoder, rs[3:0]).
constexpr uint8_t RS_PRB  = 0x1;
constexpr uint8_t RS_DDRB = 0x3;
constexpr uint8_t RS_TALO = 0x4;
constexpr uint8_t RS_TAHI = 0x5;
constexpr uint8_t RS_CRA  = 0xE;
constexpr uint8_t RS_ICR  = 0xD;

// paula_intcontroller register addresses: reg_address_in is [8:1], i.e. the
// byte address with bit 0 dropped (byte_addr >> 1) -- confirmed against
// hrm/ch7-interrupt-priority-encoder's own working A()/wr()/rd() helpers.
uint16_t A(int byte_addr) { return (uint16_t)(byte_addr >> 1); }
const uint16_t INTENA  = A(0x09a);
const uint16_t INTREQR = A(0x01e);

// RIG HAZARD (SPECIFICATION.md, build brief): `dsktrack` has no reset term
// (paula_floppy.v:671-680); `rpm_pulse_cnt` (paula_floppy.v:563-573) is the
// SAME shape -- also no reset term. A reused Verilated paula_floppy would
// carry stale head position AND stale rotation phase into the next trial.
// A fresh Rig (all three DUTs) is allocated per trial below; none is ever
// reused across two measurements.
struct Rig {
    Vpaula_floppy* pf;
    Vciab* cb;
    Vpaula_intcontroller* pic;

    Rig() {
        pf = new Vpaula_floppy;
        cb = new Vciab;
        pic = new Vpaula_intcontroller;

        // paula_floppy defaults, reused from hrm/ch8-drive-select-motor-led's
        // Rig() -- verified RTL-interface plumbing, not a conformance fact.
        pf->clk = 0; pf->clk7_en = 1; pf->clk7n_en = 0;
        pf->reset = 1; pf->ntsc = 0; pf->sof = 0; pf->enable = 0;
        pf->reg_address_in = 0; pf->data_in = 0;
        pf->_step = 1; pf->direc = 1; pf->_sel = 0xF; pf->side = 0; pf->_motor = 1;
        pf->wordsync = 0;
        pf->floppy_drives = 3;
        pf->floppy_wrprot = 0;
        pf->sdc_image_mounted = 0; pf->sdc_image_size = 0;
        pf->sdc_busy = 0; pf->sdc_done = 0;
        pf->sdc_byte_in_strobe = 0; pf->sdc_byte_addr = 0; pf->sdc_byte_in_data = 0;

        // ciab defaults, reused from hrm/ch8-ciabprb-disk-control-bits's
        // Harness() -- verified RTL-interface plumbing.
        cb->clk = 0; cb->clk7_en = 1;
        cb->aen = 0; cb->rd = 0; cb->wr = 0; cb->reset = 1; cb->rs = 0; cb->data_in = 0;
        cb->tick = 0; cb->eclk = 0; cb->flag = 0; cb->porta_in = 0;

        // paula_intcontroller defaults, reused from
        // hrm/ch7-interrupt-priority-encoder's own init.
        pic->clk = 0; pic->clk7_en = 1; pic->reset = 1;
        pic->reg_address_in = 0; pic->data_in = 0;
        pic->rxint = 0; pic->txint = 0; pic->vblint = 0;
        pic->int2 = 0; pic->int3 = 0; pic->int6 = 0;
        pic->blckint = 0; pic->syncint = 0; pic->audint = 0;

        eval_settled();
        settle(8);
        pf->reset = 0; cb->reset = 0; pic->reset = 0;
        settle(8);
    }
    ~Rig() {
        pf->final(); cb->final(); pic->final();
        delete pf; delete cb; delete pic;
    }
    Rig(const Rig&) = delete;
    Rig& operator=(const Rig&) = delete;

    // The rig's own two joins (SPECIFICATION.md), made once per settle
    // round, entirely in software: no Verilog top wires these three
    // modules together anywhere in this fixture.
    void propagate_joins() {
        cb->flag = pf->index;   // join 1: minimig.v:526 + :647
        pic->int6 = cb->irq;    // join 2: minimig.v:646 + :511
    }

    void eval_settled() {
        for (int i = 0; i < 3; ++i) {
            pf->eval(); cb->eval(); pic->eval();
            propagate_joins();
        }
        pf->eval(); cb->eval(); pic->eval();
    }

    void tick() {
        eval_settled();
        pf->clk = 1; cb->clk = 1; pic->clk = 1;
        eval_settled();
        pf->clk = 0; cb->clk = 0; pic->clk = 0;
        eval_settled();
    }
    void settle(int n) { for (int i = 0; i < n; ++i) tick(); }

    // Deselect every drive, then select ONLY drive n with _motor driven to
    // the requested level held across the turn-on edge -- reused idiom from
    // hrm/ch8-drive-select-motor-led.
    void select_with_motor(int n, bool motor_on_level) {
        pf->_sel = 0xF;
        settle(4);
        pf->_motor = motor_on_level ? 0 : 1;  // DSKMOTOR* is active low
        pf->_sel = (uint8_t)(0xF & ~(1u << n));
        settle(4);
    }
    void deselect_all() { pf->_sel = 0xF; settle(4); }

    // Drive n must already be selected with its motor latched on (a prior
    // select_with_motor(n, true) call). Asserts `sof` for exactly one
    // shared-clock tick -- rpm_pulse_cnt is still 0 on a fresh Rig that has
    // never asserted `sof` before, so `index` is combinationally 1 during
    // that tick and join 1 carries it onto ciab.flag at ciab's own edge.
    // Settles further afterward so join 2's downstream (int6 -> intreq bit
    // 13 -> _ipl) has time to propagate before the caller reads it.
    void pulse_index_once() {
        pf->sof = 1;
        tick();
        pf->sof = 0;
        settle(8);
    }

    void cb_write(uint8_t rs, uint8_t data) {
        cb->aen = 1; cb->wr = 1; cb->rd = 0; cb->rs = rs; cb->data_in = data;
        tick();
        cb->aen = 0; cb->wr = 0; cb->rs = 0; cb->data_in = 0;
        tick();
    }
    // Non-clocked ICR peek: asserts aen/rd, settles combinationally (no
    // clk edge), captures data_out, deasserts. Does NOT clear the ICR
    // latch -- clearing only happens on a clocked (icrs && !wr) read, which
    // this deliberately avoids, exactly as
    // hrm/ch8-ciabprb-disk-control-bits's read_reg() does.
    uint8_t cb_peek_icr() {
        cb->aen = 1; cb->rd = 1; cb->rs = RS_ICR;
        eval_settled();
        const uint8_t v = cb->data_out;
        cb->aen = 0; cb->rd = 0; cb->rs = 0;
        eval_settled();
        return v;
    }

    void pic_write(uint16_t addr, uint16_t data) {
        pic->reg_address_in = addr; pic->data_in = data;
        tick(); tick();
        pic->reg_address_in = 0; pic->data_in = 0;
    }
    uint16_t pic_read(uint16_t addr) {
        pic->reg_address_in = addr;
        tick();
        const uint16_t v = pic->data_out;
        tick();
        pic->reg_address_in = 0;
        return v;
    }
    int level_now() { return 7 - (int)pic->_ipl; }

    // PROP-NOT-OWN-BIT's control mechanism: force-load Timer A's latch
    // (0x0000) and hold `start`+`eclk` -- `zero` is already true right
    // after the load, so the very next eclk-qualified tick produces a real
    // `underflow` (cia_timera.v), independent of `flag`/`index` entirely.
    void arm_timer_a_underflow() {
        cb_write(RS_TALO, 0x00);
        cb_write(RS_TAHI, 0x00);
        cb->eclk = 1;
        cb_write(RS_CRA, 0x11);  // bit4 forceload strobe, bit0 start
        settle(16);
    }
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // ========================================================================
    // Standing non-goal, per the build task: `index` is SYNTHESISED inside
    // paula_floppy (paula_floppy.v:576) from `_sel`, `motor_on`,
    // `rpm_pulse_cnt` and `sof` -- there is no sensor input, no medium, and
    // no physical index hole anywhere in this design. Page 250's own quoted
    // text says "index sensor triggered"; none of this package's 7 claims'
    // restatements assert that causal step (all 7 restate only the FLAG-pin
    // routing and/or the level), so none is declined for it -- but the
    // boundary is real and is recorded here rather than silently assumed.
    // ========================================================================
    checks.record(
        "Standing non-goal: this package grades only what happens to a "
        "pulse on paula_floppy's `index` output FROM that point downstream "
        "-- the FLAG pin, the ICR latch, `irq`, `int6`, and the level-6 "
        "request. It does not and cannot grade where the pulse itself comes "
        "from physically: `index` is synthesised (paula_floppy.v:576, "
        "`|(~_sel & motor_on) & ~|rpm_pulse_cnt & sof`), not driven by any "
        "modelled sensor pin. No claim in this package's own 7 restates "
        "page 250's \"index sensor triggered\" as a causal claim, so none "
        "is declined for it specifically, but a claim of that shape would "
        "have no observable at this or any boundary in this design.");

    // ========================================================================
    // A0 -- FIXTURE LIVENESS. No claim id, no manual page: the build brief's
    // own requirement that the fixture prove it can deliver a known,
    // distinct word before any read-based assertion is trusted. Two
    // distinct conditions at paula_floppy's boundary (never pulse `index`,
    // vs. really pulse it) must produce two distinctly different readings
    // at paula_intcontroller's boundary, through the WHOLE rig (both
    // joins), before PROP-FLAG-PIN/PROP-LEVEL6/PROP-NOT-OWN-BIT are trusted.
    // ========================================================================
    {
        Rig r;
        r.cb_write(RS_ICR, 0x90);           // unmask FLG (bit7 set-mode, bit4 FLG)
        r.pic_write(INTENA, 0xE000);        // master (bit14) + EXTER (bit13)

        // Condition (a): drive0 selected+motor on, but `sof` never pulsed.
        r.select_with_motor(0, true);
        r.settle(8);
        const int level_a = r.level_now();
        const bool bit13_a = (r.pic_read(INTREQR) & (1u << 13)) != 0;

        // MUTATION A0-FIXTURE-LIVE-INDEX-CHAIN stimulus red: condition (b)
        // driven IDENTICAL to condition (a) -- `sof` never pulsed either --
        // instead of the real pulse, so the two readings cannot be told
        // apart.
        bool mut = mutating("A0-FIXTURE-LIVE-INDEX-CHAIN");
        int level_b;
        bool bit13_b;
        if (!mut) {
            r.pulse_index_once();
            level_b = r.level_now();
            bit13_b = (r.pic_read(INTREQR) & (1u << 13)) != 0;
        } else {
            r.settle(8);
            level_b = r.level_now();
            bit13_b = (r.pic_read(INTREQR) & (1u << 13)) != 0;
        }
        checks.note("A0: condition(a) no index pulse -> level=" + std::to_string(level_a) +
                    " bit13=" + std::to_string(bit13_a) + "; condition(b) " +
                    std::string(mut ? "also no pulse (mutated)" : "real index pulse") +
                    " -> level=" + std::to_string(level_b) + " bit13=" + std::to_string(bit13_b));
        checks.expect(level_a == 0 && !bit13_a && level_b == 6 && bit13_b && level_a != level_b,
            "A0-FIXTURE-LIVE-INDEX-CHAIN: two distinct, known conditions at paula_floppy's boundary "
            "(no index pulse vs. a real one) produce two distinctly different, correctly-differing "
            "readings at paula_intcontroller's boundary through the whole rig (both joins) -- the "
            "index->flag->irq->int6->level6 data path is live, not stuck [no page -- infrastructure]");
    }

    // ========================================================================
    // PROP-FLAG-PIN -- opus:8-cia.c56 (p.245, "FLAG DSKINDEX* Disk index
    // pulse"), positive half of opus:8-dskint.c9 and all of sol:8-int.c9
    // (p.250, "...arrives on/is connected through the 8520 Flag pin").
    // A pulse on paula_floppy's real `index`, carried across join 1, is
    // what ciab treats as the FLAG pin's DSKINDEX* pulse: it sets ICR bit 4
    // (FLG) and, with FLG unmasked, raises `irq`.
    // ========================================================================
    {
        Rig r;
        bool mut = mutating("opus:8-cia.c56");
        // MUTATION opus:8-cia.c56 stimulus red: the FLG unmask write is
        // withheld (ICR mask stays all-clear from reset) -- icr[4] still
        // latches internally, but `irq` cannot rise from it.
        if (!mut) r.cb_write(RS_ICR, 0x90);
        r.select_with_motor(0, true);
        r.pulse_index_once();
        const uint8_t icr_read = r.cb_peek_icr();
        checks.note("PROP-FLAG-PIN: ICR read after a real index pulse, FLG " +
                    std::string(mut ? "left masked (mutated)" : "unmasked") +
                    " -> data_out=" + hex8(icr_read));
        checks.expect(icr_read == 0x90,
            "opus:8-cia.c56 opus:8-dskint.c9 sol:8-int.c9 PROP-FLAG-PIN: a pulse on paula_floppy's real "
            "`index` output, carried across join 1 (minimig.v:526+:647) onto ciab's `flag` input, is what "
            "ciab treats as the FLAG pin's DSKINDEX* pulse -- ICR bit 4 (FLG) latches and, with FLG "
            "unmasked, `irq` rises; a non-clocked ICR read returns 0x90 (irq=1, FLG=1) [p.245 \"FLAG "
            "DSKINDEX* Disk index pulse ($BFDD00, bit 4)\"; p.250 \"...8520 Flag pin...\"]");
    }

    // ========================================================================
    // PROP-LEVEL6 -- opus:8-dskint.c8, sol:8-int.c8 (p.250, "INDEX (level 6,
    // 8520 Flag pin)"), opus:8-cia.c58, sol:8-cia.c52 (p.245, "Can be used
    // to create a level 6 interrupt"). A pulse on `index`, carried through
    // BOTH joins with FLG unmasked and INTENA (master + bit 13) enabled,
    // ends with paula_intcontroller requesting 68000 level 6.
    // ========================================================================
    {
        Rig r;
        r.cb_write(RS_ICR, 0x90);   // unmask FLG
        bool mut = mutating("opus:8-dskint.c8");
        // MUTATION opus:8-dskint.c8 stimulus red: the INTENA write is
        // withheld -- intreq bit 13 still latches (join 2 still carries
        // `irq` into `int6`), but intreqena stays 0 (intena[14]=0 gates
        // everything off), so `_ipl` cannot leave idle.
        if (!mut) r.pic_write(INTENA, 0xE000);
        r.select_with_motor(0, true);
        r.pulse_index_once();
        const uint16_t reqr = r.pic_read(INTREQR);
        const int level = r.level_now();
        checks.note("PROP-LEVEL6: INTENA " + std::string(mut ? "withheld (mutated)" : "set (master+EXTER)") +
                    ", after a real index pulse -> INTREQR=" + hex16(reqr) + " level=" + std::to_string(level));
        checks.expect(level == 6 && (reqr & (1u << 13)) != 0,
            "opus:8-dskint.c8 sol:8-int.c8 opus:8-cia.c58 sol:8-cia.c52 PROP-LEVEL6: a pulse on `index`, "
            "carried through join 1 (minimig.v:526+:647) and join 2 (minimig.v:646+:511) with FLG "
            "unmasked and INTENA's master+bit13 enabled, sets INTREQ bit 13 and drives paula_intcontroller "
            "to request 68000 level 6 (measured _ipl=1, level=7-_ipl=6) [p.250 \"INDEX (level 6, 8520 Flag "
            "pin)\"; p.245 \"Can be used to create a level 6 interrupt\"]");
    }

    // ========================================================================
    // PROP-NOT-OWN-BIT / CONTROL -- opus:8-dskint.c9 (p.250, negative half:
    // "...not through an INTREQ bit of its own").
    //
    // REPAIRED (independent validation, 2026-09-15, Lot F): the withdrawn
    // form drove ONLY Timer A (a DIFFERENT, non-FLG source) to a real
    // underflow and showed IT ALSO reaches bit13/level6 -- proof that bit
    // 13 is genuinely SHARED, but not, by itself, proof that INDEX has NO
    // additional bit of its own: a coexisting index-specific INTREQ bit
    // could exist alongside the shared one and this check would still
    // pass, since it never looks at what a real index pulse itself does to
    // the REST of INTREQR. The port-list argument in the prose ("no port
    // named for the disk index") was correct but was not, by itself, a
    // falsifiable condition the code checked.
    //
    // Repaired to add a second, direct condition: pulse a REAL index (the
    // same mechanism PROP-LEVEL6 uses) and read the WHOLE INTREQR word
    // before and after. If index had an additional, dedicated bit anywhere
    // in paula_intcontroller's own latch array, that bit would move too,
    // and `changed_bits == (1 << 13)` would fail -- an actual hypothesis
    // this check can now break, not just narrate. The original Timer A
    // half is kept alongside it (still a real, complementary fact: bit 13
    // is reachable from a source that never touches index at all), and
    // both conditions must hold.
    // ========================================================================
    {
        Rig r;
        r.pic_write(INTENA, 0xE000);       // master + EXTER, same as PROP-LEVEL6
        bool mut = mutating("opus:8-dskint.c9");
        // MUTATION opus:8-dskint.c9 stimulus red: the ICR mask targets a
        // DIFFERENT bit (FLG, bit4) instead of TA (bit0) -- Timer A still
        // underflows internally (icr[0] latches), but stays masked out, so
        // `irq` never rises and int6/bit13/level6 are never reached.
        r.cb_write(RS_ICR, mut ? uint8_t{0x90} : uint8_t{0x81});
        r.arm_timer_a_underflow();
        const uint8_t icr_read = r.cb_peek_icr();
        const uint16_t reqr_timera = r.pic_read(INTREQR);
        const int level_timera = r.level_now();

        // Second, independent Rig: a real index pulse, full-word before/
        // after comparison. Not gated by `mut` -- opus:8-dskint.c9's own
        // mutation targets the Timer A half above; this half is a fixed,
        // always-run structural check.
        Rig r2;
        r2.cb_write(RS_ICR, 0x90);            // unmask FLG
        r2.pic_write(INTENA, 0xE000);
        const uint16_t reqr_before = r2.pic_read(INTREQR);
        r2.select_with_motor(0, true);
        r2.pulse_index_once();
        const uint16_t reqr_after = r2.pic_read(INTREQR);
        const uint16_t changed_bits = reqr_before ^ reqr_after;

        checks.note("PROP-NOT-OWN-BIT/CONTROL: Timer A underflow, ICR mask " +
                    std::string(mut ? "targets FLG instead of TA (mutated)" : "targets TA (bit0)") +
                    " -> ICR=" + hex8(icr_read) + " INTREQR=" + hex16(reqr_timera) + " level=" + std::to_string(level_timera) +
                    "; real index pulse -> INTREQR before=" + hex16(reqr_before) + " after=" + hex16(reqr_after) +
                    " changed_bits=" + hex16(changed_bits));
        checks.expect(
            level_timera == 6 && (reqr_timera & (1u << 13)) != 0 &&
            changed_bits == (1u << 13),
            "opus:8-dskint.c9 PROP-NOT-OWN-BIT/CONTROL: paula_intcontroller declares no port for the disk "
            "index (paula_intcontroller.v:4-24: rxint, txint, vblint, int2, int3, int6, blckint, syncint, "
            "audint -- nothing named for INDEX), so INTREQ bit 13 can only ever be reached through the "
            "shared `int6` line. Graded by TWO falsifiable conditions: (1) a DIFFERENT, non-FLG ciab source "
            "(Timer A, forced to underflow), through join 2 alone with `flag`/`index` never touched, reaches "
            "the IDENTICAL bit13/level6 outcome, proving bit 13 is `int6`'s bit, not a dedicated INDEX bit; "
            "(2) a REAL index pulse changes EXACTLY bit 13 of INTREQR and no other bit -- a hypothesis where "
            "index also drives some additional, coexisting INTREQ bit of its own would flip this condition "
            "false, which condition (1) alone could not do. Both hold [p.250 \"...not through an INTREQ bit "
            "of its own\"]");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION A0-FIXTURE-LIVE-INDEX-CHAIN stimulus red: condition (b) driven identical to condition (a) -- sof never pulsed either -- instead of a real index pulse, so the two readings cannot be told apart.
// MUTATION opus:8-cia.c56 stimulus red: the FLG unmask write to ciab's ICR is withheld, so icr[4] still latches from a real index pulse but `irq` cannot rise from it.
// MUTATION opus:8-dskint.c8 stimulus red: the INTENA write on paula_intcontroller is withheld, so INTREQ bit 13 still latches via join 2 but intreqena stays 0 and `_ipl` cannot leave idle.
// MUTATION opus:8-dskint.c9 stimulus red: the ICR mask targets FLG (bit4) instead of TA (bit0), so Timer A's real underflow latches into icr[0] but stays masked out and never reaches int6/bit13/level6.
