// Test purpose: Figure 7-4's NMI row (page 221): hardware priority 7, no
//   Exec software priority. Package ch7-nmi-level-seven, fixture `minimig`
//   -- the whole top-level chipset module, per the task sheet.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, page 221
//   (Figure 7-4's NMI row; the whole claims file is this one figure).
// Non-goals: anything outside the `minimig` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in the chapter's
//   claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// *** THIS PACKAGE COMPILES RTL THE SHIPPED CONFIGURATION DOES NOT. ***
//   The NMI path this package measures exists only inside minimig.v's own
//   `ifdef ENABLE_CART branch (minimig.v:739-767): with that define unset,
//   minimig.v:764 ties the int7 net that carries it to a constant 1'b0, and
//   no build anywhere else in nanomig-hrm-tests sets +define+ENABLE_CART --
//   this package's own Makefile is the only place in this tree that does.
//   A green result below is therefore evidence about this define-enabled
//   build of the RTL only; it is NOT evidence about NanoMig as it ships,
//   and this file's own PRECONDITION check exists specifically so that a
//   green PROP-1 cannot be quietly standing on a build where the define
//   silently failed to take effect (see PRECONDITION below).
//
// BOUNDARY: nanomig/src/minimig-aga/minimig.v (module `minimig`, the whole
//   top-level chipset, ports at minimig.v:149-282) -- built with -DDISABLE_IDE
//   (ide.v/gayle.v need a per-board Lattice DP16KD primitive that will not
//   Verilate standalone, same as the ch7-overlay-source sibling package)
//   and, uniquely to this package, -DENABLE_CART (this package's own
//   subject lives entirely behind that define). Excluded from the Verilog
//   source list, same as the sibling: ide.v, gayle.v, cpu_wrapper.v,
//   cpu_cache_new.v, amiga_clk.v -- the last three are the real CPU core and
//   the on-FPGA clock generator, one level above minimig.v in the real SoC
//   and not part of this module's own hierarchy; minimig.v takes
//   cpu_address/cpu_data/clk/clk7_en/c1/c3/cck/eclk as plain input ports,
//   driven by this harness instead of by amiga_clk.v/a real 68000.
//
// MECHANISM READ FROM THE RTL (step 3, after SPECIFICATION.md's own
//   commit): the "NMI"/freeze signal chain is
//     ciaa.v:187 (Action Replay freeze keycode 0x5f, gated by hrtmon_en ==
//       memory_config[6], latched into freeze_reg on a kbd_mouse_type==2
//       keystrobe) --output--> minimig.v:360/628 wire freeze
//     --> cart.v:57 input freeze -> cart.v:84 freeze_req (rising-edge
//       detect) -> cart.v:87 int7_req -> cart.v:94-99 output reg int7
//       (int7 <= 1 on int7_req, <= 0 only on a level-7 IACK cycle this
//       harness never drives, so once set it is sticky for the rest of the
//       run -- adequate for this package's own proposition, which is about
//       the level asserted, not about the CPU's own vector-fetch cycle)
//     --> minimig.v:758 .int7(int7) --> minimig.v:770
//       `assign _cpu_ipl = int7 ? 3'b000 : _iplx;` (active-low: level L
//       reads 3'b(7-L), so 3'b000 is 68000 interrupt level 7, 3'b111 is
//       "no interrupt" -- same encoding paula_intcontroller.v's own
//       casez priority encoder uses for _iplx, per an earlier stage's own
//       measurement cited in classify.evidence for these claims).
//   int7 and _iplx are internal nets of module `minimig` itself (not of a
//   sub-instance), declared minimig.v:405-406; reached in this file via
//   Verilator's --public-flat-rw as dut->rootp->minimig->int7 and
//   dut->rootp->minimig->_iplx (verified against the generated
//   obj_dir/Vminimig_minimig.h field names, not guessed).
//
// ORDER-OF-WORK, DISCLOSED: SPECIFICATION.md was written from
//   ch7-nmi-level-seven-spec-stage.jsonl's own 3 claims' quotations alone,
//   before any RTL or STEP3-NOTES-ch7-nmi-level-seven.md/
//   ch7-nmi-level-seven.jsonl reading, and committed before this
//   file was opened or written -- per BUILD-BRIEF.md and the task sheet's
//   own two-step order. The mechanism paragraph above, the port names, and
//   every stimulus value below were only added in this step-3 file.
//
// CLAIM COVERAGE (3 claims, all on page 221):
//   opus:7-int.c78        -> NMI-HARDWARE-PRIORITY-7 (hardware-priority
//                             half) and NMI-NON-MASKABLE (the row's own
//                             name); its OTHER half ("no Exec software
//                             priority") is DECLINED, see EXEC-SOFTWARE-
//                             PRIORITY-NOT-HARDWARE below -- not asserted,
//                             not silently dropped.
//   sol:7-fig7-4.c48       -> NMI-HARDWARE-PRIORITY-7 (twin) and
//                             NMI-NON-MASKABLE (its own crossread note:
//                             "NMI raises hardware level 7").
//   opus:7-fig7-4.c48      -> identification only ("NMI" names the
//                             condition Figure 7-4 also describes as
//                             "non-maskable interrupt"); folded into the
//                             wording of NMI-HARDWARE-PRIORITY-7 and
//                             NMI-NON-MASKABLE rather than scored on its
//                             own, per SPECIFICATION.md's own reasoning.
//   All 3 claims accounted for: 2 hardware assertions (one cited by all
//   3 claims between them), 1 declined half of 1 claim.
//
// PRECONDITION (not from any claim): a green
//   NMI-HARDWARE-PRIORITY-7 could otherwise mean either "the define-enabled
//   path was exercised" or "the define silently failed to apply and this
//   run just read a constant" -- the two look identical on _cpu_ipl alone.
//   ENABLE-CART-COMPILED-IN measures the *internal* int7 net directly:
//   under minimig.v's `else` branch (no ENABLE_CART) int7 is a permanent
//   `assign int7 = 1'b0;` tie and cannot rise no matter the stimulus; under
//   the `ifdef ENABLE_CART branch it is a register that this file's own
//   freeze stimulus can and does drive to 1. This checks a net whose
//   behaviour differs between the two branches, not the ENABLE_CART
//   preprocessor symbol itself (testing the symbol would only prove what
//   the compiler was told).
//
// NON-MASKABILITY (reproducing -- not adopting -- STEP3-NOTES-ch7-nmi-level-seven.md's own
//   measurement "(c)": INTENA is never written anywhere in this file, so
//   paula_intcontroller's own arbitrated request _iplx sits at 3'b111 (no
//   interrupt) for the whole run. NMI-NON-MASKABLE checks that _cpu_ipl
//   still reads 3'b000 (level 7) at that same instant regardless -- the
//   level the CPU sees did not come from Paula's INTENA-gated encoder at
//   all (minimig.v:770's other mux input), which is the concrete meaning of
//   the row's own name, "non-maskable interrupt".
//
// FIXTURE-DATA-PATH-LIVE / CONTROL: NMI-IDLE-BASELINE-NOT-LEVEL-7 samples
//   the identical port (_cpu_ipl), read the identical way, BEFORE the
//   freeze stimulus is applied, and requires it to read 3'b111 (no
//   interrupt), not 3'b000. Without this, a green
//   NMI-HARDWARE-PRIORITY-7 would not distinguish "the module correctly
//   raises level 7 for this condition" from "this measurement always reads
//   back 3'b000, regardless of what happened" -- the silently-dead-
//   measurement failure BUILD-BRIEF.md itself warns about.
//
// PRACTICALITY, MEASURED not assumed: the task sheet warned reset alone
//   takes over two million clock ticks before anything is observable
//   (minimig_syscontrol.v counts 4 start-of-frame pulses, each a full PAL
//   field, before releasing `reset`) and to HALT rather than quietly test a
//   smaller fixture if this turned out impractical. It did not: Verilating
//   this ~9.4 MB, 53-module design takes about 1 second (elaboration) plus
//   roughly 11 s of C++ compilation (WSL/Verilator 5.052, ccache cold),
//   and the actual reset-lift tick count, MEASURED (not assumed from the
//   task sheet's own wording): rst_out lifted after 2,271,837 raw clk
//   edges, in every run (baseline and both mutations below) -- this
//   file's own reset-wait loop bound (10,000,000 raw clk edges) is
//   nearly 4.4x that and was never approached. Wall-clock for the whole
//   build+run: about 25 s per variant.
//
// WHAT THIS FILE DOES NOT ASSERT: whether any OTHER interrupt source in
//   this module can ever reach _cpu_ipl==3'b000 by way of Paula's own
//   INTENA/INTREQ path is a different package's own claim
//   (sol:7-interrupts.c2, opus:7-int.c3, per sol:7-fig7-4.c48's own
//   classify.note) and is out of this package's boundary. This file also
//   never drives a real 68000 IACK cycle (int7_ack, cart.v:90) -- once
//   int7 rises it is observed staying high for the rest of each run, which
//   is sufficient for the level-7/non-maskability propositions above and
//   does not require reproducing the CPU's own vector-fetch bus cycle.
//
// RESULTS (WSL/Verilator 5.052, this session): baseline build (`make`,
//   no MUT) -- 4 passed, 0 failed. All four PASS; see the PASS/MEASURED/
//   RECORDED lines this binary itself prints for the exact values (idle
//   _cpu_ipl=0x7, int7 and _cpu_ipl both taking their post-freeze values
//   at raw tick 2272045, _iplx=0x7 at that same tick, correlated at that
//   same tick).
//
// CORRELATION CHECK: grading NMI-HARDWARE-PRIORITY-7 and
//   NMI-NON-MASKABLE on int7_seen_high/ipl_seen_level7, two
//   independently-latched "ever true" flags, would let a spurious
//   _cpu_ipl==level7 reading with no real int7 behind
//   it, followed later by int7 genuinely rising on a chip that maps it to
//   the wrong level, satisfy both accumulated flags without the two
//   ever actually having happened together. The existing MUT_NOFREEZE
//   mutation removes the stimulus event and the resulting response
//   together, so it never exercises that path either way. Both
//   assertions instead grade on correlated_seen (int7 and _cpu_ipl==level7
//   observed true in the SAME loop iteration -- see the loop below).
//   MUTATION DECORRELATE (see the #define and in-loop comment) fabricates
//   that exact false-positive sequence from the harness side and confirms
//   the check rejects it -- see its own run-log entry below.
//   Structurally, no *real* stimulus this fixture could apply produces
//   that sequence on this RTL: _cpu_ipl is `assign _cpu_ipl = int7 ?
//   3'b000 : _iplx;` (minimig.v:770), a continuous assignment, and
//   paula_intcontroller's own priority encoder never outputs level 7 on
//   any INTENA/INTREQ pattern (every casez branch yields 1-7 as its
//   *_ipl* value or the explicit all-zero/default 7, never 0 -- confirmed
//   against the RTL and independently asserted as ch7-interrupt-priority-
//   encoder's own P2, "no combination of this module's own sources ever
//   produces level 7"). So on this correct RTL, int7==1 and _cpu_ipl==
//   level7 are tied by construction, every eval(), regardless of how
//   int7's value arose -- which is exactly why MUTATION DECORRELATE has
//   to fabricate the divergence directly rather than reach it through a
//   chip input.
//
// RUN LOG -- STIMULUS MUTATIONS ACTUALLY BUILT, RUN AND REVERTED THIS
//   SESSION (each: `make MUT=<NAME>` -> read the PASS/FAIL lines -> confirm
//   only the intended assertion(s) changed colour -> `make` with no MUT ->
//   confirm the 4-passed/0-failed baseline was restored). One record below
//   per assertion, each record's own first line naming that assertion's own
//   `expect()` label as its second word, per tools/check_gates.py's own
//   parsing rule (label, then "stimulus", then the result, immediately
//   adjacent).
//
// MUTATION NMI-IDLE-BASELINE-NOT-LEVEL-7 stimulus red -- built with
//   `make MUT=MUT_PREFREEZE`, which applies the freeze stimulus BEFORE
//   sampling this assertion's own idle baseline instead of after -> 3
//   passed, 1 failed. Only this assertion turned red (measured: the idle
//   sample now reads 0x0, because the freeze stimulus had already fired by
//   the time it was taken); ENABLE-CART-COMPILED-IN, NMI-HARDWARE-
//   PRIORITY-7 and NMI-NON-MASKABLE all stayed green (freeze still
//   happens, merely earlier). This mutation isolates the control on its
//   own, moving only this one label and nothing else -- which is what
//   shows the control can fail independently of the other three. Rebuilt
//   with no flags: 4/0 restored.
// MUTATION ENABLE-CART-COMPILED-IN stimulus red -- built with
//   `make MUT=MUT_NOFREEZE`, which withholds the freeze stimulus entirely
//   (kbd_mouse_type/kbd_mouse_data/kms_level stay idle) -> 1 passed, 3
//   failed (measured: "int7 (internal) never reached 1"). NMI-IDLE-
//   BASELINE-NOT-LEVEL-7 stayed green, unaffected. This same edit also
//   turns NMI-HARDWARE-PRIORITY-7 and NMI-NON-MASKABLE red at the same
//   time: all three depend on the same net, int7, ever rising at all, so
//   one edit turning three assertions red is one piece of evidence, not
//   three -- see those two records for the identical caveat, restated
//   rather than assumed known from this one. Rebuilt with no flags: 4/0
//   restored.
// MUTATION NMI-HARDWARE-PRIORITY-7 stimulus red -- built with
//   `make MUT=MUT_NOFREEZE`, the same edit as ENABLE-CART-COMPILED-IN's own
//   record above (withholds the freeze stimulus entirely) -> 1 passed, 3
//   failed (measured: "_cpu_ipl never reached 0x0"). All three of
//   ENABLE-CART-COMPILED-IN, NMI-HARDWARE-PRIORITY-7 and NMI-NON-MASKABLE
//   depend on the same net, int7, ever rising at all, so this one edit
//   turning all three red is one piece of evidence, not three separate
//   ones. NMI-IDLE-BASELINE-NOT-LEVEL-7 stayed green, unaffected. Rebuilt
//   with no flags: 4/0 restored.
// MUTATION NMI-NON-MASKABLE stimulus red -- built with
//   `make MUT=MUT_NOFREEZE`, the same edit as the two records above
//   (withholds the freeze stimulus entirely) -> 1 passed, 3 failed
//   (measured: "_cpu_ipl never reached 0x0", so this assertion's own
//   _iplx-at-that-tick comparison never runs). All three of
//   ENABLE-CART-COMPILED-IN, NMI-HARDWARE-PRIORITY-7 and NMI-NON-MASKABLE
//   depend on the same net, int7, ever rising at all, so this one edit
//   turning all three red is one piece of evidence, not three separate
//   ones. NMI-IDLE-BASELINE-NOT-LEVEL-7 stayed green, unaffected. Rebuilt
//   with no flags: 4/0 restored.
//
// CORRELATION CHECK: NMI-HARDWARE-PRIORITY-7 and NMI-NON-MASKABLE grade on
// correlated_seen (int7 and _cpu_ipl==level7 observed true in the same
// loop iteration) rather than two independently-latched flags. The four
// MUT_NOFREEZE/MUT_PREFREEZE entries above still apply unchanged -- none
// of them touch this correlation logic, only the pre-existing flags it
// is built from. One new entry below, built, run and reverted this
// session:
// MUTATION NMI-HARDWARE-PRIORITY-7 stimulus red -- built with
//   `make MUT=MUT_DECORRELATE`, which fabricates the false-positive
//   sequence finding 6 described: at k==3 (raw tick 2272041, well before
//   int7's real rise), _cpu_ipl is force-written to 0x0 with int7 still 0
//   (a spurious level-7 reading with no real int7 behind it); from int7's
//   real rise onward (raw tick 2272045, confirmed by the still-correct
//   "int7 (internal) reached 1 at raw tick 2272045" MEASURED line),
//   _cpu_ipl is force-written back to 0x7 every cycle (a stand-in for a
//   chip that maps NMI to the wrong level) -> 2 passed, 2 failed
//   (measured: "int7==1 and _cpu_ipl==0x0 were never observed in the same
//   cycle"). NMI-IDLE-BASELINE-NOT-LEVEL-7 and ENABLE-CART-COMPILED-IN
//   both stayed green -- the old, uncorrelated int7_seen_high and
//   ipl_seen_level7 flags this mutation was built to fool are BOTH still
//   true under it (see the MEASURED int7/_cpu_ipl lines), confirming this
//   is exactly the false-positive path grading on the two independent
//   flags would have missed and the correlation check catches. One edit turns both correlation-based
//   assertions red together, for the one reason both now share -- not two
//   separate pieces of evidence. Reverted (MUT_DECORRELATE back to its
//   default 0), rebuilt with no flags: 4/0 restored.
// MUTATION NMI-NON-MASKABLE stimulus red -- same build, same edit, same
//   run as the NMI-HARDWARE-PRIORITY-7 entry immediately above (2 passed,
//   2 failed; correlated_seen is this assertion's own precondition too,
//   so it fails for the identical reason). Reverted, rebuilt: 4/0
//   restored.

