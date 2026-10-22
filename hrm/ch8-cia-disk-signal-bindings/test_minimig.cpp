// Test purpose: the 24 claims p.244-245's Table 8-5 makes about which named
//   drive signal sits on which CIA port bit -- PA5 DSKRDY*, PA4 DSKTRACK0*,
//   PA3 DSKPROT*, PA2 DSKCHANGE* (CIAAPRA, $BFE001, read side) and PB7
//   DSKMOTOR*, PB6 DSKSEL3*, PB5 DSKSEL2*, PB4 DSKSEL1*, PB3 DSKSEL0*, PB2
//   DSKSIDE, PB1 DSKDIREC, PB0 DSKSTEP* (CIABPRB, $BFD100, write side).
//   hrm/ch8-ciaapra-disk-sense-bits and hrm/ch8-ciabprb-disk-control-bits
//   both declined all 24 (`record()`, not graded): at the `ciaa`/`ciab`
//   boundary alone, the port is a generic porta_in[7:2]/portb_out[7:0] with
//   no signal named DSKRDY* etc -- the binding is one level up, at
//   minimig.v:632/660. This package is built specifically to pick those 24
//   claims up at that join, per the build brief handed down for this unit.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp.244-245, Table 8-5. Exact uid strings and page quotations are taken
//   verbatim from the `record()` lines already committed in
//   hrm/ch8-ciaapra-disk-sense-bits/test_ch8-ciaapra-disk-sense-bits.cpp and
//   hrm/ch8-ciabprb-disk-control-bits/test_ch8-ciabprb-disk-control-bits.cpp
//   -- not retyped from the build brief, and neither of those two files is
//   modified by this package (read-only siblings; their own record() lines
//   stay exactly as they are).
// Non-goals: anything outside the `minimig` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// THE JOIN, verified by direct reading before this file was written:
//   minimig.v:385-392   wire _sel1,_sel2,_sel3,side,_motor,_track0,_change,
//                        _ready,_wprot; (declaration order differs from the
//                        brief's paraphrase; the four names are identical)
//   minimig.v:617-640   ciaa CIAA1 (...
//                        .porta_in({_fire1,_fire0,_ready,_track0,_wprot,_change}),
//                        ...)
//   minimig.v:643-661   ciab CIAB1 (...
//                        .portb_out({_motor,_sel3,_sel2,_sel1,_sel0,side,direc,_step})
//                        );
//   minimig.v:494-558   paula PAULA1 (... ._step(_step), .direc(direc),
//                        ._sel({_sel3,_sel2,_sel1,_sel0}), .side(side),
//                        ._motor(_motor), ._track0(_track0), ._change(_change),
//                        ._ready(_ready), ._wprot(_wprot), ...
//                        .floppy_wrprot(floppy_config[1]),
//                        .floppy_drives(floppy_config[3:2]) ... );
//   ciaa.v:95           input [7:2] porta_in
//   ciaa.v:253-254      pa_out[7:0] = {porta_in2[7:2],porta_out[1:0]} (PRA
//                        read mux; bits 7:2 are UNCONDITIONALLY the
//                        synchronised external input, confirmed already by
//                        hrm/ch8-ciaapra-disk-sense-bits' own PROP-INPUT)
//   ciab.v               portb_out[7:0] = (~ddrportb)|regportb (confirmed
//                        already by hrm/ch8-ciabprb-disk-control-bits' own
//                        PROP-POSITION)
//   gary.v:190-192      sel_cia=cpu_address_in[23:16]==8'hBF;
//                        sel_cia_a=sel_cia&~cpu_address_in[12];
//                        sel_cia_b=sel_cia&~cpu_address_in[13]; -- verified
//                        against $BFE001 (selects A, rs=0=PRA) and $BFD100
//                        (selects B, rs=1=PRB) by direct measurement before
//                        this file's real assertions were written (see
//                        STEP3 notes / commit history for this package).
//   So: the six-signal concatenation at minimig.v:632 lands on PA7..PA2 as
//   {_fire1,_fire0,_ready,_track0,_wprot,_change} -- _ready=PA5, _track0=PA4,
//   _wprot=PA3, _change=PA2, exactly what Table 8-5 requires. Confirmed by
//   this package's own measurements below, not merely read off the source.
//
// FIXTURE: TOP_MODULE=minimig, the whole chipset (Makefile copied from
//   hrm/ch7-overlay-source, +--public-flat-rw so the join's internal nets --
//   _ready/_track0/_wprot/_change and _motor/_sel3..0/side/direc/_step --
//   can be OBSERVED via dut->rootp->minimig-><signal> (verified against the
//   generated Vminimig_minimig.h field names in an earlier placeholder
//   build, same discipline hrm/ch7-nmi-level-seven's own header documents).
//   Nothing is ever forced through this path -- only ever read.
//   Same boundary and CPU-bus protocol (address/data/_as/_uds/_lds/r_w/
//   _dtack, the ClkGen clock-enable state machine reproducing amiga_clk.v,
//   the long _cpu_reset release wait) as hrm/ch7-overlay-source's own
//   test_minimig.cpp -- that harness shape is reused verbatim below (it is
//   this suite's own established, already-validated plumbing for this exact
//   fixture, not a source of this package's assertions or expected values).
//   bus_cycle()'s byte-lane/address-shift arithmetic and the $BFD100/rs=1
//   mapping were independently re-derived and confirmed by direct
//   measurement (a validation build, kept in this package's own history)
//   before being trusted for the real assertions below.
//
// STIMULUS SOURCE (the disk-side half, PA claims only): hrm/ch8-drive-sense-
//   lines already builds paula_floppy alone and its 12 assertions were
//   independently retained in full -- the only package in this slice left
//   completely untouched. Its Rig (mount/unmount, select/deselect, do_step,
//   the step_ena/dsktrack timing notes) is this package's trusted stimulus
//   recipe, carried forward here through the CIAB CPU-bus join instead of
//   direct port pokes (paula_floppy's _step/direc/_sel/side/_motor are, at
//   this boundary, the SAME wires ciab's portb_out drives). Two differences
//   from that trusted Rig, both forced by moving one level up:
//     1. step_ena_cnt/dsktrack have no RTL reset term (confirmed by that
//        package's own reading); this harness's long _cpu_reset wait idles
//        with no drive selected (_selx=1) throughout, so dsktrack never
//        moves during it, but the DDRB-write transient (regportb briefly
//        reads 0 the instant ddrportb commits, before the first real PRB
//        write) can present one extra, uncontrolled _step edge. HOME-ZERO
//        below (repeated outward/refused steps) removes any dependence on
//        that transient rather than reasoning about its exact effect.
//     2. clk7_ticks() counts genuine clk7_en-gated ticks (this fixture's
//        clk7_en pulses roughly 1 raw clk in 4, unlike the direct-Rig
//        harness which held clk7_en permanently high) so every timing
//        constant carried over from the trusted recipe (the step low/high
//        phase, the 300-tick step_ena settle margin) means the same thing
//        it meant there.
//
// WHAT MAKES AN ASSERTION HERE SOUND (build brief): drive the named signal
//   to a known state through real stimulus; read the named CIA register
//   through a real CPU bus access at its printed address; assert the named
//   bit carries it AND that the other bits in its group do not. Every PA
//   check below reads a 4-bit (PA5,PA4,PA3,PA2) nibble from a real CIAAPRA
//   CPU read and compares the WHOLE nibble against an expected pattern in
//   which exactly one bit is asserted -- a swap between any two of the four
//   positions changes which bit reads low and is caught. Every PB check
//   reads all eight named join wires after one CPU write and requires the
//   target wire low AND the other seven high -- same property, write side.
//
// CLAIM COVERAGE. All 24, exact uid strings/quotes copied from the two
//   sibling packages' own record() lines (not retyped from the build
//   brief):
//     PA5/_ready   opus:8-cia.c6  sol:8-cia.c5   -- PA5-BINDING
//     PA4/_track0  opus:8-cia.c11 sol:8-cia.c9   -- PA4-BINDING
//     PA3/_wprot   opus:8-cia.c16 sol:8-cia.c16  -- PA3-BINDING
//     PA2/_change  opus:8-cia.c18 sol:8-cia.c17  -- PA2-BINDING
//     PB7/_motor   opus:8-cia.c21 sol:8-cia.c20  -- PB7-BINDING
//     PB6/_sel3    opus:8-cia.c32 sol:8-cia.c30  -- PB6-BINDING
//     PB5/_sel2    opus:8-cia.c34 sol:8-cia.c31  -- PB5-BINDING
//     PB4/_sel1    opus:8-cia.c36 sol:8-cia.c32  -- PB4-BINDING
//     PB3/_sel0    opus:8-cia.c38 sol:8-cia.c33  -- PB3-BINDING
//     PB2/side     opus:8-cia.c41 sol:8-cia.c35  -- PB2-BINDING
//     PB1/direc    opus:8-cia.c45 sol:8-cia.c39  -- PB1-BINDING
//     PB0/_step    opus:8-cia.c49 sol:8-cia.c44  -- PB0-BINDING
//   Each uid's FULL quoted text (as carried by the declining sibling) often
//   bundles more than the binding -- e.g. opus:8-cia.c6 also quotes DSKRDY*'s
//   motor-validity clause, already graded (as a decline, correctly, per that
//   page's own next sentence) by hrm/ch8-drive-sense-lines under the
//   DIFFERENT uid opus:8-cia.c7. The declining package's own comment on each
//   uid scopes what that uid is understood to cover here: PA5-BINDING grades
//   the position+polarity content of opus:8-cia.c6/sol:8-cia.c5 (both
//   inherent in a bit-exact nibble comparison), not the motor clause, which
//   this uid's own citing sibling already says belongs to a different uid
//   entirely. Same pattern for opus:8-cia.c11 (software/step-refusal clause
//   already graded under opus:8-cia.c15, a different uid) and opus:8-cia.c18
//   (retention/latch clause already graded under opus:8-cia.c20, a different
//   uid). No uid is placed on a graded line here that this reasoning does
//   not support.
//
// MUTATION LOG -- every MUT=<NAME> variant below was actually built and run
//   (`make MUT=<NAME> test`), WSL/Verilator 5.052. Baseline (no MUT): 15
//   checks.expect() calls, 15 passed, 0 failed. Each of the 15 variants
//   below was run and confirmed 14 passed / 1 failed -- ONLY the targeted
//   line flipped from PASS to FAIL relative to baseline, no cross-
//   contamination between assertions -- EXCEPT MUT_RESET, whose target
//   (CONTROL-RESET-RELEASES) is the harness's own precondition gate: with
//   it never releasing, main() halts after printing the one FAIL, so that
//   run reads 0 passed / 1 failed by design, not a second failure. All 15
//   mutations are `stimulus`; every expected value in every matching
//   expect() was left untouched.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include "Vminimig.h"
#include "verilated.h"
#include "Vminimig___024root.h"
#include "Vminimig_minimig.h"

