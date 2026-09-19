// Test purpose: conformance test for JOYTEST's write effect.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 227, 241, 263, 264, 281, 282, and more
// Non-goals: anything outside the `userio` fixture this package builds, and
//   any claim outside the 44 it cites. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 44 ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s1-build/ch8-joytest-counter-write.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// ch8-joytest-counter-write -- conformance test for JOYTEST's write effect.
//
// Fixture: userio.v alone (Verilator 5.052, --top-module userio).
// Oracle: printed page 282 (Appendix A), quoted in full in SPECIFICATION.md
// and again next to each assertion below. If a check disagrees with the RTL,
// the RTL is what is wrong -- a red result here is a valid, finished outcome.
//
// Scope: JOYTEST's *semantic* write effect only -- that one write loads all
// four joystick-mouse counters at once, and the specific write-bit -> counter
// mapping. JOYTEST's ADDRESS and its read/write direction are graded by
// ch8-register-map-userio and are not re-graded here.
//
// REPAIR (2026-09-15, independent validation report B, finding 10): the
// twelve claims about the table's "xx" cells (opus .c11 .c12 .c19 .c20 .c27
// .c28 .c35 .c36; sol .c6 .c8 .c10 .c12) were originally graded through
// expect(), asserting that toggling such a write bit must leave the counter
// unchanged. The claims themselves only say the table defines no test-data
// value there -- "undefined" is not "must be ignored", and nothing on p.282
// states a required behaviour for those positions. Checked for a printed
// rule before repairing, not assumed: pp.227-241 (chapter body), the
// Appendix A preamble (pp.263-264), pp.281-282 themselves, and pp.287-289
// (Appendix B) were read. One relevant rule exists, p.264: "If a bit is
// described as unused in a write-only register, be sure to keep that bit
// clear when writing values to that register. ... Setting unused bits in a
// write-only register ... may cause serious future software incompatibility
// if those bits or addresses are implemented in the future by Amiga, Inc."
// That is advice to the PROGRAMMER about what to write, and a warning that
// present hardware behaviour on such bits is deliberately left open for
// future use -- it does not promise the current implementation leaves the
// counter unchanged. No rule found defines these twelve positions, so they
// are now declined via record() as unspecified rather than graded, and carry
// no MUTATION record (declined claims take no mutation record). The
// twelve's old MUT_UNDEFSHIFT mutations rerouted the probe onto a defined
// neighbouring bit -- which only shows that comparator CAN fail somewhere,
// not that the invented "must be unchanged" rule is real. That is preserved
// here only as history in this comment, not as a record line.
//
// Measured mechanism (userio.v, read after SPECIFICATION.md was committed):
//   - JOYTEST decode is a level, not a strobe: `test_load` is simply
//     `reg_address_in == JOYTEST` (userio.v:317). This harness therefore reads
//     data_out with a purely combinational address change immediately after
//     the one posedge that captured the write, and never lets a second
//     clk7_en-active cycle elapse before reading -- an extra idle cycle would
//     let the *live* quadrature-phase logic (see next point) re-assert itself
//     and silently overwrite what the write just did.
//   - JOY0DAT (mouse mode, the default -- port 1) is `mouse0dat + dmouse0dat`.
//     A JOYTEST write loads test_data into `ycount[7:2]`/`xcount[7:2]`
//     (userio.v:342-343) and, on the *same* edge, unconditionally zeroes all
//     of `dmouse0dat` (userio.v:233,246) -- bits 9:8 and 1:0 included, even
//     though the write-test-data table does not source them from test_data.
//     JOY0DAT's "no test data" bits therefore read 0 after every write.
//   - JOY1DAT (port 2) is `dmouse1dat` directly. A JOYTEST write loads
//     test_data into `dmouse1dat[15:10]`/`[7:2]` (userio.v:260,273) but never
//     assigns `dmouse1dat[9:8]`/`[1:0]` in the test_load branch at all -- so
//     those bits simply keep whatever the live quadrature-phase capture
//     (userio.v:266,279) last put there. Both ports are held at the same
//     idle input pattern by this harness (_joy1=_joy2=16'h0000), so those
//     bits settle at a fixed, nonzero pattern before the first write and are
//     carried unchanged through every JOYTEST write after that.
//   - This is the RTL clearing different bits on the two ports the task
//     description warned about: MEASURED here, an all-1s write ($FFFF)
//     produced JOY0DAT=$FCFC and JOY1DAT=$FEFE -- matching the carried-
//     forward finding exactly, for the mechanistic reason above, not by
//     coincidence.
//
// Every assertion is graded through expect(), labelled with the exact
// classify.uid from the claims file (opus:8-appa-joytest.cN /
// sol:8-appa-joytest.cN) -- the ledger reads uids off graded lines, so a uid
// appearing on an expect() line counts as ASSERTED regardless of nearby
// prose. One infrastructure check (fixture word delivery) is graded without
// a uid, since it is not a manual claim.
//
// Mutation policy: every assertion's *stimulus* is changed, its expected
// value is left alone, per assertion below. Because several assertions are
// structurally identical (same comparator, same kind of proposition, only
// the bit position or register differs), four `MUT_*` flags each guard a
// *family* of assertions rather than one flag per uid; every uid in a family
// was independently re-run with its family's flag defined and confirmed red
// (see the coverage note in the header list). `make clean && make MUT=<NAME>`
// replays any one of them.
//
// MUTATION opus:8-appa-joytest.c4 stimulus red wrote to address 0x00, not JOYTEST (MUT_ADDR)
// MUTATION sol:8-appa-joytest.c4 stimulus red wrote to address 0x00, not JOYTEST (MUT_ADDR)
// MUTATION opus:8-appa-joytest.c5 stimulus red one-hot write bit shifted 15->14 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c6 stimulus red one-hot write bit shifted 14->13 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c7 stimulus red one-hot write bit shifted 13->12 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c8 stimulus red one-hot write bit shifted 12->11 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c9 stimulus red one-hot write bit shifted 11->10 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c10 stimulus red one-hot write bit shifted 10->15 (MUT_BITSHIFT)
// MUTATION sol:8-appa-joytest.c5 stimulus red 6-bit Y-field pattern inverted (MUT_RANGEINV)
// MUTATION opus:8-appa-joytest.c13 stimulus red one-hot write bit shifted 7->6 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c14 stimulus red one-hot write bit shifted 6->5 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c15 stimulus red one-hot write bit shifted 5->4 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c16 stimulus red one-hot write bit shifted 4->3 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c17 stimulus red one-hot write bit shifted 3->2 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c18 stimulus red one-hot write bit shifted 2->7 (MUT_BITSHIFT)
// MUTATION sol:8-appa-joytest.c7 stimulus red 6-bit X-field pattern inverted (MUT_RANGEINV)
// MUTATION opus:8-appa-joytest.c21 stimulus red one-hot write bit shifted 15->14 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c22 stimulus red one-hot write bit shifted 14->13 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c23 stimulus red one-hot write bit shifted 13->12 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c24 stimulus red one-hot write bit shifted 12->11 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c25 stimulus red one-hot write bit shifted 11->10 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c26 stimulus red one-hot write bit shifted 10->15 (MUT_BITSHIFT)
// MUTATION sol:8-appa-joytest.c9 stimulus red 6-bit Y-field pattern inverted (MUT_RANGEINV)
// MUTATION opus:8-appa-joytest.c29 stimulus red one-hot write bit shifted 7->6 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c30 stimulus red one-hot write bit shifted 6->5 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c31 stimulus red one-hot write bit shifted 5->4 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c32 stimulus red one-hot write bit shifted 4->3 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c33 stimulus red one-hot write bit shifted 3->2 (MUT_BITSHIFT)
// MUTATION opus:8-appa-joytest.c34 stimulus red one-hot write bit shifted 2->7 (MUT_BITSHIFT)
// MUTATION sol:8-appa-joytest.c11 stimulus red 6-bit X-field pattern inverted (MUT_RANGEINV)
// MUTATION fixture-delivers-distinct-words stimulus red second write repeats the first word instead of a different one (MUT_FIXTURE)

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include "Vuserio.h"
#include "verilated.h"