#include <cstdio>
#include <cstdint>
#include <cstring>
#include <string>
#include <iostream>
#include <verilated.h>
#include "Vminimig.h"
// int7/_iplx are internal nets of module `minimig` itself. Verilator split
// this design's generated state into a per-module class (Vminimig_minimig)
// rather than one flat root struct, because of its size (53 modules) --
// Vminimig.h only forward-declares that class, so its full definition is
// included explicitly here (confirmed against obj_dir/Vminimig_minimig.h,
// not guessed).
#include "Vminimig___024root.h"
#include "Vminimig_minimig.h"

// ---------------------------------------------------------------------
// THE ASSERTION CALL FORM IS FIXED, AND IT IS `expect` (BUILD-BRIEF.md,
// verbatim class).
// ---------------------------------------------------------------------
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

// ---------------------------------------------------------------------
// MUT_NOFREEZE: withholds the freeze stimulus entirely (kbd_mouse_type,
// kbd_mouse_data, kms_level all stay at their idle/reset values). One
// edit, three assertions -- ENABLE-CART-COMPILED-IN, NMI-HARDWARE-
// PRIORITY-7 and NMI-NON-MASKABLE all depend on int7 ever rising at all,
// and this is the single stimulus fact all three share (same reasoning
// hrm/ch5-audio-dmacon-adkcon/test_paula.cpp's own MUT_CHIP entry gives
// for one edit covering two of its assertions).
#ifndef MUT_NOFREEZE
#define MUT_NOFREEZE 0
#endif

