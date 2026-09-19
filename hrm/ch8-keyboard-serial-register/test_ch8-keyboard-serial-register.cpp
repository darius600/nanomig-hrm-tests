// Test purpose: grade the 69 claims of
//   hrm-testpipeline/work/ch8-s3-build/ch8-keyboard-serial-register.jsonl at
//   the `ciaa` module boundary, compiled alone (plus cia_int/cia_timera/
//   cia_timerb/cia_timerd, needed only for Verilator to elaborate the top
//   module). Subject: p.251's "the keyboard is interfaced to the system via
//   the serial shift register on one of the 8520 CIA chips" and its
//   interrupt sentence, plus Appendix G's byte-rotation rule (pp.357-358),
//   its two worked examples ($35, $B5), and its two follow-on mentions on
//   pp.359 and 364. Every assertion below grades bit identity/ordering at a
//   whole-byte boundary; none grades a voltage, an edge, or a pulse width --
//   this boundary has no CNT and no SP port, and receives a byte on one
//   `keystrobe` latch, never bit by bit.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 251, 357, 358, 359, 364; committed specification
//   (SPECIFICATION.md, this package, commit 2357132).
// Requirement IDs: PROP-1 .. PROP-9, plus three untagged assertions
//   (WIDTH-8BIT, MAPPING-SAME-CLASS, FLAG-LAST-GENERAL) that back seven
//   partially-gradeable uids via `record()` rather than carrying those uids
//   themselves -- see SPECIFICATION.md's "Claim inventory" and
//   "Partially-gradeable claims" sections for the uid-by-uid accounting.
//   Claims file: hrm-testpipeline/work/ch8-s3-build/ch8-keyboard-serial-register.jsonl
//   (69 claims; 62 uids fully graded, 7 uids partially graded).
// Non-goals: anything outside the `ciaa` fixture this package builds, and
//   any claim outside the 69 it cites. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 69 ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s3-build/ch8-keyboard-serial-register.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// RTL read for step 2, after SPECIFICATION.md was committed (2357132): read
//   ciaa.v (388 lines) in full, plus cia_int.v (needed for PROP-2's interrupt
//   path) in full. cia_timera.v/cia_timerb.v/cia_timerd.v were not read
//   beyond confirming they exist and are instantiated -- Verilator needs them
//   to elaborate `ciaa`, but nothing in this package's assertions depends on
//   timer content. Also read, per the build brief's "read a sibling's
//   Makefile for the build pattern only after the spec is committed":
//   hrm/ch8-ciaa-port-direction/Makefile and its full test .cpp (Harness
//   class, write_reg/read_reg/tick bus-timing protocol for ciaa.v
//   specifically, and the Checks/expect/note/record class and MUTATION-
//   comment convention this file reuses verbatim as suite-wide plumbing, not
//   as a source of this package's own assertions or expected values, which
//   come only from SPECIFICATION.md and the printed pages it cites).
//
// RTL facts established by reading ciaa.v/cia_int.v (none of this shaped
//   SPECIFICATION.md, which was committed first):
//   - ciaa.v:147 `assign sdr = enable && rs==4'hC` -- the serial data
//     register PROP-1 names.
//   - ciaa.v:167-174: keystrobe is a one-cycle pulse, registered on
//     `kms_level` toggling while `kbd_mouse_type==2`, gated by `clk7n_en`.
//   - ciaa.v:185-186: on that pulse, `sdr_latch[7:0] <=
//     ~{kbd_mouse_data[6:0], kbd_mouse_data[7]}` -- gated by `clk7_en`, one
//     cycle after the strobe registers. Two clock ticks after changing
//     `kms_level` (with `kbd_mouse_type==2` and `kbd_mouse_data` held
//     stable), `sdr_latch` holds the rotated, inverted byte.
//   - ciaa.v:195 `assign sdr_out = (!wr && sdr) ? sdr_latch[7:0] : 8'h00`;
//     ciaa.v:155 ORs it onto `data_out` (every other stream in that OR is
//     zero unless its own register is selected) -- purely combinational,
//     needs no extra tick to read.
//   - ciaa.v:94 `output irq`; ciaa.v:311-326 instantiates `cia_int` with
//     `.ser(keystrobe | ser_tx_irq)`. cia_int.v:22 `icrs` is `rs==4'hD`;
//     cia_int.v:29-35 a write with `data_in[7]=1` OR-sets `icrmask` bits
//     named by `data_in[4:0]`, bit 3 is the `ser` mask bit (cia_int.v:12,
//     :48, :56, :63); cia_int.v:53-58 latch `icr[3] <= icr[3] | ser` every
//     cycle `icrs` is not being read; cia_int.v:62-66 `irq` is the OR of
//     `icrmask & icr` bit by bit -- purely combinational.
//   - ciaa.v:161 declares `keydat` and nothing in the file uses it; not
//     referenced anywhere below, per the build task's own warning.
//   - Consequence used throughout below: `present_and_read()` (strobe a
//     byte, two ticks, read rs=4'hC combinationally) is the one measurement
//     basis PROP-1, PROP-3 through PROP-9, WIDTH-8BIT, MAPPING-SAME-CLASS
//     and FLAG-LAST-GENERAL all share; PROP-2 adds one `write_reg(RS_ICR,
//     ...)` before the same strobe and reads `irq` instead of `data_out`.
//
// Every mutation below is a compile-time variant, replayable with
// `make clean && make MUT=MUT_n test` (n = 0..18, matching the MUT_n token
// at the end of each record line below) -- see the `#if MUT_n` guard at
// each assertion's stimulus/oracle site. `make clean` first is required:
// the object-directory target has no dependency on MUT, so a stale binary
// from a previous variant is silently reused otherwise. Every one of the
// fourteen was actually run this way and the FAIL line quoted is what that
// run printed, not a prediction; each was then reverted (rebuilt with
// MUT unset) and the baseline reconfirmed clean before the next variant.
// Every assertion in this package is green at baseline (the RTL implements
// p.357's rotation rule and p.358's active-low convention exactly, confirmed
// by hand in SPECIFICATION.md before this file existed) -- every mutation
// below is therefore a `stimulus` or `oracle` mutation proving a passing
// assertion is not vacuous, never a `stimulus green` substitution proof
// (nothing here is read at a red baseline).
//
// MUTATION FIXTURE-LIVE stimulus red MUT_0: the second DDRA write changed from the
//   distinct sentinel 0x5A to a repeat of the first (0xA5) -> FAIL, second
//   reading no longer differs from the first (both 0xA5). Reverted.
// MUTATION CONTROL stimulus red MUT_1: the stimulus changed from 0x00 to 0x01
//   while still comparing against the hand-computed 0xFF -> FAIL, actual
//   readback for 0x01 is 0xFD, not 0xFF. Reverted.
// MUTATION opus:8-kbd.c1 stimulus red MUT_2: the second stimulus byte changed from
//   0x22 to 0x11 (a repeat of the first), while the comparison still expects
//   the two rotated readbacks to differ -> FAIL, both readbacks equal
//   (0xDD). Reverted.
// MUTATION opus:8-kbd.c12 stimulus red MUT_3: the interrupt-mask write's data
//   changed from 0x88 (SET, bit 3) to 0x08 (CLEAR, bit 3, a no-op from the
//   post-reset default) -> FAIL, `irq` reads 0 after the same key strobe
//   that reads 1 at baseline. Reverted.
// MUTATION opus:8-kbdtiming.c11 oracle red MUT_4: the "which position the flag
//   alone must drive" comparison changed from position 0 to position 1
//   -> FAIL, bit 7 alone drives position 0 (measured, per PROP-4/PROP-5's
//   sweep), not position 1. Reverted.
// MUTATION opus:8-kbdtiming.c11 stimulus red MUT_14: the bit presented as "the
//   flag" changed from 7 to 6 (a real code bit), oracle left at position 0
//   -> FAIL, bit 6 alone drives position 7 (measured, per PROP-4/PROP-5's
//   sweep), not position 0. Reverted.
// MUTATION opus:8-appG.c26 oracle red MUT_5: the "does this bit's response
//   position differ from its own index" comparator inverted (changed `!=`
//   to `==`) -> FAIL for every one of the eight bit positions (none is a
//   fixed point under the real rotation). Reverted.
// MUTATION opus:8-appG.c26 stimulus red MUT_15: at loop index i=0, the
//   presented bit changed from 0 to 7, oracle comparison (`p != i`) left
//   unchanged -> FAIL, bit 7 alone drives position 0 (measured, per
//   PROP-3/PROP-5), so p==i at that index. Reverted.
// MUTATION opus:8-appG.c27 oracle red MUT_6: the expected per-bit position array
//   changed from {1,2,3,4,5,6,7,0} (the manual's rotation) to
//   {0,1,2,3,4,5,6,7} (identity, i.e. no rotation) -> FAIL, all eight
//   positions reported MISMATCH. Reverted.
// MUTATION opus:8-appG.c27 stimulus red MUT_16: at loop index i=0, the
//   presented bit changed from 0 to 1, expected_position[0]=1 left
//   unchanged -> FAIL, bit 1 alone drives position 2 (measured, per
//   PROP-4/PROP-5's sweep), not position 1. Reverted.
// MUTATION sol:8-appG.c36 stimulus red MUT_7: the injected bit changed from 7 to 6
//   (a different, wrong "flag" bit) -> FAIL, bit 6's response position is 7
//   (measured, per PROP-4/PROP-5's sweep), not 0. Reverted.
// MUTATION opus:8-kbdtiming.c18 stimulus red MUT_8: the presented byte changed
//   from 0x35 to 0x36, expected value left at 0x95 -> FAIL, actual readback
//   for 0x36 is 0x93. Reverted.
// MUTATION opus:8-kbdtiming.c19 oracle red MUT_9: the comparison mask changed from
//   0xFE (bits 7:1) to 0xFF (all 8 bits) -> FAIL, the two readbacks differ
//   at bit 0 by design (that is the up/down flag). Reverted.
// MUTATION opus:8-kbdtiming.c19 stimulus red MUT_17: the "up" stimulus byte
//   changed from 0xB5 to 0xB4 (a different 7-bit code, not merely the flag
//   bit set), mask left at 0xFE -> FAIL, the two readbacks differ on bits
//   7:1 (0xB4's code is 0x34, not 0x35). Reverted.
// MUTATION opus:8-kbdtiming.c22 stimulus red MUT_10: the presented byte changed
//   from 0xB5 to 0xB6, expected value left at 0x94 -> FAIL, actual readback
//   for 0xB6 is 0x92. Reverted.
// MUTATION WIDTH-8BIT stimulus red MUT_11: the sweep range changed from 0-255 to
//   0-127 (still checking for 256 distinct values) -> FAIL, only 128 values
//   were presented, at most 128 distinct readbacks are possible (measured:
//   "128 distinct values of 256"). Reverted.
// MUTATION MAPPING-SAME-CLASS oracle red MUT_12: the expected-value computation for
//   all three special codes changed from `manual_rotate_invert(s.code)` to
//   `manual_rotate_invert(s.code + 1)`, the presented byte left unchanged
//   -> FAIL, all three comparisons report MISMATCH. Reverted.
// MUTATION MAPPING-SAME-CLASS stimulus red MUT_18: the presented byte for
//   all three special codes changed from `s.code` to `s.code + 1`, the
//   expected-value computation left at `manual_rotate_invert(s.code)`
//   -> FAIL, all three comparisons report MISMATCH (the rotation+invert map
//   is bijective, so presenting a different code always reads back
//   differently). Reverted.
// MUTATION FLAG-LAST-GENERAL stimulus red MUT_13: the injected bit changed from 7
//   to 0 (a different, wrong bit) -> FAIL, bit 0's response position is 1
//   (measured, per PROP-4/PROP-5's sweep), not 0. Reverted.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <set>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

