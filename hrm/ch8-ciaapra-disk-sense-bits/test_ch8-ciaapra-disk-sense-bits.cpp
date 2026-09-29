// Test purpose: assert what printed p.244, Table 8-5's four PA rows and its
//   own "Bits labeled PA are input bits in CIAAPRA ($BFE001)" sentence claim
//   about CIAAPRA ($BFE001, ciaa's PRA register, rs=4'h0) at the `ciaa`
//   module boundary, compiled alone with its CIA submodules -- that PA5
//   (DSKRDY*), PA4 (DSKTRACK0*), PA3 (DSKPROT*) and PA2 (DSKCHANGE*) are
//   genuine input bits: each reflects its own externally-driven pin,
//   distinctly from the others, and no write through either the direction
//   register (DDRA) or the data register (PRA) itself ever moves what a
//   CIAAPRA read reports at that position. Per SPECIFICATION.md (committed
//   140d670, before this file was opened for the RTL): `ciaa` alone carries
//   no signal named DSKRDY*, DSKTRACK0*, DSKPROT* or DSKCHANGE* -- only a
//   generic `porta_in[7:2]`. The binding of a bit POSITION to one of those
//   NAMES is a one-line structural fact at minimig.v:622, one level above
//   this fixture's boundary, and is not re-derived or exercised here; nor is
//   what a low level physically means for a drive (paula_floppy.v, further
//   downstream), nor DSKCHANGE*'s retention rule (needs a latch this fixture
//   is measured not to have). What is graded is exactly the one proposition
//   SPECIFICATION.md assigns: PROP-INPUT.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 244; committed specification 140d670 (SPECIFICATION.md, this
//   package).
// Requirement IDs: PROP-INPUT, per SPECIFICATION.md's claim inventory.
//   Claims file:
//   hrm-testpipeline/work/ch8-s2-build/ch8-ciaapra-disk-sense-bits.jsonl
//   (10 claims; 2 scored as PROP-INPUT, 8 declined -- see RECORDED lines in
//   main() and SPECIFICATION.md's "Claims not assigned an assertion").
// Non-goals / scope carve-outs, per the build task and SPECIFICATION.md:
//   - which drive signal (DSKRDY*, DSKTRACK0*, DSKPROT*, DSKCHANGE*) sits at
//     which PA bit position is a minimig.v:622 wiring fact, out of `ciaa`
//     alone's boundary -- declined, not graded.
//   - what a drive does with an asserted (LOW) level -- motor at speed,
//     track zero, write-protect tab, disk presence -- is a paula_floppy.v /
//     physical-drive fact, also out of boundary and not graded, even where a
//     claim's own evidence cites paula_floppy.v as corroboration.
//   - DSKCHANGE*'s retention/latch rule: not graded at this boundary at all
//     (ciaa does not latch this bit; where a latch exists it is at
//     paula_floppy, a different package's fixture).
//   - the motor-validity clause in DSKRDY*'s own paragraph and the
//     step-outward clause in DSKTRACK0*'s own paragraph are drive/software
//     behaviour, not gradable at `ciaa` either; already the subject of a
//     built, red sibling package (`hrm/ch8-drive-sense-lines`,
//     opus:8-cia.c7) that this package's claims do not name.
//   - PA6/PA7 (the fire-line bit positions) and PA1/PA0 are outside Table
//     8-5 and this package's claim set entirely; PA6/PA7 are still driven
//     below purely as an inert, neutral "different position" stimulus for
//     PROP-INPUT's crosstalk check -- that does not grade anything about
//     them.
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s2-build/ch8-ciaapra-disk-sense-bits.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// RTL read for step 2, after SPECIFICATION.md was committed (140d670): read
//   ciaa.v (388 lines) in full, plus its four instantiated submodules
//   (cia_int.v, cia_timera.v, cia_timerb.v, cia_timerd.v -- needed only for
//   Verilator to elaborate the top module, not opened/read for content).
//   Also read, before this file existed but after the specification was
//   committed and disclosed in it: hrm/ch8-ciaa-port-direction's and
//   hrm/ch8-ciabprb-disk-control-bits's full test .cpp (Checks class,
//   Harness bus-timing protocol, main()-layout and MUTATION-comment
//   convention -- the suite's own coding convention, reused verbatim below
//   as verified RTL-interface plumbing).
//   A placeholder build (this file as a two-line stub) was run first, purely
//   to inspect Verilator's own generated Vciaa___024root.h: `porta_in`
//   (declared `input [7:2]`, a non-zero-based range) is stored by Verilator
//   as a renumbered, LSB-aligned 6-bit CData -- C++ bit 0 is Verilog
//   porta_in[2], C++ bit 5 is porta_in[7] (confirmed both by the generated
//   `CData/*5:0*/` field comment and by the overWidthError guard masking
//   0xC0, i.e. only bits 0-5 are valid storage). `data_out` (`output
//   [7:0]`, already zero-based) is NOT renumbered -- reading it needs no
//   such translation. `Harness::drive_porta_in()` below does that one
//   translation in a single place so every other line in this file can
//   speak in the manual's own PA-bit numbering.
//
// RTL facts established by that reading (none of this shaped
//   SPECIFICATION.md, which was committed first):
//   - ciaa.v:136 (`pra = enable && rs==4'h0`), :138 (`ddra = enable &&
//     rs==4'h2`) -- selected by `rs`, never by name or bus address (`ciaa`
//     alone has no address decoder for $BFE001; that is gary.v's job, out of
//     this boundary).
//   - ciaa.v:222 `reg [7:2] porta_in2;` synchronises `porta_in` on
//     `clk7_en` (ciaa.v:227-230) -- one tick of latency, unconditional of
//     `reset`, `rs`, `wr` or anything written to PRA/DDRA.
//   - ciaa.v:236-238: a PRA write only ever latches `regporta[3:0] <=
//     {data_in[7:6], data_in[1:0]}` -- bits 5:2 of a PRA write are not
//     captured into any register at all; there is nothing for them to
//     latch.
//   - ciaa.v:245-247: a DDRA write latches `ddrporta[7:0] <= data_in[7:0]`
//     in full (all 8 bits stored), but ciaa.v:262's only consumer,
//     `porta_out[3:0] = {(~ddrporta[7:6] | regporta[3:2]),
//     (~ddrporta[1:0] | regporta[1:0])}`, reads only `ddrporta[7:6]` and
//     `ddrporta[1:0]` -- `ddrporta[5:2]` is written and stored but read by
//     nothing anywhere in the file.
//   - ciaa.v:253-254 (the PRA read mux): `pa_out[7:0] =
//     {porta_in2[7:2],porta_out[1:0]}` -- bits 7 down to 2 of a PRA read are
//     UNCONDITIONALLY the synchronised external input; `porta_out`,
//     `ddrporta` and `regporta` do not appear on the right-hand side for
//     those six bit positions at all. Consequence, confirmed by direct
//     measurement below: PA5-PA2 track `porta_in` no matter what is written
//     to DDRA or PRA at those positions, and never latch a written value --
//     "input bits" in the fullest sense the page's sentence can mean at
//     this boundary.
//
// MUTATION opus:8-cia.c2 stimulus red: PROP-INPUT's sweep (this assertion's
//   label also carries the leading token's twin, sol:8-cia.c1, which the
//   suite's mutation-label gate reads only the FIRST token of; the switch
//   below covers both, since they are the same graded assertion), at PA3,
//   redirects which physical porta_in line is driven low from PA3 itself to
//   the neutral other-position line (PA7), expected value (PA3 reads LOW)
//   left unchanged, guarded by mutating("opus:8-cia.c2") and read once from
//   the MUTATE environment variable at startup (MUTATE=opus:8-cia.c2:
//   opus:8-cia.c2 sol:8-cia.c1 PROP-INPUT FAIL -- PA3 read back HIGH, only
//   PA7 was actually driven low; 0 passed, 1 failed). (PROP-INPUT is the
//   only assertion below still graded via expect(); its mutation record is
//   therefore the only one kept.)
// MUTATION opus:8-cia.c2-immune-write stimulus red: bit_is_pure_input's own-
//   write clause holds a position's line low, leaves that position's own
//   direction bit clear and writes its own PRA bit HIGH anyway, expecting a
//   read still LOW (p.341: a clear DDR bit defines an input, and a PR read
//   returns the actual pin regardless of any write). At PA3, this mutation
//   releases every porta_in line -- PA3's included -- immediately before
//   that read, leaving the fixed expectation (PA3 reads LOW) untouched, so a
//   comparator that actually reads the pin must go red; guarded by
//   mutating("opus:8-cia.c2-immune-write") and read once from MUTATE at
//   startup (MUTATE=opus:8-cia.c2-immune-write: opus:8-cia.c2 sol:8-cia.c1
//   PROP-INPUT FAIL -- PA3 read back HIGH once its own line was released;
//   0 passed, 1 failed).
//
// REPAIR NOTE (2026-09-15): independent validation of this package's two
//   other checks. FIXTURE-LIVE (no uid) is a liveness measurement only --
//   converted expect() -> note(), per this suite's rule that a no-uid check
//   is never graded; its former MUTATION FIXTURE-LIVE record is removed
//   with it. CONTROL-PA5 (no uid) repeats a subset of PROP-INPUT's own
//   comparator on the same direct vector -- duplicate rather than
//   independent evidence -- also converted expect() -> note(), its former
//   MUTATION CONTROL-PA5 record removed with it. The suite's separate
//   hrm/ch8-cia-disk-signal-bindings package now grades the 8 named-signal
//   BINDING claims this package's own pre-existing declines named "one
//   level above this boundary"; those declines are re-pointed below to name
//   that package and the specific PAn-BINDING assertion carrying each
//   signal, instead of the boundary in general. No RTL, oracle or kept
//   assertion (PROP-INPUT) was touched.

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