// MUT_PREFREEZE: applies the freeze stimulus BEFORE sampling the
// NMI-IDLE-BASELINE-NOT-LEVEL-7 control, instead of after. Leaves the
// control's own expected value (3'b111) untouched; only reorders the
// stimulus so that value is no longer what the port reads at sample time.
// Targets NMI-IDLE-BASELINE-NOT-LEVEL-7 alone -- the other three
// assertions still pass, since the freeze stimulus still happens, merely
// earlier.
#ifndef MUT_PREFREEZE
#define MUT_PREFREEZE 0
#endif

// MUT_DECORRELATE (see header "CORRELATION CHECK"): fabricates,
// from the harness side, the exact false-positive sequence described
// above -- a spurious _cpu_ipl==level7 reading
// with no real int7 behind it, followed later by int7 genuinely rising
// while _cpu_ipl is forced to NOT show level 7 at that same cycle (a
// stand-in for a chip that maps NMI to the wrong level). This writes
// directly to the top-level _cpu_ipl port -- a port this file already
// reads, not a new one; nothing under nanomig/ is touched. It is not a
// claim that real stimulus can produce this sequence on this RTL (see the
// header for why it structurally cannot here); it exists to prove the
// correlation logic itself, not the chip, rejects data that the old,
// independent-flags check would have accepted.
#ifndef MUT_DECORRELATE
#define MUT_DECORRELATE 0
#endif