#ifndef MUT_RESET
#define MUT_RESET 0
#endif
#ifndef MUT_FIXPB
#define MUT_FIXPB 0
#endif
#ifndef MUT_FIXPA
#define MUT_FIXPA 0
#endif
#ifndef MUT_PA5
#define MUT_PA5 0
#endif
#ifndef MUT_PA4
#define MUT_PA4 0
#endif
#ifndef MUT_PA3
#define MUT_PA3 0
#endif
#ifndef MUT_PA2
#define MUT_PA2 0
#endif
#ifndef MUT_PB7
#define MUT_PB7 0
#endif
#ifndef MUT_PB6
#define MUT_PB6 0
#endif
#ifndef MUT_PB5
#define MUT_PB5 0
#endif
#ifndef MUT_PB4
#define MUT_PB4 0
#endif
#ifndef MUT_PB3
#define MUT_PB3 0
#endif
#ifndef MUT_PB2
#define MUT_PB2 0
#endif
#ifndef MUT_PB1
#define MUT_PB1 0
#endif
#ifndef MUT_PB0
#define MUT_PB0 0
#endif

namespace {

// Reproduces amiga_clk.v's exact register update -- verbatim from
// hrm/ch7-overlay-source/test_minimig.cpp (this suite's own validated
// plumbing for this fixture, not a source of this package's own
// assertions).
struct ClkGen {
    uint8_t clk7_cnt = 2;
    bool clk7_en = true, clk7n_en = true, cck = true;
    uint16_t shifter = 1;
    bool c1 = false, c3 = false;
    void step() {
        uint8_t old_cnt = clk7_cnt;
        bool old_c3 = c3;
        bool old_cck = cck;
        uint16_t old_shifter = shifter;
        bool old_clk7 = (old_cnt >> 1) & 1;
        clk7_cnt = (old_cnt + 1) & 0x3;
        clk7_en = (old_cnt == 0);
        clk7n_en = (old_cnt == 2);
        if (old_cnt == 1) {
            cck = !old_cck;
            uint16_t rotated = (uint16_t)(((old_shifter << 1) | (old_shifter >> 9)) & 0x3FF);
            shifter = rotated ? rotated : 1;
        } else {
            cck = old_cck;
            shifter = old_shifter;
        }
        c3 = old_clk7;
        c1 = !old_c3;
    }
};

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

struct PBJoin { int motor, sel3, sel2, sel1, sel0, side, direc, step; };

class Harness {
public:
    Vminimig* dut;
    ClkGen cg;
    long long tickcount = 0;
    bool reset_released = false;
    long long reset_release_tick = -1;
    uint8_t pb_state = 0xFF; // shadow of the last byte written to CIAB PRB