// ---------------------------------------------------------------------------
// harness -- three verbs, not interchangeable. record() is kept character-
// identical to the form fixed by the brief.
// ---------------------------------------------------------------------------

struct Checks {
    int passed = 0;
    int failed = 0;
};
static Checks checks;

static void expect(bool cond, const std::string& label) {
    if (cond) {
        checks.passed++;
        std::cout << "PASS: " << label << "\n";
    } else {
        checks.failed++;
        std::cout << "FAIL: " << label << "\n";
    }
}

static void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }

void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

// ---------------------------------------------------------------------------
// register addresses -- reg_address_in is [8:1], i.e. the module's own
// 9-bit parameter shifted right by one. Taken directly from userio.v's own
// `parameter` declarations (lines 54-61), not guessed.
// ---------------------------------------------------------------------------

static const uint8_t JOY0DAT_ADDR = 0x05;  // 9'h00a >> 1
static const uint8_t JOY1DAT_ADDR = 0x06;  // 9'h00c >> 1
static const uint8_t JOYTEST_ADDR = 0x1B;  // 9'h036 >> 1

// ---------------------------------------------------------------------------
// DUT plumbing
// ---------------------------------------------------------------------------

static void tick(Vuserio* top) {
    top->clk = 0;
    top->eval();
    top->clk = 1;
    top->eval();
}