namespace {

constexpr uint8_t IPL_NONE  = 0x7; // 3'b111, active-low encoding: no interrupt
constexpr uint8_t IPL_LEVEL7 = 0x0; // 3'b000, active-low encoding: level 7 (NMI)

std::string hex2(unsigned v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "0x%X", v & 0xFFu);
    return std::string(buf);
}

class Harness {
public:
    Harness() : dut(new Vminimig) {
        // CPU bus pins: held inactive throughout. This package never drives
        // a real m68k bus cycle (see header, "WHAT THIS FILE DOES NOT
        // ASSERT") -- memory_config/kbd_mouse_*/kms_level are plain top-
        // level input pins, not registers reached through this bus, so
        // none of the assertions below need it live.
        dut->cpu_address = 0;
        dut->cpudata_in = 0;
        dut->_cpu_as = 1;      // inactive (address strobe not asserted)
        dut->_cpu_uds = 1;
        dut->_cpu_lds = 1;
        dut->cpu_r_w = 1;
        dut->_cpu_reset_in = 1; // never asserted; reset driven via rst_ext only
        dut->nmi_addr = 0;
        dut->ramdata_in = 0;
        dut->chip48 = 0;

        dut->rst_ext = 1; // asserted for the initial warm-up below
        dut->rxd = 1;
        dut->cts = 1;
        dut->dsr = 1;
        dut->cd = 1;
        dut->ri = 1;
        dut->_joy1 = 0xffff;
        dut->_joy2 = 0xffff;
        dut->_joy3 = 0xffff;
        dut->_joy4 = 0xffff;
        dut->joya1 = 0;
        dut->joya2 = 0;
        dut->mouse_btn = 7;

        // The freeze stimulus proper: idle until apply_freeze() below.
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;

        // hrtmon_en (memory_config[6]) must be set for the whole run --
        // it is a configuration pin (like a DIP switch), not a triggered
        // stimulus event; ciaa.v:187 gates the freeze keycode on it.
        dut->memory_config = 0x40;

        // OCS scope: opus:7-int.c78's own chipset_basis scopes chapter 7's
        // body (Figure 7-4 included) to OCS, not ECS/AGA. chipset_config=0
        // gives ecs=0 (|chipset_config[4:3]==0), aga=0 (chipset_config[4]
        // ==0), a1k=0 (chipset_config[2]==0) -- plain OCS, per minimig.v's
        // own .ecs/.aga/.a1k wiring to AGNUS1/DENISE1.
        dut->chipset_config = 0;
        dut->floppy_config = 0;

        dut->sdc_img_mounted = 0;
        dut->sdc_img_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        dut->cpucfg = 0;

        // Clock generation (this fixture excludes amiga_clk.v; see header).
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->cck = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->eclk = 1;

        // Warm-up: hold rst_ext asserted for a generous number of raw
        // clk edges so every register has taken at least one clocked
        // reset edge before minimig_syscontrol's own 4-start-of-frame
        // countdown (rst_ext gates rst_cnt itself, minimig_syscontrol.v:33)
        // begins to run.
        for (int i = 0; i < 2000; ++i) tick();
        dut->rst_ext = 0;
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // One raw clk edge, plus this fixture's own clk7_en/clk7n_en/cck/c1/c3/
    // eclk generation -- the phase relationships amiga_clk.v would produce
    // (clk7_en once per 4 raw edges, cck once per 2 clk7_en pulses; c1/c3
    // the complementary halves of the clk7 period; eclk a 10-phase rotator,
    // 1/10th of clk7_en, matching the port's own "ECLK enable (1/10th of
    // CLK)" comment, minimig.v:187). No assertion in this file depends on
    // c1/c3/eclk's own exact phase -- they only feed the CPU-bus bridge and
    // the SRAM bridge (minimig.v:664-729) and the CIA timers, none of which
    // this file reads or writes -- so their generation only has to be live,
    // not cycle-accurate against the real hardware.
    void tick() {
        dut->eval();
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();

        clk7_phase = (clk7_phase + 1) % 4;
        dut->clk7_en = (clk7_phase == 0);
        dut->clk7n_en = (clk7_phase == 2);
        if (dut->clk7_en) {
            cck_phase = !cck_phase;
            dut->cck = cck_phase;
            eclk_shift = ((eclk_shift << 1) | ((eclk_shift >> 9) & 1)) & 0x3ffu;
            if (!eclk_shift) eclk_shift = 1;
            dut->eclk = eclk_shift;
        }
        dut->c3 = (clk7_phase >> 1) & 1;
        dut->c1 = !dut->c3;
        ++raw_ticks;
    }

    void ticks(long n) { for (long i = 0; i < n; ++i) tick(); }

    // Presses the Action Replay freeze key: kbd_mouse_type==2 ("keyboard"),
    // kbd_mouse_data==0x5f (the freeze keycode, ciaa.v:187), kms_level
    // toggled 0->1 to produce exactly one keystrobe (ciaa.v:168-174).
    //
    // MUT_NOFREEZE (see the header comment and the #define above): withholds
    // this stimulus entirely -- kbd_mouse_type/kbd_mouse_data/kms_level are
    // left at the idle values set in the constructor, so ciaa.v's own
    // keystrobe/freeze_reg logic never latches the freeze keycode, freeze
    // never rises, and nothing downstream of it (int7, _cpu_ipl) can move
    // either.
    void apply_freeze() {
#if !MUT_NOFREEZE
        dut->kbd_mouse_type = 2;
        dut->kbd_mouse_data = 0x5f;
        dut->kms_level = 1;
#endif
    }

    Vminimig* dut;
    long raw_ticks = 0;

private:
    int clk7_phase = 0;
    int cck_phase = 1;
    unsigned eclk_shift = 1;
};

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;
    Harness h;

