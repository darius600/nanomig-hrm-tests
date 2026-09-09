// Test purpose: Confirm the one per-channel DMA slot count the `agnus` port
//   list actually exposes: the number of memory-refresh cycles allocated per
//   horizontal line, appendix "System DMA Channels", printed p.194.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   p.194, quote "4 cycles for memory refresh" (chapter 6, System DMA
//   Channels section).
// Non-goals: the disk, audio, sprite and bitplane per-channel slot counts in
//   the same manual section - packages.md records those as correctly
//   discarded (no beam-position or bus-slot signal for those channels on the
//   `agnus` port list) and this package does not reopen that discard; the
//   refresh channel is the one exception, because `refresh` is a dedicated
//   output port. Also not this package's job: which register or address a
//   refresh cycle touches, or memory-controller-level refresh correctness -
//   the manual's claim is a per-line count, and that is all that is
//   asserted here.
//
// BOUNDARY: `agnus` (top-level chipset module), NOT `agnus_blitter`. This is
// the first package in the chapter to assert at this boundary; the ten-package
// agnus_blitter harness and its fixture traps do not apply here. No existing
// HRM-style package compiles `agnus` (checked: hrm/agnus_refresh/ predates
// this chapter's HRM claim pipeline entirely - its own header names a
// different test class, "RTL_REGRESSION", against "the Frozen FPGA-021
// specification", not a manual quotation, and its Makefile's TOP_MODULE and
// its testbench's Verilated class name do not even agree - not prior art for
// an HRM_CONFORMANCE package, and left untouched).
//
// CLAIMS (verbatim from work/2026-09-08_fpga-090_classified.jsonl, filtered on
// classify.package == "PKG-AGNUS-SLOTS"; both are classify.status == "testable",
// classify.observable == "yes, at `agnus`"):
//
//   id: 6-system-dma.c18 (reader: sol)
//     extract.page: 194
//     extract.quote: "4 cycles for memory refresh"
//     extract.restatement: "Each horizontal line reserves four cycles for
//       memory refresh."
//
//   id: ch6-sysdma.c17 (reader: opus)
//     extract.page: 194
//     extract.quote: "4 cycles for memory refresh"
//     extract.restatement: "4 cycles per horizontal line are allocated to
//       memory refresh."
//
// Both ids are the SAME proposition read independently by two readers (per
// packages.md row 15: "Two claims, one per reader") and are cited together,
// in full, on REFRESH-FOUR-CYCLES-PER-LINE below - there is no second,
// distinct proposition in this package to split them across.
//
// PROVENANCE / SPEC-BEFORE-RTL GATE - RECORDED, NOT HIDDEN (carried over from
// the previous commit, 18afde3, "PKG-AGNUS-SLOTS: specification from
// inventory quotations" - see hrm/agnus_slots's own history): this
// package's build brief (FPGA-090 Prompt 7, section D) itself quotes agnus.v
// line numbers as background handed to the building agent, and explicitly
// instructs reading agnus.v "before you write the counting loop." Before
// drafting even the first (spec-only) commit, the building agent had already
// read agnus.v, agnus_refresh.v, agnus_diskdma.v and agnus_beamcounter.v in
// full - to locate the agnus_top_dmacon Makefile named as the starting point,
// and to work out whether a precondition/control was even needed. That
// reading happened before the spec was drafted, not after this test was
// written, so it is a narrower variant of this chapter's already-recorded
// gate breach, not a repeat of the same failure mode (adjusting an oracle to
// match what the RTL does): the expected count on REFRESH-FOUR-CYCLES-PER-LINE
// is 4 because the manual says 4, fixed before agnus.v was opened, and
// nothing found while reading it changed that number.
//
// FIXTURE. Top-level `agnus`, driven directly - no CPU register writes are
// needed anywhere in this file. `refresh` (agnus.v:72) is driven by the
// priority mux at agnus.v:169-267: `if (dma_dsk) ... else if (dma_ref)
// ... refresh = 1; ...`. `dma_ref` comes from agnus_refresh.v (instantiated
// as ref1, agnus.v:319-322), an UNCONDITIONAL function of hpos alone (no
// dmacon gate, no enable input) that is 1'b1 at exactly hpos in {9, 13, 17,
// 21} and 0 everywhere else - the four slots the p.194 claim counts. Because
// disk DMA (agnus_diskdma.v) has strictly higher priority than refresh in
// that same mux (line 171 checked before line 183), and its own
// speed-gated case table claims hpos[8:1] in {0x4,0x6,0x8,0xA} - which, ANDed
// with the dma expression's own hpos[0] factor, is EXACTLY hpos in {9, 13,
// 17, 21} again - disk DMA can, in one specific NanoMig-only configuration,
// take these same four cycles away from refresh. That fact drives the
// PRECONDITION and CONTROL assertions below.
//
// `sol` (agnus.v:81, "start of video line") is agnus_beamcounter.v's `eol`
// output (`assign eol = vpos_inc;`, itself `if (hpos==2) vpos_inc<=1'b1;`,
// so `sol` pulses for exactly one tick per line, shortly after the previous
// line's hpos wraps to 0). `hpos` itself is built from an externally-driven
// `cck` (agnus.v:58 "colour clock enable, active whenever hpos[0] is high"):
// agnus_beamcounter.v's Verilator branch sets `hpos_0 = cck` combinationally
// and increments the upper bits on `clk7_en` when `cck` is set - the classic
// toggle-flop-plus-carry construction, so a `cck` that alternates 0/1/0/1...
// on every clk7_en tick makes `hpos` advance by exactly 1 per tick (this
// harness's `tick()`), matching the "true divide-by-2 of clk7_en" cck
// convention already established and validated by hrm/agnus_blitter_cycles
// and reused by every later agnus_blitter package in this chapter (see e.g.
// hrm/agnus_blitter_bltcon's header). Neither `hpos` nor `cck`'s
// relationship to it is asserted on here - only `refresh` and `sol`, both
// real output ports - `cck`'s toggle rate is fixture plumbing, checked by
// SOL-MARKS-A-STABLE-LINE-LENGTH below rather than assumed.
//
// The counting method (packages.md, row 15): run `agnus` and count `refresh`
// assertions between two `sol` pulses. Implemented as Harness::capture_line:
// starting right after one `sol` tick, run ticks, counting `refresh`-high
// ticks, until the next `sol` tick (exclusive) - one call, one line.
//
// CLAIM COVERAGE: both testable ids in classify.package=="PKG-AGNUS-SLOTS"
// (6-system-dma.c18, ch6-sysdma.c17) are cited in full on
// REFRESH-FOUR-CYCLES-PER-LINE, the only manual-sourced proposition in this
// package.
//
// MUTATION LOG (every entry below was actually applied - edit, rebuild under
// WSL/Verilator, run, revert, rebuild clean - by this agent on 09.09.2026;
// the PASS/FAIL counts quoted are what `make test` printed that run, not
// predictions):
//
//   MUTATION SOL-MARKS-A-STABLE-LINE-LENGTH, FIRST ATTEMPT (recorded, not
//     hidden, per the standing rule against reporting a result not actually
//     produced): `set_ntsc(true)` inserted between capturing l2 and l3 ->
//     all 4 STILL PASSED. Not a valid mutation: `ntsc` is only latched into
//     the `pal` register at `reset` (agnus_beamcounter.v:171
//     `if(reset) pal<=~ntsc;`), and horizontal line length (`htotal`) is
//     hardwired to `HTOTAL_VAL<<1` unless `varbeamen` is set via a
//     `BEAMCON0` register write (agnus_beamcounter.v:223,236) - a bare
//     `ntsc` toggle with no reset pulse and no register write cannot reach
//     `htotal` at all, so the mutation touched an input the assertion
//     provably cannot see. CORRECTED MUTATION: added `write_reg`/`set_ecs`
//     to the Harness and, between capturing l2 and l3, wrote BEAMCON0=0x0080
//     (varbeamen=1, with ecs held true for that one write) then
//     HTOTAL=0x0064 (htotal_reg=200, a much shorter line) -> FAIL,
//     l2.length_ticks (454) != l3.length_ticks (~201). Reverted (both
//     write_reg calls removed): all green again.
//   MUTATION REFRESH-FOUR-CYCLES-PER-LINE stimulus red: `set_disk_dmal(true)`
//     and `set_floppy_speed(true)` added to this assertion's own Harness
//     (matching the control's configuration) -> FAIL, refresh_count reads 0,
//     not 4 (disk DMA's higher-priority branch of the mux wins every one of
//     the four slots). Reverted: all green again.
//   MUTATION REFRESH-SLOTS-SURVIVE-ACTIVE-DISK-REQUESTS stimulus red:
//     `set_floppy_speed(true)` added (matching the control's speed setting,
//     disk_dmal already true) -> FAIL, refresh_count reads 0, not 4 (now the
//     disk-DMA slot table's `speed`-gated entries are actually live).
//     Reverted: all green again.
//   MUTATION HIGHER-PRIORITY-DISK-DMA-DISPLACES-REFRESH-AT-DOUBLE-SPEED
//     stimulus red: `set_floppy_speed(true)` changed to `false` (disk_dmal
//     stays true) -> FAIL, refresh_count reads 4, not the expected 0 (without
//     floppy_speed, disk's slot-table entries for these four hpos values are
//     never live, so disk_dmal alone cannot take the cycles). Reverted: all
//     green again.
//
// Every mutation is a STIMULUS change to this file's own Harness calls -
// expected values and oracle text were never touched, and nanomig/ was never
// touched. Every run's total was re-confirmed by reading the full PASS/FAIL
// listing, not just the target line; a final clean rebuild after the last
// revert printed "4 passed, 0 failed".

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus.h"