    Harness() : dut(new Vminimig) {
        dut->clk = 0;
        dut->clk7_en = cg.clk7_en; dut->clk7n_en = cg.clk7n_en;
        dut->c1 = cg.c1; dut->c3 = cg.c3; dut->cck = cg.cck; dut->eclk = cg.shifter;
        dut->rst_ext = 0;
        dut->_cpu_reset_in = 0;
        dut->cpu_address = 0; dut->cpu_r_w = 1;
        dut->_cpu_as = 1; dut->_cpu_uds = 1; dut->_cpu_lds = 1;
        dut->cpudata_in = 0;
        dut->memory_config = 0; dut->chipset_config = 0; dut->floppy_config = 0;
        dut->nmi_addr = 0; dut->chip48 = 0; dut->ramdata_in = 0;
        dut->rxd = 1; dut->cts = 1; dut->dsr = 1; dut->cd = 1; dut->ri = 1;
        dut->_joy1 = 0xFFFF; dut->_joy2 = 0xFFFF; dut->_joy3 = 0xFFFF; dut->_joy4 = 0xFFFF;
        dut->joya1 = 0; dut->joya2 = 0; dut->mouse_btn = 0;
        dut->kms_level = 0; dut->kbd_mouse_type = 0; dut->kbd_mouse_data = 0;
        dut->sdc_img_mounted = 0; dut->sdc_img_size = 0; dut->sdc_busy = 0; dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0; dut->sdc_byte_addr = 0; dut->sdc_byte_in_data = 0;
        dut->cpucfg = 0;

        // Same harness-hygiene throwaway _as edge as ch7-overlay-source's
        // own Harness (see that file's header for why: Verilator zero-inits
        // minimig_m68k_bridge.v's `_ta_n`, so _cpu_dtack reads a stale 0
        // until the first _as low->high edge).
        dut->_cpu_as = 0; tick(); tick();
        dut->_cpu_as = 1; tick(); tick();

#if MUT_RESET
        dut->rst_ext = 1; // MUTATION CONTROL-RESET-RELEASES: hold master reset forever.
#endif
        for (int i = 0; i < 8; ++i) tick();
        dut->_cpu_reset_in = 1;

        const long long BUDGET = 3000000;
        for (long long i = 0; i < BUDGET && !reset_released; ++i) {
            tick();
            if (dut->_cpu_reset == 1) { reset_released = true; reset_release_tick = tickcount; }
        }
        for (int i = 0; i < 8; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        cg.step();
        dut->clk7_en = cg.clk7_en; dut->clk7n_en = cg.clk7n_en;
        dut->c1 = cg.c1; dut->c3 = cg.c3; dut->cck = cg.cck; dut->eclk = cg.shifter;
        ++tickcount;
    }

    // Counts genuine clk7_en-gated ticks (dut->clk7_en, as it will read on
    // the NEXT posedge -- already the value tick() leaves it in from the
    // previous call), matching what hrm/ch8-drive-sense-lines' own Rig
    // called a "tick" under its permanently-clk7_en-high harness.
    void clk7_ticks(int n) {
        int count = 0;
        while (count < n) {
            bool en = (bool)dut->clk7_en;
            tick();
            if (en) ++count;
        }
    }

    // Verbatim (data-carrying) extension of ch7-overlay-source's own
    // bus_cycle(): same dtack-wait/settle shape, plus an actual write_data
    // payload (that package never needed one -- minimig.v:872's ovl mux
    // reads only the write strobe, never the data).
    bool bus_cycle(uint32_t byte_addr, bool is_write, uint16_t write_data = 0) {
        bool low_byte = (byte_addr & 1u) != 0;
        dut->cpu_address = (byte_addr >> 1) & 0x7FFFFFu;
        dut->cpu_r_w = is_write ? 0 : 1;
        dut->cpudata_in = is_write ? write_data : 0;
        if (low_byte) { dut->_cpu_uds = 1; dut->_cpu_lds = 0; }
        else          { dut->_cpu_uds = 0; dut->_cpu_lds = 1; }
        dut->_cpu_as = 0;
        bool dtack_seen = false;
        for (int i = 0; i < 4000 && !dtack_seen; ++i) {
            tick();
            if (dut->_cpu_dtack == 0) dtack_seen = true;
        }
        for (int i = 0; i < 16; ++i) tick();
        dut->_cpu_as = 1; dut->_cpu_uds = 1; dut->_cpu_lds = 1; dut->cpu_r_w = 1;
        for (int i = 0; i < 8; ++i) tick();
        return dtack_seen;
    }

    bool write_byte(uint32_t byte_addr, uint8_t value) {
        bool low = (byte_addr & 1u) != 0;
        uint16_t wd = low ? (uint16_t)value : (uint16_t)((uint16_t)value << 8);
        return bus_cycle(byte_addr, true, wd);
    }
    uint8_t read_byte(uint32_t byte_addr) {
        bus_cycle(byte_addr, false, 0);
        bool low = (byte_addr & 1u) != 0;
        uint16_t v = dut->cpu_data;
        return low ? (uint8_t)(v & 0xFF) : (uint8_t)((v >> 8) & 0xFF);
    }

    // --- CIAB PRB/DDRB (the write-side join, minimig.v:660) ---
    void ciab_ddrb_all_out() { write_byte(0xBFD300, 0xFF); }
    void ciab_prb(uint8_t value) { pb_state = value; write_byte(0xBFD100, value); }
    uint8_t ciab_prb_read() { return read_byte(0xBFD100); }

    void deselect_all() { ciab_prb((uint8_t)(pb_state | 0x78)); }      // PB6..PB3 = 1
    void select0()      { ciab_prb((uint8_t)((pb_state | 0x78) & ~0x08)); } // _sel0 asserted
    void set_direc_bit(bool outward) {
        pb_state = outward ? (uint8_t)(pb_state | 0x02) : (uint8_t)(pb_state & ~0x02);
    }
    // One _step pulse (mirrors hrm/ch8-drive-sense-lines' do_step(): low
    // long enough for _step_del to catch it, then high -- the rising edge
    // paula_floppy.v:673 gates on), plus a generous clk7_en settle margin
    // so step_ena_cnt (paula_floppy.v:594-603, 256-tick gate) is always
    // re-saturated before any later step this file issues.
    void do_step(bool outward) {
        set_direc_bit(outward);
        ciab_prb((uint8_t)(pb_state & ~0x01)); // step low
        clk7_ticks(8);
        ciab_prb((uint8_t)(pb_state | 0x01));  // step high -- rising edge
        clk7_ticks(300);
    }
    // HOME-ZERO: repeated outward (refused-at-zero) steps, removing any
    // dependence on dsktrack[0]'s exact value entering a check (see the
    // file header's stimulus-source note about the DDRB-write transient).
    void home_zero() { for (int i = 0; i < 5; ++i) do_step(/*outward=*/true); }

    // --- SD-card mount/unmount (top-level pins, minimig.v:544-545) ---
    // disk_present's own always-block (paula_floppy.v:959-966) is NOT
    // clk7_en-gated, so a handful of raw ticks is enough.
    void mount(int n, uint32_t size) {
        dut->sdc_img_mounted = (uint8_t)(1u << n);
        dut->sdc_img_size = size;
        for (int i = 0; i < 8; ++i) tick();
        dut->sdc_img_mounted = 0;
    }
    void unmount(int n) { mount(n, 0); }
    void set_wrprot(bool on) {
        dut->floppy_config = on ? (uint8_t)(dut->floppy_config | 0x02)
                                 : (uint8_t)(dut->floppy_config & ~0x02);
        dut->eval();
    }

    // --- Observation (public-flat-rw, minimig.v:632/660 internal nets) ---
    // CIAA PRA read via a real CPU bus access at $BFE001; nibble bits
    // [3:0] = (PA5,PA4,PA3,PA2), matching Table 8-5's own column order.
    uint8_t pa_nibble() {
        uint8_t v = read_byte(0xBFE001);
        uint8_t pa5 = (v >> 5) & 1, pa4 = (v >> 4) & 1, pa3 = (v >> 3) & 1, pa2 = (v >> 2) & 1;
        return (uint8_t)((pa5 << 3) | (pa4 << 2) | (pa3 << 1) | pa2);
    }
    PBJoin pb_join() {
        dut->eval();
        return { (int)dut->rootp->minimig->_motor, (int)dut->rootp->minimig->_sel3,
                 (int)dut->rootp->minimig->_sel2,  (int)dut->rootp->minimig->_sel1,
                 (int)dut->rootp->minimig->_sel0,  (int)dut->rootp->minimig->side,
                 (int)dut->rootp->minimig->direc,  (int)dut->rootp->minimig->_step };
    }
};

// field_index: 0=motor(PB7) 1=sel3(PB6) 2=sel2(PB5) 3=sel1(PB4) 4=sel0(PB3)
//              5=side(PB2)  6=direc(PB1) 7=step(PB0)
bool pb_join_matches(const PBJoin& j, int only_low_field) {
    int vals[8] = { j.motor, j.sel3, j.sel2, j.sel1, j.sel0, j.side, j.direc, j.step };
    for (int i = 0; i < 8; ++i) {
        int expected = (i == only_low_field) ? 0 : 1;
        if (vals[i] != expected) return false;
    }
    return true;
}

// One CIABPRB bit -> one named join wire. bit_index is the real PB bit
// (7..0); field_index indexes pb_join()'s vals[] the same way (0..7, PB7
// first). `mut_wrong_bit`: clear a DIFFERENT, neutral bit instead of the
// target -- catches exactly a severed/swapped/wrong-bit join, per the build
// brief's own definition of what this package must catch.
bool check_pb_bit(Harness& h, int bit_index, int field_index, bool mut_wrong_bit) {
    h.ciab_prb(0xFF); // baseline: all eight deasserted
    int neutral_other = (bit_index == 7) ? 6 : 7;
    int target = mut_wrong_bit ? neutral_other : bit_index;
    h.ciab_prb((uint8_t)(0xFF & ~(1u << target)));
    PBJoin j = h.pb_join();
    bool ok = pb_join_matches(j, field_index);
    h.ciab_prb(0xFF);
    return ok;
}

// PA5 / _ready -- opus:8-cia.c6, sol:8-cia.c5. Fully deselected (every PA
// bit masked HIGH, C17-style) vs selected with track/wrprot/change all held
// at their OWN deasserted state -- the only bit expected to move is PA5.
bool check_pa5(Harness& h, bool mut_no_select) {
    h.mount(0, 1000);
    h.deselect_all();
    uint8_t before = h.pa_nibble();
    if (!mut_no_select) h.select0();
    h.home_zero();
    h.do_step(false); // away from zero; also the qualifying step for a stable baseline
    uint8_t after = h.pa_nibble();
    return before == 0xF && after == 0x7;
}

// PA4 / _track0 -- opus:8-cia.c11, sol:8-cia.c9.
bool check_pa4(Harness& h, bool mut_no_return) {
    h.mount(0, 1000);
    h.deselect_all();
    h.select0();
    h.home_zero();
    h.do_step(false); // away from zero -> neutral reference
    uint8_t neutral = h.pa_nibble();
    if (!mut_no_return) h.do_step(true); // back to zero
    uint8_t after = h.pa_nibble();
    return neutral == 0x7 && after == 0x3;
}

// PA3 / _wprot -- opus:8-cia.c16, sol:8-cia.c16.
bool check_pa3(Harness& h, bool mut_no_wrprot) {
    h.mount(0, 1000);
    h.deselect_all();
    h.select0();
    h.home_zero();
    h.do_step(false); // away from zero -> neutral reference
    uint8_t neutral = h.pa_nibble();
    if (!mut_no_wrprot) h.set_wrprot(true);
    uint8_t after = h.pa_nibble();
    h.set_wrprot(false);
    return neutral == 0x7 && after == 0x5;
}

// PA2 / _change -- opus:8-cia.c18, sol:8-cia.c17.
bool check_pa2(Harness& h, bool mut_no_unmount) {
    h.mount(0, 1000);
    h.deselect_all();
    h.select0();
    h.home_zero();
    h.do_step(false); // away from zero + qualifying step -> stable baseline
    uint8_t neutral = h.pa_nibble();
    if (!mut_no_unmount) h.unmount(0);
    uint8_t after = h.pa_nibble();
    return neutral == 0x7 && after == 0x6;
}

bool fixture_live_pb(Harness& h, bool mut_repeat) {
    h.ciab_prb(0xA5);
    uint8_t r1 = h.ciab_prb_read();
    h.ciab_prb(mut_repeat ? 0xA5 : 0x5A);
    uint8_t r2 = h.ciab_prb_read();
    return r1 == 0xA5 && r2 == 0x5A && r1 != r2;
}

bool fixture_live_pa(Harness& h, bool mut_no_select) {
    h.mount(0, 1000);
    h.deselect_all();
    uint8_t before = h.pa_nibble();
    if (!mut_no_select) h.select0();
    h.home_zero();
    h.do_step(false);
    uint8_t after = h.pa_nibble();
    return before == 0xF && after == 0x7 && before != after;
}

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;
    Harness h;