namespace {

const char* g_mutate = nullptr;
inline bool mutating(const std::string& id) {
    return g_mutate != nullptr && id == g_mutate;
}

constexpr uint8_t RS_PRA = 0x0;
constexpr uint8_t RS_DDRA = 0x2;

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Bus-timing protocol for ciaa.v, reused from hrm/ch8-ciaa-port-direction's
// own Harness (the only other package in this suite that builds this exact
// fixture): regporta/ddrporta are posedge-clk, clk7_en-gated, so a write
// needs one tick to latch; porta_in2's synchroniser is the same shape, so a
// change to porta_in also needs one tick before a PRA read reflects it.
// pa_out is combinational and needs only eval().
class Harness {
public:
    Harness() : dut(new Vciaa) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->clk7n_en = 1;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        dut->tick = 0;
        dut->eclk = 0;
        dut->porta_in = 0;
        dut->portb_in = 0;
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        dut->hrtmon_en = 0;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void write_reg(uint8_t rs, uint8_t data) {
        dut->aen = 1;
        dut->wr = 1;
        dut->rd = 0;
        dut->rs = rs;
        dut->data_in = data;
        tick();
        dut->aen = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        tick();
    }

    uint8_t read_reg(uint8_t rs) {
        dut->aen = 1;
        dut->rd = 1;
        dut->wr = 0;
        dut->rs = rs;
        dut->eval();
        const uint8_t v = dut->data_out;
        dut->aen = 0;
        dut->rd = 0;
        dut->eval();
        return v;
    }