namespace {

struct LineResult {
    bool timed_out = false;
    int length_ticks = 0;              // ticks from this line's boundary to the next sol
    int refresh_count = 0;             // number of ticks with refresh==1 strictly inside the window
    std::vector<int> refresh_offsets;  // 1-based tick offsets within the line at which refresh was seen high
};

class Harness {
public:
    Harness() : dut(new Vagnus) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->cck = 0;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->hwr = 0;
        dut->lwr = 0;
        dut->data_in = 0;
        dut->address_in = 0;
        dut->audio_dmal = 0;
        dut->audio_dmas = 0;
        dut->disk_dmal = 0;
        dut->disk_dmas = 0;
        dut->bls = 0;
        dut->ntsc = 0;
        dut->a1k = 0;
        dut->ecs = 0;
        dut->aga = 0;
        dut->floppy_speed = 0;
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void set_disk_dmal(bool v) { dut->disk_dmal = v ? 1 : 0; }
    void set_floppy_speed(bool v) { dut->floppy_speed = v ? 1 : 0; }
    void set_ntsc(bool v) { dut->ntsc = v ? 1 : 0; }
    void set_ecs(bool v) { dut->ecs = v ? 1 : 0; }

    // One CPU register write (byte-style 9-bit register address, matching
    // agnus_beamcounter.v's own VPOSR/HTOTAL/BEAMCON0-style parameters).
    // Not needed by any of the four assertions themselves - only by the
    // SOL-MARKS-A-STABLE-LINE-LENGTH mutation below, which has to reach
    // `htotal` and none of this file's plain input ports can.
    void write_reg(uint16_t reg9, uint16_t data) {
        dut->aen = 1;
        dut->hwr = 1;
        dut->address_in = static_cast<uint16_t>(reg9 >> 1);
        dut->data_in = data;
        tick();
        dut->aen = 0;
        dut->hwr = 0;
        dut->address_in = 0;
        dut->data_in = 0;
    }