    // Wait for minimig_syscontrol's own reset countdown to lift. Bounded
    // generously (see header, PRACTICALITY) -- if it is never lifted, that
    // is reported as a HALT, not silently retried with a smaller fixture.
    const long RESET_BOUND = 10'000'000;
    while (h.dut->rst_out && h.raw_ticks < RESET_BOUND) h.tick();
    bool reset_lifted = !h.dut->rst_out;
    checks.note("reset (rst_out) lifted after " + std::to_string(h.raw_ticks) +
                " raw clk edges (bound was " + std::to_string(RESET_BOUND) + ")");
    if (!reset_lifted) {
        checks.record("HALT: rst_out never lifted within " + std::to_string(RESET_BOUND) +
                       " raw clk edges -- fixture did not reach an observable state; "
                       "no further assertion in this file can mean anything and none "
                       "were attempted past this point.");
        std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
        return checks.failed == 0 ? 0 : 1;
    }

    h.ticks(200); // settle past the reset edge itself

#if MUT_PREFREEZE
    // MUTATION: apply the freeze stimulus before taking the control
    // sample below, instead of after.
    h.apply_freeze();
    h.ticks(50);
#endif

    // ---------------------------------------------------------------
    // CONTROL (methodology, not a manual claim -- see header):
    // NMI-IDLE-BASELINE-NOT-LEVEL-7
    // ---------------------------------------------------------------
    uint8_t idle_ipl = h.dut->_cpu_ipl;
    const std::string s_idle_ipl = hex2(idle_ipl);
    const std::string s_none = hex2(IPL_NONE);     // formatted once, before any expect() call
    const std::string s_level7 = hex2(IPL_LEVEL7); // formatted once, before any expect() call
    checks.note("_cpu_ipl before the freeze stimulus = " + s_idle_ipl);
    checks.expect(idle_ipl == IPL_NONE,
        "NMI-IDLE-BASELINE-NOT-LEVEL-7: before the freeze stimulus, _cpu_ipl reads "
        + s_none + " (no interrupt), not " + s_level7
        + " -- control for NMI-HARDWARE-PRIORITY-7's own measurement path (same "
          "port, same encoding, same run), required so a green there cannot mean "
          "\"this measurement always reads back level 7\".");

#if !MUT_PREFREEZE
    h.apply_freeze();
#endif