    // ========================================================================
    // CONTROL-RESET-RELEASES (harness precondition, no claim id -- same
    // shape as hrm/ch7-overlay-source's own). Without this, ciaa/ciab/
    // paula_floppy's shared `reset` (minimig.v:427, sys_reset|~_cpu_reset_in)
    // never deasserts and nothing below would be measuring live RTL.
    // ========================================================================
    {
        char tickbuf[24];
        std::snprintf(tickbuf, sizeof tickbuf, "%lld", h.reset_release_tick);
        checks.expect(h.reset_released,
            std::string("CONTROL-RESET-RELEASES: _cpu_reset rose within a 3,000,000-tick "
                        "budget (measured tick=") + tickbuf + ", or -1 if exhausted). No "
                        "manual page (harness precondition).");
    }
    if (!h.reset_released) {
        std::cout << "\nHALT-CANDIDATE: reset never released; no claim below would be "
                     "measuring anything but a permanently-reset chip, so none are run.\n";
        std::cout << checks.passed << " passed, " << checks.failed << " failed\n";
        return checks.failed == 0 ? 0 : 1;
    }

    // CIAB DDRB=0xFF once: all eight PB bits become genuine outputs (the
    // 8520 pull-up only applies to bits still configured as inputs). Every
    // PB write below assumes this.
    h.ciab_ddrb_all_out();
    h.ciab_prb(0xFF);