// Fresh, deterministic baseline: reset, then idle long enough for the
// two-stage joystick synchronizers to settle. Joystick/mouse inputs are held
// at a fixed, never-changing pattern for the rest of the run, so the
// quadrature-decode logic that shares userio.v with JOYTEST never fires a
// transition of its own -- everything this file measures comes from the
// JOYTEST writes it performs, not from incidental joystick activity.
static void reset_dut(Vuserio* top) {
    top->clk7_en = 1;
    top->reg_address_in = 0;
    top->data_in = 0;
    top->pot_cnt_en = 0;
    top->_fire0_dat = 1;
    top->_fire1_dat = 1;
    top->_joy1 = 0x0000;
    top->_joy2 = 0x0000;
    top->joy_ana1 = 0;
    top->joy_ana2 = 0;
    top->mouse_btn = 0;
    top->kms_level = 0;
    top->kbd_mouse_type = 0;
    top->kbd_mouse_data = 0;
    top->reset = 1;
    for (int i = 0; i < 4; i++) tick(top);
    top->reset = 0;
    for (int i = 0; i < 20; i++) tick(top);

    // userio.v never resets dmouse1dat (no `if(reset)` branch touches it --
    // confirmed by reading the module, not assumed): its data-carrying bits
    // [15:10]/[7:2] persist across `reset` and are only replaced by an
    // actual JOYTEST write. A bare `reset` is therefore not a clean baseline
    // for JOY1DAT by itself. Write a known, all-zero word once so every test
    // below starts from a *written*, not merely reset-defaulted, baseline --
    // this also does not disturb the just-settled quadrature-phase pattern
    // in bits 9:8/1:0, since that write never touches those bits either.
    top->reg_address_in = JOYTEST_ADDR;
    top->data_in = 0x0000;
    tick(top);
    top->reg_address_in = 0;
    top->eval();
}

// One JOYTEST write: hold the address for exactly the one active cycle that
// captures it. userio.v has no separate write strobe (task-carried fact,
// confirmed at userio.v:317: test_load is bare address-match, a level) --
// this is the actually-observable conjunction that loads data, not something
// asserted from the manual's "W" column.
static void write_joytest(Vuserio* top, uint16_t value) {
    top->reg_address_in = JOYTEST_ADDR;
    top->data_in = value;
    tick(top);
}

// Reading is a combinational address-mux lookup (userio.v:289-310) -- no
// clock edge, so it disturbs nothing. Crucially this means it is safe to
// call immediately after write_joytest() without an extra idle cycle: one
// extra clk7_en-active cycle would let the live quadrature-phase capture
// (userio.v:239,252,266,279) run again and silently overwrite what the
// write just placed in dmouse0dat/dmouse1dat's low two bits per half.
static uint16_t read_joy0dat(Vuserio* top) {
    top->reg_address_in = JOY0DAT_ADDR;
    top->eval();
    return top->data_out;
}
static uint16_t read_joy1dat(Vuserio* top) {
    top->reg_address_in = JOY1DAT_ADDR;
    top->eval();
    return top->data_out;
}

typedef uint16_t (*ReadFn)(Vuserio*);

// ---------------------------------------------------------------------------
// claim table -- one row per opus per-bit claim. The `letter` field is the
// write-test-data table's own cell text (page 282); it is not used for
// grading, only to keep the source of each row legible.
// ---------------------------------------------------------------------------

struct BitProp {
    int wbit;
    const char* letter;
    const char* uid;
};