// Replayable mutations: `make MUT=MUT_n test` rebuilds with mutation n's
// stimulus/oracle substituted in place of the baseline at that one site;
// unset/empty MUT builds the unmutated baseline. Fourteen sites, MUT_0
// through MUT_13, one per assertion below, in the order they appear.
#ifndef MUT_0
#define MUT_0 0
#endif
#ifndef MUT_1
#define MUT_1 0
#endif
#ifndef MUT_2
#define MUT_2 0
#endif
#ifndef MUT_3
#define MUT_3 0
#endif
#ifndef MUT_4
#define MUT_4 0
#endif
#ifndef MUT_5
#define MUT_5 0
#endif
#ifndef MUT_6
#define MUT_6 0
#endif
#ifndef MUT_7
#define MUT_7 0
#endif
#ifndef MUT_8
#define MUT_8 0
#endif
#ifndef MUT_9
#define MUT_9 0
#endif
#ifndef MUT_10
#define MUT_10 0
#endif
#ifndef MUT_11
#define MUT_11 0
#endif
#ifndef MUT_12
#define MUT_12 0
#endif
#ifndef MUT_13
#define MUT_13 0
#endif
#ifndef MUT_14
#define MUT_14 0
#endif
#ifndef MUT_15
#define MUT_15 0
#endif
#ifndef MUT_16
#define MUT_16 0
#endif
#ifndef MUT_17
#define MUT_17 0
#endif
#ifndef MUT_18
#define MUT_18 0
#endif