    // ========================================================================
    // FIXTURE-LIVE-PB -- no claim id: the CPU write/read path to CIAB PRB
    // itself (independent of paula_floppy) delivers two distinct, known,
    // correct bytes before any PB-BINDING claim below is trusted.
    // ========================================================================
    {
        bool mut = MUT_FIXPB;
        checks.expect(fixture_live_pb(h, mut),
            "FIXTURE-LIVE-PB: two distinct, known bytes (0xA5, 0x5A) written to CIABPRB "
            "($BFD100) through a real CPU bus write both arrive and read back correctly "
            "and distinctly through a real CPU bus read of the same address, before any "
            "PB-BINDING claim below is trusted [no page -- infrastructure]");
    }

    // ========================================================================
    // FIXTURE-LIVE-PA -- no claim id: the paula_floppy -> minimig join ->
    // ciaa -> CPU-read path (never exercised by any sibling package) is
    // live, not stuck, before any PA-BINDING claim below is trusted.
    // ========================================================================
    {
        bool mut = MUT_FIXPA;
        checks.expect(fixture_live_pa(h, mut),
            "FIXTURE-LIVE-PA: fully deselecting the floppy reads all four PA5-PA2 bits "
            "masked HIGH through a real CPU read of CIAAPRA ($BFE001); selecting drive 0 "
            "and driving it to a known, different state reads a distinct, known nibble -- "
            "the paula_floppy->minimig join->ciaa->CPU-read path is live before any "
            "PA-BINDING claim below is trusted [no page -- infrastructure]");
    }