    // Drives `porta_in`, speaking in the manual's own PA-bit numbering:
    // pass a byte whose bit k (k in 2..7) is the desired value of PA-bit k
    // (porta_in[k]); bits 0 and 1 are ignored (porta_in has no such bits).
    // Verilator stores `input [7:2] porta_in` as a renumbered, LSB-aligned
    // 6-bit field (C++ bit 0 == Verilog bit 2, ..., C++ bit 5 == Verilog
    // bit 7 -- confirmed against the generated Vciaa___024root.h, see the
    // header comment above), so this is the one place that translation
    // happens; every call site below uses Verilog/manual bit numbers only.
    // One tick is needed afterwards for porta_in2's synchroniser to catch
    // up (ciaa.v:227-230) before a PRA read reflects the new value.
    void drive_porta_in(uint8_t verilog_bits_7to2) {
        uint8_t cpp_value = 0;
        for (int vbit = 2; vbit <= 7; ++vbit) {
            if ((verilog_bits_7to2 >> vbit) & 1u) cpp_value |= static_cast<uint8_t>(1u << (vbit - 2));
        }
        dut->porta_in = cpp_value;
        tick();
    }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    Vciaa* dut;
};

// ---------------------------------------------------------------------
// FIXTURE-LIVE: `ciaa` has never been measured by this package before.
// Two distinct, known 4-bit words across exactly the PA5-PA2 nibble this
// package claims must both arrive at a PRA read and read back correctly
// and distinctly before PROP-INPUT is graded.
// ---------------------------------------------------------------------
bool fixture_live(Harness& h, bool repeat_first_word_bug = false) {
    // Pattern A: PA5=0 PA4=1 PA3=0 PA2=1 (PA7/PA6 held high, inert).
    h.drive_porta_in(0b11010100);
    const uint8_t r1 = h.read_reg(RS_PRA) & 0x3C;  // bits 5:2 only.
    // Pattern B: the complement on PA5-PA2, PA5=1 PA4=0 PA3=1 PA2=0.
    h.drive_porta_in(repeat_first_word_bug ? uint8_t{0b11010100} : uint8_t{0b11101000});
    const uint8_t r2 = h.read_reg(RS_PRA) & 0x3C;
    return r1 == 0x14 && r2 == 0x28 && r1 != r2;
}

// ---------------------------------------------------------------------
// Shared per-bit comparator. PROP-INPUT sweeps this across PA5, PA4, PA3
// and PA2; CONTROL-PA5 calls the SAME function restricted to PA5 alone --
// the substitution/measurement-basis control needs the identical
// comparator, not a reimplementation of it.
//
// `bit` and `other_bit` are Verilog/manual PA-bit numbers (2..7).
// `wrong_bit_bug`, the stimulus mutation: instead of actually driving
// `bit` low, drives `other_bit` low -- the expectation below (that `bit`
// reads LOW) is left untouched, so a correctly-discriminating comparator
// must go red.
// ---------------------------------------------------------------------
bool bit_is_pure_input(Harness& h, int bit, int other_bit,
                        bool wrong_bit_bug = false,
                        bool release_before_read_bug = false) {
    const uint8_t bit_mask = static_cast<uint8_t>(1u << bit);
    const uint8_t other_mask = static_cast<uint8_t>(1u << other_bit);

    // (a) drive porta_in so ONLY `bit` (or, under the bug, `other_bit`
    // instead) is low; every other porta_in position (2..7) high.
    const int driven_low_vbit = wrong_bit_bug ? other_bit : bit;
    h.drive_porta_in(static_cast<uint8_t>(0xFC & ~(1u << driven_low_vbit)));
    const uint8_t r1 = h.read_reg(RS_PRA);
    const bool tracks_own_pin = ((r1 >> bit) & 1u) == 0u;  // fixed expectation: `bit` reads LOW.

    // (b) a DIFFERENT position's direction switched to "output" and
    // written -- must not move `bit`'s reading away from porta_in's truth.
    h.write_reg(RS_DDRA, other_mask);
    h.write_reg(RS_PRA, other_mask);
    const uint8_t r2 = h.read_reg(RS_PRA);
    const bool no_crosstalk = ((r2 >> bit) & 1u) == 0u;

    // (c) `bit`'s own direction left CLEAR -- p.341 defines a position with
    // a clear DDR bit as an input -- and `bit`'s own PRA position written
    // HIGH anyway: a PR read returns the actual pin state regardless of any
    // write (p.341), and `bit`'s line is still held low from (a), so the
    // write must not move it: still LOW.
    h.write_reg(RS_DDRA, 0x00);
    h.write_reg(RS_PRA, bit_mask);
    if (release_before_read_bug) h.drive_porta_in(0xFC);  // MUTATION stimulus: releases every line, `bit`'s included, just before the read this clause grades.
    const uint8_t r3 = h.read_reg(RS_PRA);
    const bool immune_to_own_write = ((r3 >> bit) & 1u) == 0u;

    return tracks_own_pin && no_crosstalk && immune_to_own_write;
}

// ---------------------------------------------------------------------
// PROP-INPUT: PA5, PA4, PA3 and PA2 of CIAAPRA are genuine input bits --
// graded across all four bit positions Table 8-5 names.
// ---------------------------------------------------------------------
bool prop_input_sweep(Harness& h, std::string& detail, int wrong_bit_bug_at = -1,
                       int release_bug_at = -1) {
    bool all_ok = true;
    const int bits[4] = {5, 4, 3, 2};
    for (int bit : bits) {
        const bool ok = bit_is_pure_input(h, bit, /*other_bit=*/7,
            bit == wrong_bit_bug_at, bit == release_bug_at);
        detail += "PA" + std::to_string(bit) + "=" + (ok ? "PASS " : "FAIL ");
        all_ok = all_ok && ok;
    }
    return all_ok;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    {
        Harness h;
        const bool live = fixture_live(h);
        checks.note(std::string("FIXTURE-LIVE: two distinct, known 4-bit words across PA5-PA2 "
            "(0b0101, 0b1010) arrive and read back ") + (live ? "correctly and distinctly" : "INCORRECTLY") +
            " at a CIAAPRA read -- liveness measurement only, not itself a page claim, not graded "
            "[no page -- infrastructure]");
    }

    {
        Harness h;
        std::string detail;
        const bool result = prop_input_sweep(h, detail,
            mutating("opus:8-cia.c2") ? 3 : -1,
            mutating("opus:8-cia.c2-immune-write") ? 3 : -1);
        checks.expect(result,
            "opus:8-cia.c2 sol:8-cia.c1 PROP-INPUT: PA5, PA4, PA3 and PA2 of "
            "CIAAPRA are genuine input bits -- graded across all four "
            "positions Table 8-5 names: each position's CIAAPRA read tracks "
            "its own externally-driven pin, a DIFFERENT position's DDRA/PRA "
            "write does not move it, and a write to that position's OWN "
            "PRA, with its own DDRA bit left clear, does not move it "
            "either. Not graded: which drive signal (DSKRDY* etc.) each "
            "position is named for -- that binding sits one level above "
            "this fixture's boundary [p.244 \"Bits labeled PA are input "
            "bits in CIAAPRA ($BFE001)\", Table 8-5 (PA5 DSKRDY*, PA4 "
            "DSKTRACK0*, PA3 DSKPROT*, PA2 DSKCHANGE*); p.341 \"If a DDR "
            "bit is set to a 0, the corresponding PR bit is defined as an "
            "input\" and \"you read the actual current state of the I/O "
            "pins ... regardless of whether you have set them to be "
            "inputs or outputs\"]");
        checks.note("PROP-INPUT per-bit result: " + detail);
    }

    { Harness h; const bool ok = bit_is_pure_input(h, /*bit=*/5, /*other_bit=*/6);
      checks.note(std::string(
        "CONTROL-PA5: the same bit_is_pure_input() comparator PROP-INPUT "
        "sweeps above, instantiated at PA5 alone -- ") + (ok ? "PASS" : "FAIL") +
        " -- repeats a subset of PROP-INPUT on the same direct vector, "
        "duplicate rather than independent evidence; measurement-basis "
        "control only, not itself a page claim, not graded [no page -- "
        "infrastructure control]"); }

    // ------------------------------------------------------------------
    // Declined claims. Each names a proposition this package does not
    // score, and the reason -- per SPECIFICATION.md's "Claims not assigned
    // an assertion". No uid below appears on a graded (expect()) line.
    // ------------------------------------------------------------------
    checks.record(
        "opus:8-cia.c6 (p.244, \"PA5 DSKRDY* ... Disk ready (active low). "
        "The drive will pull this line low when the motor is known to be "
        "rotating at full speed. This signal is only valid when the motor "
        "is ON...\"): names PA5's drive-signal binding, now graded in "
        "hrm/ch8-cia-disk-signal-bindings' PA5-BINDING assertion; the "
        "motor-validity clause is a separate, already-built sibling claim "
        "(hrm/ch8-drive-sense-lines, opus:8-cia.c7). PA5's bit-position/"
        "input-ness is still exercised as one of PROP-INPUT's four data "
        "points -- a different claim. Not graded here.");
    checks.record(
        "sol:8-cia.c5 (p.244, composed: PA5 position + DSKRDY* name + "
        "active-low polarity, twin of opus:8-cia.c6): same reason; now "
        "graded in hrm/ch8-cia-disk-signal-bindings' PA5-BINDING assertion. "
        "Not graded here.");
    checks.record(
        "opus:8-cia.c11 (p.244, \"PA4 DSKTRACK0* ... Track zero detect. The "
        "drive will pull this line low when the disk heads are positioned "
        "over track zero. Software must not attempt to step outwards...\"): "
        "names PA4's drive-signal binding, now graded in hrm/ch8-cia-disk-"
        "signal-bindings' PA4-BINDING assertion; the software/step-refusal "
        "clause is a separate, already-graded claim at paula_floppy "
        "(hrm/ch8-drive-sense-lines' own step-refusal assertion). PA4's "
        "bit-position/input-ness is still exercised as one of PROP-INPUT's "
        "four data points. Not graded here.");
    checks.record(
        "sol:8-cia.c9 (p.244, \"PA4 DSKTRACK0* ... Track zero detect.\", "
        "twin of opus:8-cia.c11): same reason; now graded in hrm/ch8-cia-"
        "disk-signal-bindings' PA4-BINDING assertion. Not graded here.");
    checks.record(
        "opus:8-cia.c16 (p.244, \"PA3 DSKPROT* Disk is write protected "
        "(active low).\"): names PA3's drive-signal binding and the "
        "active-low polarity meaning, now graded in hrm/ch8-cia-disk-"
        "signal-bindings' PA3-BINDING assertion. PA3's bit-position/"
        "input-ness is still exercised as one of PROP-INPUT's four data "
        "points. Not graded here.");
    checks.record(
        "sol:8-cia.c16 (p.244, composed: PA3 position + DSKPROT* name + "
        "active-low polarity, twin of opus:8-cia.c16): same reason; now "
        "graded in hrm/ch8-cia-disk-signal-bindings' PA3-BINDING assertion. "
        "Not graded here.");
    checks.record(
        "opus:8-cia.c18 (p.244, \"PA2 DSKCHANGE* Disk has been removed from "
        "the drive. The signal goes low whenever a disk is removed. It "
        "remains low until a disk is inserted AND a step pulse is "
        "received.\"): names PA2's drive-signal binding, now graded in "
        "hrm/ch8-cia-disk-signal-bindings' PA2-BINDING assertion; its own "
        "quotation also carries DSKCHANGE*'s retention sentence, which "
        "needs a latch -- `ciaa` is measured to hold none for this bit "
        "(transparent buffer, confirmed above by direct measurement: PA2 "
        "tracks porta_in unconditionally of any DDRA/PRA write), and the "
        "latch that exists is at paula_floppy, already graded there by "
        "hrm/ch8-drive-sense-lines' own removal and retention assertions. "
        "PA2's bit-position/input-ness is still exercised as one of "
        "PROP-INPUT's four data points. Not graded here.");
    checks.record(
        "sol:8-cia.c17 (p.244, \"PA2 DSKCHANGE* Disk has been removed from "
        "the drive.\", twin of opus:8-cia.c18, no retention sentence in "
        "this shorter quotation): now graded in hrm/ch8-cia-disk-signal-"
        "bindings' PA2-BINDING assertion, for the name-binding content. Not "
        "graded here.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