namespace {

std::string hex2(uint8_t v) {
    char buf[8];
    std::snprintf(buf, sizeof(buf), "0x%02X", static_cast<unsigned>(v));
    return std::string(buf);
}

constexpr uint8_t RS_DDRA = 0x2;
constexpr uint8_t RS_SDR  = 0xC;
constexpr uint8_t RS_ICR  = 0xD;

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

// Bus-timing protocol for ciaa.v, following the pattern already verified for
// this exact fixture by hrm/ch8-ciaa-port-direction: regporta/ddrporta/
// sdr_latch/icr are posedge-clk, clk7_en-gated (or clk7n_en-gated for the
// keystrobe-detect register, ciaa.v:167-174), so a write needs one tick to
// latch; data_out is combinational (ciaa.v:155, :195) and needs only eval().
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

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    Vciaa* dut;
    bool kms_level_state = false;
};

// ---------------------------------------------------------------------
// FIXTURE-LIVE: `ciaa` has never been measured by this package before.
// Two distinct, known bytes must both arrive at DDRA and read back
// correctly and distinctly before anything else here is trusted -- this
// exercises the harness/clock/reset plumbing only, not the keyboard path.
// ---------------------------------------------------------------------
bool fixture_live(Harness& h, bool repeat_second_word_bug = false) {
    h.write_reg(RS_DDRA, 0xA5);
    const uint8_t r1 = h.read_reg(RS_DDRA);
    h.write_reg(RS_DDRA, repeat_second_word_bug ? 0xA5 : 0x5A);
    const uint8_t r2 = h.read_reg(RS_DDRA);
    return r1 == 0xA5 && r2 == 0x5A && r1 != r2;
}