    // ---------------------------------------------------------------
    // Monitor for the stimulus taking effect. int7 (internal) and
    // _cpu_ipl (top-level port) are both sticky once set in this file's
    // own run (see header, "WHAT THIS FILE DOES NOT ASSERT"), so a
    // bounded window is enough.
    //
    // CORRELATION CHECK: int7_seen_high and ipl_seen_level7 latch
    // independently of each other -- either one alone would let a spurious
    // _cpu_ipl==level7 reading with no real int7 behind it, followed later
    // by int7 genuinely rising on a chip that maps it to the wrong level,
    // satisfy both accumulated flags without the two ever actually having
    // happened together. Kept below only for the MEASURED notes and for
    // ENABLE-CART-COMPILED-IN (legitimately about int7 alone, not its
    // correlation with _cpu_ipl). NMI-HARDWARE-PRIORITY-7 and NMI-NON-
    // MASKABLE are graded on correlated_seen instead: int7 and
    // _cpu_ipl==level7 read true in the SAME loop iteration.
    // ---------------------------------------------------------------
    bool int7_seen_high = false;
    bool ipl_seen_level7 = false;
    uint8_t iplx_at_first_level7 = 0xFF;
    long tick_int7_high = -1;
    long tick_ipl_level7 = -1;
    bool correlated_seen = false;
    uint8_t iplx_at_correlated = 0xFF;
    long tick_correlated = -1;
    for (int k = 0; k < 2000; ++k) {
        h.tick();
        bool int7_now = h.dut->rootp->minimig->int7 != 0;
#if MUT_DECORRELATE
        // See the #define above and header "CORRELATION CHECK" for what
        // this does and why.
        if (k == 3 && !int7_now) {
            h.dut->_cpu_ipl = IPL_LEVEL7; // fabricated: no real int7 behind this
        } else if (int7_now) {
            h.dut->_cpu_ipl = IPL_NONE;   // fabricated: "wrongly mapped" NMI
        }
#endif
        bool ipl_level7_now = (h.dut->_cpu_ipl == IPL_LEVEL7);
        if (int7_now && !int7_seen_high) {
            int7_seen_high = true;
            tick_int7_high = h.raw_ticks;
        }
        if (ipl_level7_now && !ipl_seen_level7) {
            ipl_seen_level7 = true;
            tick_ipl_level7 = h.raw_ticks;
            iplx_at_first_level7 = h.dut->rootp->minimig->_iplx;
        }
        if (int7_now && ipl_level7_now && !correlated_seen) {
            correlated_seen = true;
            tick_correlated = h.raw_ticks;
            iplx_at_correlated = h.dut->rootp->minimig->_iplx;
        }
    }
    const std::string s_int7_note = int7_seen_high
        ? ("reached 1 at raw tick " + std::to_string(tick_int7_high))
        : std::string("never reached 1");
    checks.note("int7 (internal) " + s_int7_note);
    const std::string s_iplx_at_first = hex2(iplx_at_first_level7);
    const std::string s_ipl_note = ipl_seen_level7
        ? ("reached " + s_level7 + " at raw tick " + std::to_string(tick_ipl_level7)
           + ", _iplx at that same tick = " + s_iplx_at_first)
        : ("never reached " + s_level7);
    checks.note("_cpu_ipl " + s_ipl_note);
    const std::string s_correlated_note = correlated_seen
        ? ("int7==1 and _cpu_ipl==" + s_level7 + " observed in the same cycle at raw "
           "tick " + std::to_string(tick_correlated) + ", _iplx at that same tick = "
           + hex2(iplx_at_correlated))
        : ("int7==1 and _cpu_ipl==" + s_level7 + " were never observed in the same cycle");
    checks.note("correlation (same-cycle int7 and _cpu_ipl==level7): " + s_correlated_note);