static const BitProp k0DatY[] = {
    {15, "Y7", "opus:8-appa-joytest.c5"},  {14, "Y6", "opus:8-appa-joytest.c6"},
    {13, "Y5", "opus:8-appa-joytest.c7"},  {12, "Y4", "opus:8-appa-joytest.c8"},
    {11, "Y3", "opus:8-appa-joytest.c9"},  {10, "Y2", "opus:8-appa-joytest.c10"},
};
static const BitProp k0DatX[] = {
    {7, "X7", "opus:8-appa-joytest.c13"}, {6, "X6", "opus:8-appa-joytest.c14"},
    {5, "X5", "opus:8-appa-joytest.c15"}, {4, "X4", "opus:8-appa-joytest.c16"},
    {3, "X3", "opus:8-appa-joytest.c17"}, {2, "X2", "opus:8-appa-joytest.c18"},
};
static const BitProp k1DatY[] = {
    {15, "Y7", "opus:8-appa-joytest.c21"}, {14, "Y6", "opus:8-appa-joytest.c22"},
    {13, "Y5", "opus:8-appa-joytest.c23"}, {12, "Y4", "opus:8-appa-joytest.c24"},
    {11, "Y3", "opus:8-appa-joytest.c25"}, {10, "Y2", "opus:8-appa-joytest.c26"},
};
static const BitProp k1DatX[] = {
    {7, "X7", "opus:8-appa-joytest.c29"}, {6, "X6", "opus:8-appa-joytest.c30"},
    {5, "X5", "opus:8-appa-joytest.c31"}, {4, "X4", "opus:8-appa-joytest.c32"},
    {3, "X3", "opus:8-appa-joytest.c33"}, {2, "X2", "opus:8-appa-joytest.c34"},
};

static const BitProp k0DatYUndef[] = {
    {9, "bit9=xx", "opus:8-appa-joytest.c11"},
    {8, "bit8=xx", "opus:8-appa-joytest.c12"},
};
static const BitProp k0DatXUndef[] = {
    {1, "bit1=xx", "opus:8-appa-joytest.c19"},
    {0, "bit0=xx", "opus:8-appa-joytest.c20"},
};
static const BitProp k1DatYUndef[] = {
    {9, "bit9=xx", "opus:8-appa-joytest.c27"},
    {8, "bit8=xx", "opus:8-appa-joytest.c28"},
};
static const BitProp k1DatXUndef[] = {
    {1, "bit1=xx", "opus:8-appa-joytest.c35"},
    {0, "bit0=xx", "opus:8-appa-joytest.c36"},
};

template <int N>
static int arrlen(const BitProp (&)[N]) { return N; }

// ---------------------------------------------------------------------------
// mutation helpers -- move the driven bit within its own 6-bit field
// (data-bit family), or reroute an "undefined" bit's probe onto its nearest
// data-carrying neighbour (undefined-bit family). Both keep the write
// one-hot; only which position is exercised changes.
// ---------------------------------------------------------------------------

static int shift_within_group(int wbit) {
    if (wbit >= 10) return (wbit == 10) ? 15 : wbit - 1;
    return (wbit == 2) ? 7 : wbit - 1;
}

// ---------------------------------------------------------------------------
// assertion families
// ---------------------------------------------------------------------------

// Per-bit data proposition: a one-hot write of this bit must produce the
// identically-positioned one-hot readback, masked to the bits JOYTEST
// actually loads (0xFCFC) so the separately-graded "no test data" bits never
// contaminate this comparison.
static void test_data_bit(Vuserio* top, ReadFn rd, const BitProp& p) {
    reset_dut(top);
    const uint16_t expected = static_cast<uint16_t>(1u << p.wbit);
    uint16_t stim = expected;
#ifdef MUT_BITSHIFT
    stim = static_cast<uint16_t>(1u << shift_within_group(p.wbit));
#endif
    write_joytest(top, stim);
    uint16_t got = static_cast<uint16_t>(rd(top) & 0xFCFCu);
    expect(got == expected, p.uid);
}

// Composed range proposition (sol): a single write carrying a non-repeating
// 6-bit pattern across the field must read back exactly that pattern -- a
// genuinely different stimulus from the one-hot family above, not a re-use
// of its measurements.
static void test_range(Vuserio* top, ReadFn rd, uint16_t pattern6, int lo, const char* uid) {
    reset_dut(top);
    const uint16_t expected = static_cast<uint16_t>((pattern6 & 0x3F) << lo);
    uint16_t stim = expected;
#ifdef MUT_RANGEINV
    uint16_t inv = static_cast<uint16_t>((~pattern6) & 0x3F);
    stim = static_cast<uint16_t>(inv << lo);
#endif
    write_joytest(top, stim);
    uint16_t got = static_cast<uint16_t>(rd(top) & (0x3Fu << lo));
    expect(got == expected, uid);
}