    // ========================================================================
    // PA-BINDING claims (CIAAPRA, $BFE001, read side). Each check drives the
    // named drive signal through paula_floppy's own real behaviour (the
    // trusted hrm/ch8-drive-sense-lines recipe, carried through the CIAB
    // join), reads CIAAPRA through a real CPU bus access, and requires the
    // WHOLE (PA5,PA4,PA3,PA2) nibble to match -- not just the target bit --
    // so a swap between any two of the four positions is caught.
    // ========================================================================
    checks.expect(check_pa5(h, MUT_PA5),
        "opus:8-cia.c6 sol:8-cia.c5 PA5-BINDING: CIAAPRA's PA5 carries _ready (DSKRDY*), "
        "distinctly from PA4/PA3/PA2 -- fully deselected reads (PA5,PA4,PA3,PA2)=(1,1,1,1) "
        "(the C17-style deselected mask on all four), selecting drive 0 (floppy_drives=0, "
        "one drive configured, so paula_floppy.v:691-694's _ready reduces to _sel[0] alone) "
        "with track/wrprot/change all otherwise deasserted reads (0,1,1,1) -- PA5 alone "
        "moves. minimig.v:632 .porta_in({_fire1,_fire0,_ready,_track0,_wprot,_change}) with "
        "ciaa.v:254's unconditional porta_in2[7:2] passthrough [p.244 \"PA5 DSKRDY* Disk "
        "ready (active low)\"]");