    // ---------------------------------------------------------------
    // PRECONDITION (step-3 addition, see header): ENABLE-CART-COMPILED-IN
    // ---------------------------------------------------------------
    checks.expect(int7_seen_high,
        "ENABLE-CART-COMPILED-IN: minimig.v's internal int7 net (not a top-level "
        "port; reached via --public-flat-rw) rises to 1 in response to the freeze "
        "stimulus -- proves this build actually took the `ifdef ENABLE_CART branch "
        "(minimig.v:739-767, cart.v's freeze/int7 logic wired in) rather than the "
        "`else` branch that permanently ties int7 to 1'b0 (minimig.v:764). Measures "
        "a net whose behaviour differs between the two branches, not the "
        "ENABLE_CART preprocessor symbol itself.");

    // ---------------------------------------------------------------
    // PROP-1: NMI-HARDWARE-PRIORITY-7
    // ---------------------------------------------------------------
    checks.expect(correlated_seen,
        "NMI-HARDWARE-PRIORITY-7 (opus:7-int.c78, sol:7-fig7-4.c48, "
        "opus:7-fig7-4.c48, p.221): after the stimulus Figure 7-4's NMI row "
        "describes (\"non-maskable interrupt\"), int7 (internal) and _cpu_ipl=="
        + s_level7 + " -- 68000 interrupt level 7, the row's own printed hardware "
          "priority -- are observed in the SAME cycle, not merely each observed "
          "at some point in the run independently of the other.");