// Per-bit "xx" cell -- DECLINED, not asserted. See the REPAIR note at the top
// of this file: p.282's write-test-data table defines no test-data value for
// this write-bit position, and no printed rule (checked: pp.227-241, the
// Appendix A preamble pp.263-264, pp.281-282, pp.287-289) requires an
// implementation to leave the corresponding counter bit unchanged when the
// position is written. "Undefined" is not "must be ignored". Declined
// claims take no mutation record. The clear/set readback is still measured
// and folded into the record() message below, since it is cheap and honest
// to show what this RTL actually does at the position even though nothing
// here grades it.
static void decline_undef_bit(Vuserio* top, ReadFn rd, const BitProp& p,
                               const char* dat_name, const char* half_name) {
    reset_dut(top);
    write_joytest(top, 0x0000);
    uint16_t c = rd(top);
    write_joytest(top, static_cast<uint16_t>(1u << p.wbit));
    uint16_t s = rd(top);
    char buf[640];
    std::snprintf(buf, sizeof buf,
        "%s: p.282's JOYTEST write-test-data table prints write bit %d of %s's %s-half as "
        "\"xx\" -- the page defines no test-data value for this position, and no other printed "
        "rule found (pp.227-241, appA preamble pp.263-264, pp.281-282, pp.287-289) requires an "
        "implementation to leave the counter bit unchanged when it is written. Declined as "
        "unspecified, not asserted. Measured for the record only: write $0000 -> $%04X, "
        "write bit-%d-set -> $%04X.",
        p.uid, p.wbit, dat_name, half_name, c, p.wbit, s);
    record(buf);
}

// Composed "xx" pair (sol) -- same decline as the two opus per-bit claims it
// composes, stated once for both bits together.
static void decline_undef_range(Vuserio* top, ReadFn rd, const BitProp* pair,
                                 const char* uid, const char* dat_name, const char* half_name) {
    reset_dut(top);
    write_joytest(top, 0x0000);
    uint16_t c = rd(top);
    uint16_t setv = static_cast<uint16_t>((1u << pair[0].wbit) | (1u << pair[1].wbit));
    write_joytest(top, setv);
    uint16_t s = rd(top);
    char buf[640];
    std::snprintf(buf, sizeof buf,
        "%s: p.282's JOYTEST write-test-data table prints write bits %d and %d of %s's %s-half "
        "as \"xx\" -- the page defines no test-data value at either position, and no other "
        "printed rule found (pp.227-241, appA preamble pp.263-264, pp.281-282, pp.287-289) "
        "requires an implementation to leave the counter bits unchanged when they are written. "
        "Same decline as the two opus per-bit claims this composes. Declined as unspecified, not "
        "asserted. Measured for the record only: write $0000 -> $%04X, both bits set -> $%04X.",
        uid, pair[0].wbit, pair[1].wbit, dat_name, half_name, c, s);
    record(buf);
}

// G0: one write reaches all four counter halves.
static void test_g0(Vuserio* top, const char* uid) {
    reset_dut(top);
    uint16_t pre0 = read_joy0dat(top);
    uint16_t pre1 = read_joy1dat(top);
    uint16_t stim = 0xAC34;  // distinct nonzero Y-field and X-field content
#ifndef MUT_ADDR
    write_joytest(top, stim);
#else
    top->reg_address_in = 0x00;  // ask the module to write somewhere else entirely
    top->data_in = stim;
    tick(top);
#endif
    uint16_t post0 = read_joy0dat(top);
    uint16_t post1 = read_joy1dat(top);
    bool y0 = (post0 & 0xFC00) != (pre0 & 0xFC00);
    bool x0 = (post0 & 0x00FC) != (pre0 & 0x00FC);
    bool y1 = (post1 & 0xFC00) != (pre1 & 0xFC00);
    bool x1 = (post1 & 0x00FC) != (pre1 & 0x00FC);
    expect(y0 && x0 && y1 && x1, uid);
}

// Fixture-delivery proof: the harness itself must be able to carry two
// distinct words into the DUT before any data-dependent assertion above is
// trusted. Not a manual claim -- no uid.
static void test_fixture_delivery(Vuserio* top) {
    reset_dut(top);
    uint16_t a = 0xAAAA;
    uint16_t b = 0x5555;
#ifdef MUT_FIXTURE
    b = a;  // ask the harness to repeat the same word instead of a different one
#endif
    write_joytest(top, a);
    uint16_t got_a = read_joy0dat(top);
    write_joytest(top, b);
    uint16_t got_b = read_joy0dat(top);
    expect(got_a != got_b, "fixture-delivers-distinct-words");
}