    // Runs until a `sol` pulse is observed (a line boundary). Called once,
    // up front, to synchronise the harness before measuring anything.
    bool sync_to_line_boundary(int max_ticks = 2000) {
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            if (last_sol) return true;
        }
        return false;
    }

    // Call right after a boundary (the previous tick was a `sol` tick, or
    // the previous capture_line() call just returned). Runs until the next
    // `sol` tick (exclusive), counting `refresh`-high ticks strictly in
    // between - one call, one horizontal line, per packages.md's own
    // counting method.
    LineResult capture_line(int max_ticks = 1000) {
        LineResult r;
        for (int i = 1; i <= max_ticks; ++i) {
            tick();
            if (last_sol) {
                r.length_ticks = i;
                return r;
            }
            if (last_refresh) {
                ++r.refresh_count;
                r.refresh_offsets.push_back(i);
            }
        }
        r.timed_out = true;
        return r;
    }

    bool last_sol = false;
    bool last_refresh = false;

private:
    Vagnus* dut;
    bool cck_phase = false;

    void tick() {
        dut->cck = cck_phase ? 1 : 0;
        cck_phase = !cck_phase;
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        last_sol = dut->sol;
        last_refresh = dut->refresh;
    }
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- SOL-MARKS-A-STABLE-LINE-LENGTH (fixture check, no page) ----------------
    {
        Harness h;
        h.sync_to_line_boundary();
        LineResult l1 = h.capture_line();
        LineResult l2 = h.capture_line();
        LineResult l3 = h.capture_line();
        checks.expect(
            !l1.timed_out && !l2.timed_out && !l3.timed_out &&
                l1.length_ticks == l2.length_ticks && l2.length_ticks == l3.length_ticks &&
                l1.length_ticks > 0,
            "[fixture check] SOL-MARKS-A-STABLE-LINE-LENGTH: three consecutive sol-to-sol "
            "windows all measure the identical clk7_en tick count (" +
                std::to_string(l1.length_ticks) +
                " ticks per line here) - confirms 'between two sol pulses' is a genuine, "
                "repeatable one-line window (agnus_beamcounter.v's `eol`/`htotal` mechanism, "
                "agnus.v:81,86-87 port comments) before any count taken inside it is trusted. "
                "No manual page: a precondition of the fixture itself, not a claim.");
    }