    // ---------------------------------------------------------------
    // NMI-NON-MASKABLE (step-3 addition, see header)
    // ---------------------------------------------------------------
    bool nonmaskable_ok = correlated_seen && iplx_at_correlated == IPL_NONE;
    checks.expect(nonmaskable_ok,
        "NMI-NON-MASKABLE (opus:7-int.c78, sol:7-fig7-4.c48, p.221; step-3 "
        "addition reproducing STEP3-NOTES-ch7-nmi-level-seven.md's own "
        "measurement, not adopting it): at the same cycle int7 and _cpu_ipl are "
        "first correlated at level " + s_level7 + " (NMI-HARDWARE-PRIORITY-7), "
          "INTENA has never been written in this run and paula's own arbitrated "
          "request _iplx still reads " + s_none
        + " (no interrupt pending there) -- the level-7 request the CPU sees did "
          "not come from Paula's INTENA-gated encoder, matching the row's own "
          "name.");

    // ---------------------------------------------------------------
    // DECLINED: the non-hardware half of opus:7-int.c78
    // ---------------------------------------------------------------
    checks.record(
        "EXEC-SOFTWARE-PRIORITY-NOT-HARDWARE: (axis: opus:7-int.c78) (p.221) also states "
        "the row's Exec-software-priority cell reads \"--\" (no Exec software "
        "priority for NMI). Declined, not asserted: per the claim's own "
        "classify.split_note (\"exec ordering and not-hardware\"), Exec's "
        "interrupt-server ordering is an AmigaOS software-layer property with no "
        "port, register, or timing behaviour in this chip RTL to check it "
        "against, with +define+ENABLE_CART or without it.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