// ---------------------------------------------------------------------
// Core measurement primitive shared by every keyboard-path assertion below:
// present one byte on the keyboard-data input, toggle the level input to
// create the one edge this boundary's keystrobe detector responds to, run
// two clock ticks (one for the strobe to register, one for the SDR latch to
// capture it -- see the RTL-facts comment above), then read the register
// back at rs=4'hC. Purely mechanical; carries no page claim of its own.
// ---------------------------------------------------------------------
void strobe_key(Harness& h, uint8_t value) {
    h.dut->kbd_mouse_type = 2;
    h.dut->kbd_mouse_data = value;
    h.kms_level_state = !h.kms_level_state;
    h.dut->kms_level = h.kms_level_state ? 1 : 0;
    h.tick();
    h.tick();
}

uint8_t present_and_read(Harness& h, uint8_t value) {
    strobe_key(h, value);
    return h.read_reg(RS_SDR);
}

bool present_and_check_irq(Harness& h, uint8_t value) {
    strobe_key(h, value);
    h.dut->eval();
    return h.dut->irq != 0;
}

// ---------------------------------------------------------------------
// The manual's own model, built from two printed sentences alone (p.357's
// "the transmitted order is therefore 6-5-4-3-2-1-0-7" and p.358's "The
// KDAT line is active low ..."), verified by hand in SPECIFICATION.md
// before ciaa.v was opened. This function encodes that page-derived rule,
// not anything read from the RTL; the assertions below compare the RTL's
// actual behaviour against it.
// ---------------------------------------------------------------------
uint8_t manual_rotate_invert(uint8_t code) {
    static const int order[8] = {6, 5, 4, 3, 2, 1, 0, 7};
    uint8_t transmitted = 0;
    for (int pos = 0; pos < 8; ++pos) {
        const int bit = (code >> order[pos]) & 1;
        transmitted = static_cast<uint8_t>((transmitted << 1) | bit);
    }
    return static_cast<uint8_t>(~transmitted);
}