    // ---------------- REFRESH-FOUR-CYCLES-PER-LINE (p.194) ----------------
    {
        Harness h;  // standard config: disk_dmal=0 (idle), floppy_speed=0 (standard speed)
        h.sync_to_line_boundary();
        LineResult la = h.capture_line();
        LineResult lb = h.capture_line();
        checks.expect(
            !la.timed_out && !lb.timed_out && la.refresh_count == 4 && lb.refresh_count == 4 &&
                la.refresh_offsets == lb.refresh_offsets,
            "REFRESH-FOUR-CYCLES-PER-LINE: agnus asserts its `refresh` output exactly four "
            "times in every horizontal line, at the same four tick offsets every line, measured "
            "as the count of refresh-high ticks between two consecutive `sol` pulses - '4 "
            "cycles for memory refresh' (p.194). `refresh` is a dedicated output port "
            "(agnus.v:72), driven high only by the dma_ref branch of the priority mux "
            "(agnus.v:183-193), fed by agnus_refresh.v's unconditional 4-entry hpos case table "
            "(hpos 9, 13, 17, 21; agnus.v:319-322 instantiates it as ref1). "
            "6-system-dma.c18, ch6-sysdma.c17.");
    }

    // ---------------- REFRESH-SLOTS-SURVIVE-ACTIVE-DISK-REQUESTS (precondition, no page) ----------------
    {
        // agnus.v's priority mux gives disk DMA strictly higher priority than
        // refresh (line 171 `if (dma_dsk)` checked before line 183 `else if
        // (dma_ref)`): if dma_dsk were ever asserted at the same hpos as a
        // refresh slot, refresh would lose that cycle. Leaving disk_dmal at
        // its idle default, as REFRESH-FOUR-CYCLES-PER-LINE does, would never
        // exercise that race at all - the green above would rest on an
        // untested assumption. Here Paula's disk_dmal is held high for the
        // whole line (an active requester, not an idle one), with
        // floppy_speed left at its standard (non-doubled) value.
        Harness h;
        h.set_disk_dmal(true);
        h.sync_to_line_boundary();
        LineResult l = h.capture_line();
        checks.expect(
            !l.timed_out && l.refresh_count == 4,
            "REFRESH-SLOTS-SURVIVE-ACTIVE-DISK-REQUESTS [precondition]: with disk_dmal held "
            "high the entire line and floppy_speed at its standard value, `refresh` still "
            "asserts all four times - agnus_diskdma.v's case table only grants disk a slot at "
            "hpos[8:1]==4,6,8,0A (agnus_refresh.v's own slots) when `speed` is set; at "
            "floppy_speed=0 those dmaslot entries are 0 regardless of disk_dmal, so dma_dsk "
            "never coincides with dma_ref and the priority mux's disk-over-refresh ordering "
            "(agnus.v:171 vs :183) never actually triggers. Confirms the precondition behind "
            "REFRESH-FOUR-CYCLES-PER-LINE's green - that disk DMA does not contend for these "
            "slots in the configuration the p.194 claim is read against - actually holds under "
            "an active requester, rather than being true only because no one asked. No manual "
            "page: an RTL precondition, not a claim.");
    }

    // ---------------- HIGHER-PRIORITY-DISK-DMA-DISPLACES-REFRESH-AT-DOUBLE-SPEED (control, no page) ----------------
    {
        // Same disk_dmal=1 as the precondition above, but floppy_speed=1 - the
        // NanoMig-specific "higher transfer speed" option that reassigns the
        // refresh slots to disk. This is the control the build brief
        // requires: the IDENTICAL sol-to-sol counting method used by
        // REFRESH-FOUR-CYCLES-PER-LINE, on a configuration whose outcome (0,
        // not 4) is predicted from the RTL's own documented priority
        // ordering, not from the manual. If a red result on
        // REFRESH-FOUR-CYCLES-PER-LINE were ever attributed to a units or
        // off-by-one error in the counting fixture rather than to the RTL,
        // this control - measured the same way, in the same run - would have
        // to be wrong too, and it is not.
        Harness h;
        h.set_disk_dmal(true);
        h.set_floppy_speed(true);
        h.sync_to_line_boundary();
        LineResult l = h.capture_line();
        checks.expect(
            !l.timed_out && l.refresh_count == 0,
            "HIGHER-PRIORITY-DISK-DMA-DISPLACES-REFRESH-AT-DOUBLE-SPEED [control]: the same "
            "sol-to-sol counting method that reports 4 refresh cycles above reports 0 here, "
            "once floppy_speed=1 hands agnus_diskdma.v's own hpos[8:1]==4,6,8,0A slots (the "
            "same cycles agnus_refresh.v claims) to disk DMA and agnus.v's priority mux grants "
            "disk strict priority over refresh (agnus.v:171 checked before :183) - proving the "
            "fixture's count tracks real RTL behaviour in both directions and is not a constant "
            "produced regardless of what the DUT does. This is the basis a red "
            "REFRESH-FOUR-CYCLES-PER-LINE result would need to survive the units-error "
            "objection. No manual page: an RTL-grounded control, not a claim.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