// ---------------------------------------------------------------------------
int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vuserio* top = new Vuserio;

    // Fixture proof first, before anything data-dependent is trusted.
    test_fixture_delivery(top);

    // G0 -- one write reaches all four counters.
    test_g0(top, "opus:8-appa-joytest.c4");
    test_g0(top, "sol:8-appa-joytest.c4");

    // A directly measured, undisguised look at the asymmetry the task
    // description named: same $FFFF write, two different results.
    {
        reset_dut(top);
        write_joytest(top, 0xFFFF);
        uint16_t j0 = read_joy0dat(top);
        uint16_t j1 = read_joy1dat(top);
        char buf[192];
        std::snprintf(buf, sizeof buf,
                       "JOYTEST <= $FFFF -> JOY0DAT=$%04X JOY1DAT=$%04X (0DAT's xx-bits "
                       "forced 0 by the same edge that clears dmouse0dat; 1DAT's are left as they were)",
                       j0, j1);
        note(buf);
    }

    // 0DAT / Y-half: bits 15..10 -> Y7..Y2, and the composed range claim.
    for (int i = 0; i < arrlen(k0DatY); i++) test_data_bit(top, read_joy0dat, k0DatY[i]);
    test_range(top, read_joy0dat, 0b101101, 10, "sol:8-appa-joytest.c5");

    // 0DAT / Y-half: bits 9,8 are "xx" in the table -- declined as unspecified,
    // not asserted (see REPAIR note, top of file; validation finding 10).
    for (int i = 0; i < arrlen(k0DatYUndef); i++)
        decline_undef_bit(top, read_joy0dat, k0DatYUndef[i], "0DAT", "Y");
    decline_undef_range(top, read_joy0dat, k0DatYUndef, "sol:8-appa-joytest.c6", "0DAT", "Y");

    // 0DAT / X-half: bits 7..2 -> X7..X2, and the composed range claim.
    for (int i = 0; i < arrlen(k0DatX); i++) test_data_bit(top, read_joy0dat, k0DatX[i]);
    test_range(top, read_joy0dat, 0b011010, 2, "sol:8-appa-joytest.c7");

    // 0DAT / X-half: bits 1,0 are "xx" in the table -- declined as unspecified.
    for (int i = 0; i < arrlen(k0DatXUndef); i++)
        decline_undef_bit(top, read_joy0dat, k0DatXUndef[i], "0DAT", "X");
    decline_undef_range(top, read_joy0dat, k0DatXUndef, "sol:8-appa-joytest.c8", "0DAT", "X");

    // 1DAT / Y-half: bits 15..10 -> Y7..Y2, and the composed range claim.
    // Graded independently of 0DAT throughout -- nothing here assumes the
    // two ports behave the same way.
    for (int i = 0; i < arrlen(k1DatY); i++) test_data_bit(top, read_joy1dat, k1DatY[i]);
    test_range(top, read_joy1dat, 0b101101, 10, "sol:8-appa-joytest.c9");

    // 1DAT / Y-half: bits 9,8 are "xx" in the table -- declined as unspecified.
    for (int i = 0; i < arrlen(k1DatYUndef); i++)
        decline_undef_bit(top, read_joy1dat, k1DatYUndef[i], "1DAT", "Y");
    decline_undef_range(top, read_joy1dat, k1DatYUndef, "sol:8-appa-joytest.c10", "1DAT", "Y");

    // 1DAT / X-half: bits 7..2 -> X7..X2, and the composed range claim.
    for (int i = 0; i < arrlen(k1DatX); i++) test_data_bit(top, read_joy1dat, k1DatX[i]);
    test_range(top, read_joy1dat, 0b011010, 2, "sol:8-appa-joytest.c11");

    // 1DAT / X-half: bits 1,0 are "xx" in the table -- declined as unspecified.
    for (int i = 0; i < arrlen(k1DatXUndef); i++)
        decline_undef_bit(top, read_joy1dat, k1DatXUndef[i], "1DAT", "X");
    decline_undef_range(top, read_joy1dat, k1DatXUndef, "sol:8-appa-joytest.c12", "1DAT", "X");

    std::cout << checks.passed << " passed, " << checks.failed << " failed\n";

    delete top;
    return checks.failed == 0 ? 0 : 1;
}