// Single-bit injection: from an all-zero baseline, set exactly one
// presented bit and report which single register position (0-7) responds.
// -1 = no position responded, -2 = more than one position responded (not a
// clean single-bit-to-single-position map).
int single_bit_response(Harness& h, int bit, int wrong_bit_bug = -1) {
    const uint8_t baseline = present_and_read(h, 0x00);
    const int use_bit = (wrong_bit_bug >= 0) ? wrong_bit_bug : bit;
    const uint8_t withbit = present_and_read(h, static_cast<uint8_t>(1u << use_bit));
    const uint8_t diff = static_cast<uint8_t>(baseline ^ withbit);
    if (diff == 0) return -1;
    if ((diff & (diff - 1)) != 0) return -2;
    for (int p = 0; p < 8; ++p)
        if (diff & (1u << p)) return p;
    return -3;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // -------------------------------------------------------------
    { Harness h;
#if MUT_0
      const bool fixture_ok = fixture_live(h, /*repeat_second_word_bug=*/true);
#else
      const bool fixture_ok = fixture_live(h);
#endif
      checks.expect(fixture_ok,
          "FIXTURE-LIVE: two distinct, known DDRA bytes (0xA5, 0x5A) both "
          "arrive and read back correctly and distinctly before any "
          "keyboard-path assertion is graded [no page -- infrastructure]");
    }

    // -------------------------------------------------------------
    { Harness h;
#if MUT_1
      const uint8_t got = present_and_read(h, 0x01);
#else
      const uint8_t got = present_and_read(h, 0x00);
#endif
      checks.expect(got == 0xFF,
          "CONTROL: a hand-computable sanity point distinct from the two "
          "worked examples below -- an all-zero presented byte rotates to "
          "all zero and its active-low complement is 0xFF -- proving the "
          "present_and_read() measurement basis PROP-7/PROP-8/PROP-9 depend "
          "on works before those are graded [no page -- measurement-basis "
          "control]");
      checks.note("CONTROL 0x00 -> " + hex2(got));
    }

    // -------------------------------------------------------------
    // PROP-1 (p.251): the keyboard reaches the system through ciaa's
    // serial shift register -- two distinct, arbitrary bytes both arrive
    // correctly rotated+inverted at rs=4'hC, proving the register named by
    // the build task (not some other, dead path) is genuinely the vehicle.
    // -------------------------------------------------------------
    { Harness h;
      const uint8_t got1 = present_and_read(h, 0x11);
#if MUT_2
      const uint8_t got2 = present_and_read(h, 0x11);
#else
      const uint8_t got2 = present_and_read(h, 0x22);
#endif
      const bool result = got1 == manual_rotate_invert(0x11) &&
                           got2 == manual_rotate_invert(0x22) && got1 != got2;
      checks.expect(result,
          "opus:8-kbd.c1 sol:8-kbd.c1 PROP-1: the keyboard reaches the "
          "system through the serial data register at rs=4'hC -- two "
          "distinct presented bytes (0x11, 0x22) both read back exactly as "
          "the manual's own rotation-plus-active-low rule predicts, and "
          "differ from each other, so the register is a live, correctly "
          "wired vehicle for the byte, not a dead or stuck port [p.251 "
          "\"The keyboard is interfaced to the system via the serial shift "
          "register on one of the 8520 CIA chips\"]");
      checks.note("PROP-1: 0x11 -> " + hex2(got1) + ", 0x22 -> " + hex2(got2));
    }

    // -------------------------------------------------------------
    // PROP-2 (p.251): receiving a byte issues an interrupt to the
    // processor -- arm the SP mask bit, present a byte, observe irq.
    // -------------------------------------------------------------
    { Harness h;
#if MUT_3
      h.write_reg(RS_ICR, 0x08);  // CLEAR bit3, a no-op from the post-reset default
#else
      h.write_reg(RS_ICR, 0x88);  // SET (bit7=1), mask bit 3 (SP/ser source)
#endif
      const bool result = present_and_check_irq(h, 0x35);
      checks.expect(result,
          "opus:8-kbd.c12 sol:8-kbd.c9 PROP-2: after a data byte has been "
          "received from the keyboard, ciaa's irq output asserts -- with "
          "the SP interrupt source armed, presenting a byte and driving the "
          "same whole-byte strobe PROP-1 uses makes irq read 1 [p.251 "
          "\"After a data byte has been received from the keyboard, an "
          "interrupt from the 8520 is issued to the processor\"]");
    }

    // -------------------------------------------------------------
    // PROP-3 (p.358): keycodes are always 7 bits long -- the 7 code bits
    // and the 1 flag bit occupy disjoint register positions.
    // -------------------------------------------------------------
    { Harness h;
      bool code_bits_avoid_position0 = true;
      std::string detail;
      for (int i = 0; i <= 6; ++i) {
          const int p = single_bit_response(h, i);
          code_bits_avoid_position0 = code_bits_avoid_position0 && (p != 0);
          detail += "bit" + std::to_string(i) + "->pos" + std::to_string(p) + " ";
      }
#if MUT_14
      const int flag_position = single_bit_response(h, 7, /*wrong_bit_bug=*/6);
#else
      const int flag_position = single_bit_response(h, 7);
#endif
#if MUT_4
      constexpr int expected_flag_position = 1;
#else
      constexpr int expected_flag_position = 0;
#endif
      const bool result = code_bits_avoid_position0 && (flag_position == expected_flag_position);
      checks.expect(result,
          "opus:8-kbdtiming.c11 sol:8-appG.c50 PROP-3: keycodes are always "
          "7 bits long -- sweeping presented bits 0-6 (the code) never "
          "moves register position 0, and presented bit 7 alone (the flag) "
          "moves only position 0 -- the code and the flag occupy disjoint "
          "register positions, distinguishing a 7-bit-code-plus-flag layout "
          "from an undifferentiated 8-bit code [p.358 \"Keycodes are always "
          "7 bits long\"]");
      checks.note("PROP-3 per-bit positions: " + detail +
                   "bit7->pos" + std::to_string(flag_position));
    }

    // -------------------------------------------------------------
    // PROP-4 (p.357): all codes are rotated one bit before transmission --
    // every presented bit moves to a DIFFERENT register position (no
    // fixed points), proving the map is a genuine rotation, not identity.
    // -------------------------------------------------------------
    { Harness h;
      bool all_moved = true;
      std::string detail;
      for (int i = 0; i < 8; ++i) {
#if MUT_15
          const int p = single_bit_response(h, i, /*wrong_bit_bug=*/(i == 0) ? 7 : -1);
#else
          const int p = single_bit_response(h, i);
#endif
#if MUT_5
          const bool moved = (p >= 0) && (p == i);
#else
          const bool moved = (p >= 0) && (p != i);
#endif
          all_moved = all_moved && moved;
          detail += "bit" + std::to_string(i) + "->pos" + std::to_string(p) + " ";
      }
      checks.expect(all_moved,
          "opus:8-appG.c26 sol:8-appG.c34 PROP-4: all codes transmitted to "
          "the computer are rotated one bit before transmission -- for "
          "every one of the eight presented bit positions, the single "
          "register position it drives is never its own index, i.e. no bit "
          "position is a fixed point of the map [p.357 \"All codes "
          "transmitted to the computer are rotated one bit before "
          "transmission\"]");
      checks.note("PROP-4 per-bit positions: " + detail);
    }

    // -------------------------------------------------------------
    // PROP-5 (p.357 + p.358's general figure labels): the transmitted
    // order is exactly 6-5-4-3-2-1-0-7.
    // -------------------------------------------------------------
    { Harness h;
#if MUT_6
      static const int expected_position[8] = {0, 1, 2, 3, 4, 5, 6, 7};
#else
      static const int expected_position[8] = {1, 2, 3, 4, 5, 6, 7, 0};
#endif
      // expected_position[i] = serial-position-index-0 that presented bit i
      // drives, per p.357/p.358's row (6)(5)(4)(3)(2)(1)(0)(7) -> register
      // bits 7,6,5,4,3,2,1,0 in that same left-to-right order.
      bool all_match = true;
      std::string detail;
      for (int i = 0; i < 8; ++i) {
#if MUT_16
          const int p = single_bit_response(h, i, /*wrong_bit_bug=*/(i == 0) ? 1 : -1);
#else
          const int p = single_bit_response(h, i);
#endif
          const bool ok = (p == expected_position[i]);
          all_match = all_match && ok;
          detail += "bit" + std::to_string(i) + "->pos" + std::to_string(p) +
                     (ok ? "(ok) " : "(MISMATCH) ");
      }
      checks.expect(all_match,
          "opus:8-appG.c27 sol:8-appG.c35 opus:8-kbdtiming.c23 "
          "opus:8-kbdtiming.c24 opus:8-kbdtiming.c25 sol:8-appG.c41 "
          "sol:8-appG.c42 sol:8-appG.c179 sol:8-appG.c180 sol:8-appG.c181 "
          "sol:8-appG.c182 sol:8-appG.c183 sol:8-appG.c184 sol:8-appG.c185 "
          "sol:8-appG.c186 PROP-5: the transmitted order is exactly "
          "6-5-4-3-2-1-0-7 -- for each of the eight presented bit "
          "positions, the single register position it drives matches the "
          "page's own labelled row exactly: bit6->pos1(reg7,first sent), "
          "bit5->pos2, bit4->pos3, bit3->pos4, bit2->pos5, bit1->pos6, "
          "bit0->pos7, bit7->pos8(reg0,last sent) [p.357 \"The transmitted "
          "order is therefore 6-5-4-3-2-1-0-7\"; p.358's KCLK/KDAT figure, "
          "the general (6)(5)(4)(3)(2)(1)(0)(7) label row with \"First "
          "sent\"/\"Last sent\"]");
      checks.note("PROP-5 per-bit positions: " + detail);
    }

    // -------------------------------------------------------------
    // PROP-6 (p.357): the up/down flag is transmitted last -- bit 7 alone
    // drives the last register position (position 8 / register bit 0).
    // -------------------------------------------------------------
    { Harness h;
#if MUT_7
      const int p = single_bit_response(h, 6);
#else
      const int p = single_bit_response(h, 7);
#endif
      checks.expect(p == 0,
          "sol:8-appG.c36 PROP-6: the up/down flag is transmitted last -- "
          "presented bit 7 alone drives register position 0, the last "
          "serial position PROP-5 establishes [p.357 \"The reason for this "
          "is to transmit the up/down flag last, in order to cause a "
          "key-up code to be transmitted in case the keyboard is forced to "
          "restore lost sync\" -- the ordering half of this sentence only; "
          "the sync-recovery reason is p.359's territory and needs KCLK/"
          "KDAT this boundary does not have]");
    }

    // -------------------------------------------------------------
    // PROP-7 (p.358): worked example -- the B key pushed down ($35)
    // transmits 01101010, i.e. reads back as $95.
    // -------------------------------------------------------------
    { Harness h;
#if MUT_8
      const uint8_t got = present_and_read(h, 0x36);
#else
      const uint8_t got = present_and_read(h, 0x35);
#endif
      checks.expect(got == 0x95,
          "opus:8-kbdtiming.c18 sol:8-appG.c57 opus:8-kbdtiming.c28 "
          "opus:8-kbdtiming.c29 opus:8-kbdtiming.c30 opus:8-kbdtiming.c31 "
          "opus:8-kbdtiming.c32 opus:8-kbdtiming.c33 opus:8-kbdtiming.c34 "
          "opus:8-kbdtiming.c35 sol:8-appG.c187 sol:8-appG.c188 "
          "sol:8-appG.c189 sol:8-appG.c190 sol:8-appG.c191 sol:8-appG.c192 "
          "sol:8-appG.c193 sol:8-appG.c194 PROP-7: pushing the B key ($35 = "
          "00110101) transmits, due to the rotation, the bits 01101010 -- "
          "presenting $35 reads back exactly $95 (= ~01101010), matching "
          "both the whole-byte statement and each of the figure's eight "
          "individually-labelled bit positions for the \"B pushed down\" "
          "waveform at once [p.358 \"The keycode for 'B' is $35 = 00110101; "
          "due to the rotation of the byte, the bits transmitted are "
          "01101010\", and the KCLK/KDAT figure's second waveform]");
      checks.note("PROP-7: 0x35 -> " + hex2(got));
    }

    // -------------------------------------------------------------
    // PROP-8 (p.358): releasing the key does not change the 7-bit code --
    // $35 (down) and $B5 (up) agree on register bits 7:1.
    // -------------------------------------------------------------
    { Harness h;
      const uint8_t down = present_and_read(h, 0x35);
#if MUT_17
      const uint8_t up = present_and_read(h, 0xB4);
#else
      const uint8_t up = present_and_read(h, 0xB5);
#endif
#if MUT_9
      const bool result = (down & 0xFF) == (up & 0xFF);
#else
      const bool result = (down & 0xFE) == (up & 0xFE);
#endif
      checks.expect(result,
          "opus:8-kbdtiming.c19 sol:8-appG.c58 PROP-8: releasing the B key "
          "does not change its 7-bit keycode -- presenting $35 (down) and "
          "$B5 (up, same code with bit 7 set) produce register readbacks "
          "that agree on bits 7 down to 1, differing only at bit 0 (the "
          "flag PROP-6 places there) [p.358 \"the B key is released. The "
          "keycode is still $35, except that bit 7 is set to indicate "
          "'key-up'\"]");
      checks.note("PROP-8: down=" + hex2(down) + " up=" + hex2(up));
    }

    // -------------------------------------------------------------
    // PROP-9 (p.358): worked example -- the B key released ($B5)
    // transmits 01101011, i.e. reads back as $94.
    // -------------------------------------------------------------
    { Harness h;
#if MUT_10
      const uint8_t got = present_and_read(h, 0xB6);
#else
      const uint8_t got = present_and_read(h, 0xB5);
#endif
      checks.expect(got == 0x94,
          "opus:8-kbdtiming.c22 sol:8-appG.c62 opus:8-kbdtiming.c37 "
          "opus:8-kbdtiming.c38 opus:8-kbdtiming.c39 opus:8-kbdtiming.c40 "
          "opus:8-kbdtiming.c41 opus:8-kbdtiming.c42 opus:8-kbdtiming.c43 "
          "opus:8-kbdtiming.c44 sol:8-appG.c195 sol:8-appG.c196 "
          "sol:8-appG.c197 sol:8-appG.c198 sol:8-appG.c199 sol:8-appG.c200 "
          "sol:8-appG.c201 sol:8-appG.c202 PROP-9: releasing the B key "
          "($B5 = 10110101) transmits, after rotating, 01101011 -- "
          "presenting $B5 reads back exactly $94 (= ~01101011), matching "
          "both the whole-byte statement and each of the figure's eight "
          "individually-labelled bit positions for the \"B released\" "
          "waveform at once [p.358 \"resulting in a code of $B5 = "
          "10110101. After rotating, the transmission will be 01101011\", "
          "and the KCLK/KDAT figure's third waveform]");
      checks.note("PROP-9: 0xB5 -> " + hex2(got));
    }

    // -------------------------------------------------------------
    // WIDTH-8BIT (untagged -- backs opus:8-appG.c13, sol:8-appG.c20,
    // opus:8-appG.c142, sol:8-appG.c170 via record() below). Exhaustive
    // sweep: 256 presented bytes must produce 256 distinct readbacks, i.e.
    // no information is lost between an 8-bit input and the register.
    // -------------------------------------------------------------
    { Harness h;
      std::set<uint8_t> seen;
#if MUT_11
      for (int v = 0; v < 128; ++v) {
#else
      for (int v = 0; v < 256; ++v) {
#endif
          seen.insert(present_and_read(h, static_cast<uint8_t>(v)));
      }
      const bool width_8bit_result = (seen.size() == 256);
      checks.expect(width_8bit_result,
          "WIDTH-8BIT: sweeping all 256 presented byte values produces 256 "
          "distinct register readbacks -- the register carries the full "
          "8-bit width without collapsing any two distinct inputs to the "
          "same value [no uid on this line -- see the four p.357/p.364 "
          "8-bit-width record() lines below, whose gradeable half this "
          "measures]");
      checks.note("WIDTH-8BIT: " + std::to_string(seen.size()) + " distinct values of 256");
    }

    // -------------------------------------------------------------
    // MAPPING-SAME-CLASS (untagged -- backs opus:8-appG.c144,
    // sol:8-appG.c172 via record() below). p.364's own listed special
    // codes, checked against the identical rotate+invert rule PROP-5/
    // PROP-7/PROP-9 already establish for ordinary keycodes.
    // -------------------------------------------------------------
    { Harness h;
      struct { uint8_t code; const char* name; } specials[] = {
          {0x78, "Reset warning"},
          {0xF9, "Last key code bad / lost sync"},
          {0xFC, "Keyboard selftest failed"},
      };
      bool all_match = true;
      std::string detail;
      for (const auto& s : specials) {
#if MUT_18
          const uint8_t got = present_and_read(h, static_cast<uint8_t>(s.code + 1));
#else
          const uint8_t got = present_and_read(h, s.code);
#endif
#if MUT_12
          const uint8_t want = manual_rotate_invert(static_cast<uint8_t>(s.code + 1));
#else
          const uint8_t want = manual_rotate_invert(s.code);
#endif
          const bool ok = (got == want);
          all_match = all_match && ok;
          detail += std::string(s.name) + " " + hex2(s.code) +
                     "->" + hex2(got) +
                     (ok ? "(ok) " : "(MISMATCH) ");
      }
      checks.expect(all_match,
          "MAPPING-SAME-CLASS: p.364's own listed special codes ($78 reset "
          "warning, $F9 lost sync, $FC selftest failed) pass through the "
          "identical rotate-plus-active-low rule ordinary keycodes use -- "
          "`ciaa` has no notion of code class, so the same mapping applies "
          "uniformly [no uid on this line -- see the two p.364 "
          "transmission-bit-order record() lines below, whose gradeable "
          "half this measures]");
      checks.note("MAPPING-SAME-CLASS: " + detail);
    }

    // -------------------------------------------------------------
    // FLAG-LAST-GENERAL (untagged -- backs sol:8-appG.c82 via record()
    // below). Same measurement as PROP-6, computed independently and left
    // unlabelled with any uid, per the build brief's partial-grading rule.
    // -------------------------------------------------------------
    { Harness h;
#if MUT_13
      const int p = single_bit_response(h, 0);
#else
      const int p = single_bit_response(h, 7);
#endif
      checks.expect(p == 0,
          "FLAG-LAST-GENERAL: presented bit 7 alone drives register "
          "position 0, the last serial position, independent of every "
          "other presented bit [no uid on this line -- see the p.359 "
          "record() line below, whose gradeable half this measures]");
    }

    // ------------------------------------------------------------------
    // Partially-gradeable claims (7). Each is graded in full for its
    // byte-level half by one of the untagged assertions above; the
    // serial-cable half (delivery over time on KCLK/KDAT, or special-code
    // generation) is not observable at this boundary and is declined here,
    // per the build brief's rule that a partially-gradeable claim's uid
    // never appears on a graded (`expect()`) line. No uid below appears on
    // any `expect()` line in this file.
    // ------------------------------------------------------------------
    checks.record(
        "opus:8-appG.c13 (p.357, \"The keyboard transmits 8-bit data words "
        "serially to the main unit\"): the 8-bit-width half is graded by "
        "WIDTH-8BIT above (256 presented values -> 256 distinct readbacks); "
        "\"...serially\" names KCLK-clocked, bit-at-a-time delivery this "
        "boundary does not have (one whole-byte latch on one strobe, no "
        "CNT/SP port). Not graded here.");
    checks.record(
        "sol:8-appG.c20 (p.357, twin of opus:8-appG.c13, same 8-bit/serially "
        "text): same basis as opus:8-appG.c13 above. Not graded here.");
    checks.record(
        "opus:8-appG.c142 (p.364, \"The special codes are 8-bit numbers\"): "
        "the 8-bit-width half is graded by WIDTH-8BIT above; asserting this "
        "specifically of \"special codes\" (a category `ciaa` does not "
        "distinguish -- every presented byte is carried identically, "
        "whatever it is taken to mean upstream) needs a special-code "
        "generator this RTL does not contain. Not graded here.");
    checks.record(
        "sol:8-appG.c170 (p.364, twin of opus:8-appG.c142, same 8-bit-number "
        "text): same basis as opus:8-appG.c142 above. Not graded here.");
    checks.record(
        "opus:8-appG.c144 (p.364, \"However, the transmission bit order is "
        "the same as previously described\"): the mapping-sameness-across-"
        "code-classes half is graded by MAPPING-SAME-CLASS above (p.364's "
        "own listed special codes pass through the identical rule PROP-5/"
        "PROP-7/PROP-9 establish); \"transmission bit order\" as a subject "
        "names Appendix G's KCLK-clocked serial order on the cable, which "
        "this boundary does not have. Not graded here.");
    checks.record(
        "sol:8-appG.c172 (p.364, twin of opus:8-appG.c144, same transmission-"
        "bit-order text): same basis as opus:8-appG.c144 above. Not graded "
        "here.");
    checks.record(
        "sol:8-appG.c82 (p.359, \"That is why the key-up/key-down flag is "
        "always transmitted last\"): the position half is graded by "
        "FLAG-LAST-GENERAL above (bit 7 alone drives the last register "
        "position); \"always\", read as spanning every transmission over "
        "time on a wire, needs KCLK/KDAT this boundary does not have. Not "
        "graded here.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