    checks.expect(check_pa4(h, MUT_PA4),
        "opus:8-cia.c11 sol:8-cia.c9 PA4-BINDING: CIAAPRA's PA4 carries _track0 "
        "(DSKTRACK0*), distinctly from PA5/PA3/PA2 -- from the same selected/neutral "
        "reference (0,1,1,1) above, stepping the head back to track zero "
        "(paula_floppy.v:666 _track0=&(_selx|_dsktrack0), :683 _dsktrack0=~(dsktrack==0)) "
        "reads (0,0,1,1) -- PA4 alone additionally moves [p.244 \"PA4 DSKTRACK0* Track zero "
        "detect. The drive will pull this line low when the disk heads are positioned over "
        "track zero.\"]");

    checks.expect(check_pa3(h, MUT_PA3),
        "opus:8-cia.c16 sol:8-cia.c16 PA3-BINDING: CIAAPRA's PA3 carries _wprot (DSKPROT*), "
        "distinctly from PA5/PA4/PA2 -- from the same (0,1,1,1) reference, asserting "
        "floppy_wrprot (paula_floppy.v:664 _wprot=&(_sel|disk_writable), :969 "
        "disk_writable=floppy_wrprot?0:0xF) reads (0,1,0,1) -- PA3 alone additionally moves "
        "[p.244 \"PA3 DSKPROT* Disk is write protected (active low).\"]");

    checks.expect(check_pa2(h, MUT_PA2),
        "opus:8-cia.c18 sol:8-cia.c17 PA2-BINDING: CIAAPRA's PA2 carries _change "
        "(DSKCHANGE*), distinctly from PA5/PA4/PA3 -- from the same (0,1,1,1) reference "
        "(a mount+step-established stable baseline), removing the image "
        "(paula_floppy.v:610 _disk_change latch, :662 _change=&(_sel|_disk_change), :965 "
        "disk_present<=|sdc_image_size) reads (0,1,1,0) -- PA2 alone additionally moves "
        "[p.244 \"PA2 DSKCHANGE* Disk has been removed from the drive. The signal goes low "
        "whenever a disk is removed.\"]");

    // ========================================================================
    // PB-BINDING claims (CIABPRB, $BFD100, write side). Each check writes
    // one known byte through a real CPU bus write and observes all eight
    // named join wires (minimig.v:660) directly -- the target wire low, the
    // other seven high -- so a swap between any two of the eight positions
    // is caught.
    // ========================================================================
    checks.expect(check_pb_bit(h, 7, 0, MUT_PB7),
        "opus:8-cia.c21 sol:8-cia.c20 PB7-BINDING: CIABPRB's PB7 carries _motor "
        "(DSKMOTOR*) at the minimig.v:660 join, distinctly from the other seven named "
        "wires -- clearing only PB7 through a real CPU write reads _motor low and "
        "_sel3/_sel2/_sel1/_sel0/side/direc/_step all high [p.244 \"PB7 DSKMOTOR* ... "
        "Disk motor control\"]");
    checks.expect(check_pb_bit(h, 6, 1, MUT_PB6),
        "opus:8-cia.c32 sol:8-cia.c30 PB6-BINDING: CIABPRB's PB6 carries _sel3 (DSKSEL3*), "
        "distinctly from the other seven named wires [p.245 \"PB6 DSKSEL3* ... Select drive "
        "3\"]");
    checks.expect(check_pb_bit(h, 5, 2, MUT_PB5),
        "opus:8-cia.c34 sol:8-cia.c31 PB5-BINDING: CIABPRB's PB5 carries _sel2 (DSKSEL2*), "
        "distinctly from the other seven named wires [p.245 \"PB5 DSKSEL2* ... Select drive "
        "2\"]");
    checks.expect(check_pb_bit(h, 4, 3, MUT_PB4),
        "opus:8-cia.c36 sol:8-cia.c32 PB4-BINDING: CIABPRB's PB4 carries _sel1 (DSKSEL1*), "
        "distinctly from the other seven named wires [p.245 \"PB4 DSKSEL1* ... Select drive "
        "1\"]");
    checks.expect(check_pb_bit(h, 3, 4, MUT_PB3),
        "opus:8-cia.c38 sol:8-cia.c33 PB3-BINDING: CIABPRB's PB3 carries _sel0 (DSKSEL0*), "
        "distinctly from the other seven named wires [p.245 \"PB3 DSKSEL0* ... Select drive "
        "0\"]");
    checks.expect(check_pb_bit(h, 2, 5, MUT_PB2),
        "opus:8-cia.c41 sol:8-cia.c35 PB2-BINDING: CIABPRB's PB2 carries side (DSKSIDE), "
        "distinctly from the other seven named wires [p.245 \"PB2 DSKSIDE ... which disk "
        "head to use\"]");
    checks.expect(check_pb_bit(h, 1, 6, MUT_PB1),
        "opus:8-cia.c45 sol:8-cia.c39 PB1-BINDING: CIABPRB's PB1 carries direc (DSKDIREC), "
        "distinctly from the other seven named wires [p.245 \"PB1 DSKDIREC ... direction to "
        "seek the heads\"]");
    checks.expect(check_pb_bit(h, 0, 7, MUT_PB0),
        "opus:8-cia.c49 sol:8-cia.c44 PB0-BINDING: CIABPRB's PB0 carries _step (DSKSTEP*), "
        "distinctly from the other seven named wires [p.245 \"PB0 DSKSTEP* ... Step the "
        "heads\"]");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION CONTROL-RESET-RELEASES stimulus red: MUT_RESET holds rst_ext asserted forever, so sys_reset (and _cpu_reset) never release within budget.
// MUTATION FIXTURE-LIVE-PB stimulus red: MUT_FIXPB drives word 2 identical to word 1 (0xA5 again) instead of its complement, so the two CIABPRB readings cannot be told apart.
// MUTATION FIXTURE-LIVE-PA stimulus red: MUT_FIXPA never actually selects drive 0, so the "selecting moves the nibble" half is never exercised.
// MUTATION opus:8-cia.c6 stimulus red: MUT_PA5 never selects drive 0, so PA5/_ready's own isolating action is skipped and the nibble never leaves the deselected (1,1,1,1) mask.
// MUTATION opus:8-cia.c11 stimulus red: MUT_PA4 never steps back to track zero, so PA4/_track0 stays deasserted and the "PA4 alone additionally moves" comparison fails.
// MUTATION opus:8-cia.c16 stimulus red: MUT_PA3 never asserts floppy_wrprot, so PA3/_wprot stays deasserted.
// MUTATION opus:8-cia.c18 stimulus red: MUT_PA2 never unmounts the image, so PA2/_change stays deasserted.
// MUTATION opus:8-cia.c21 stimulus red: MUT_PB7 clears a neutral bit (PB6) instead of PB7, so _motor never reads low and pb_join_matches sees the wrong wire move.
// MUTATION opus:8-cia.c32 stimulus red: MUT_PB6 clears PB0 instead of PB6.
// MUTATION opus:8-cia.c34 stimulus red: MUT_PB5 clears PB0 instead of PB5.
// MUTATION opus:8-cia.c36 stimulus red: MUT_PB4 clears PB0 instead of PB4.
// MUTATION opus:8-cia.c38 stimulus red: MUT_PB3 clears PB0 instead of PB3.
// MUTATION opus:8-cia.c41 stimulus red: MUT_PB2 clears PB0 instead of PB2.
// MUTATION opus:8-cia.c45 stimulus red: MUT_PB1 clears PB0 instead of PB1.
// MUTATION opus:8-cia.c49 stimulus red: MUT_PB0 clears PB6 instead of PB0.
