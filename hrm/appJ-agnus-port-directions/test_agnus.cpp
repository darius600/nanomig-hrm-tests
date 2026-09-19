// appJ-agnus-port-directions -- agnus conformance test
//
// Test purpose: whether the DEFINITION column (I / O / I/O -- the direction
//   of each pin AT THE CHIP) of p.379's ORIGINAL AGNUS pin list and p.381's
//   FAT AGNUS pin list matches the direction of the Verilog port `agnus`
//   itself declares for the identified signal. Package
//   appJ-agnus-port-directions, fixture `agnus` (top module, plus every
//   submodule it instantiates -- see the Makefile's SRC list, model
//   hrm/ch1-agnus-what-it-contains, the only other package in hrm/ that tops
//   the full `agnus` with MUT= pass-through). 36 claims, filtered from
//   hrm-testpipeline/work/2026-09-17_appHJ_packages.jsonl on
//   classify.package == "appJ-agnus-port-directions".
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 379 (ORIGINAL AGNUS) and 381 (FAT AGNUS). The axis is exactly the
//   one hrm/appDE-ciab-port-signals graded for the CIA (its own expect()
//   label: "at ciab's own boundary, PA7 and PA6 are both DRIVEN OUTPUT bits
//   -- `porta_out` is declared `output [7:6]` (ciab.v:20)... a Verilog
//   port-direction fact, not a runtime-configurable one"); this package's
//   own appJ-pinlist/corpus-defects.md names that package as the precedent
//   for constructing the DEFINITION column here. The package-pin-NUMBER half
//   of every printed row was classified `not-hardware` at stage 3/4 and is
//   not in this package's 36 -- NanoMig has no package.
// Non-goals: anything outside the `agnus` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appHJ_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// THE STAGE-5 MERGE, executed here: p.379 and p.381 describe two chip
//   generations of the SAME module (`agnus` -- NanoMig implements one RTL
//   module for both, per the coordinator's own stage-5 decision). Where
//   both pages state the same proposition, it is graded ONCE below, citing
//   both pages' uids on that one line -- not built as two assertions for
//   one fact. SIX of the nine p.379 propositions recur on p.381 this way:
//   reset (RES*/RST*), int3 (INT3*/INT3*), dmal (DMAL/DMAL), bls
//   (BLS*/BLS*), dbr (DBR*/DBR*) and dbwe (ARW*/RRW). That is ONE more than
//   this package's own build task's headline "five of the nine" -- checked,
//   not silently followed: ARW*(p.379)/RRW(p.381) are treated here as the
//   SAME proposition (Agnus's own memory-write strobe is an output) under
//   two different printed names, by the IDENTICAL reasoning the build task
//   itself applies to RES*/RST* (a naming difference across generations
//   that does not reach the direction claim) -- see DBWE-IS-OUTPUT below,
//   which states this explicitly rather than assuming the "five" figure
//   forecloses it. The remaining three p.379-only propositions collapse
//   further: D8-D0 and D15-D9 are two printed ROWS (a package pin-range
//   split, 01-09 and 42-48) for the SAME pair of RTL ports (`data_in`/
//   `data_out`) on the SAME page -- also graded once, not twice. The other
//   two p.379-only propositions (CSY*, and the p.381-only trio RGEN*/28MHZ/
//   NTSC-PAL) have no cross-page counterpart in this claim set.
//
// CLAIM COVERAGE, all 36 accounted for below (uid counts: 6 shared-signal
//   groups x 4 + 1 same-page-merged group x 4 + 3 p.381-only groups x 2 + 1
//   declined group x 2 = 24 + 4 + 6 + 2 = 36):
//   DATA-BUS-IS-BIDIRECTIONAL  sol:J-004 opus:AHJ-135 sol:J-040 opus:AHJ-171
//   RESET-IS-INPUT             sol:J-008 opus:AHJ-139 sol:J-140 opus:AHJ-274
//   AEN-IS-INPUT               sol:J-150 opus:AHJ-284
//   DBR-IS-OUTPUT              sol:J-016 opus:AHJ-147 sol:J-144 opus:AHJ-278
//   DBWE-IS-OUTPUT             sol:J-018 opus:AHJ-149 sol:J-146 opus:AHJ-280
//   INT3-IS-OUTPUT             sol:J-010 opus:AHJ-141 sol:J-134 opus:AHJ-268
//   DMAL-IS-INPUT              sol:J-012 opus:AHJ-143 sol:J-136 opus:AHJ-270
//   BLS-IS-INPUT               sol:J-014 opus:AHJ-145 sol:J-142 opus:AHJ-276
//   CLK-IS-INPUT               sol:J-158 opus:AHJ-292
//   NTSC-IS-INPUT              sol:J-174 opus:AHJ-308
//   DECLINED (CSY*)            sol:J-034 opus:AHJ-165
//
// THE PORT LIST THE BUILD TASK HANDED DOWN WAS NOT EXHAUSTIVE, checked: it
//   named 9 signals (clk, reset, aen, dbr, dbwe, int3, audio_dmal,
//   disk_dmal, bls, ntsc, all verified at the cited agnus.v line numbers --
//   confirmed correct, every line number matches) but the 36 claims'
//   classify.evidence fields also cite `data_in`/`data_out` (agnus.v:64-65)
//   and `_csync` (agnus.v:75), neither in that list. Both were resolved
//   independently: data_in/data_out has no prior assertion anywhere in
//   hrm/ grading its direction (grep across every non-obj_dir *.cpp in the
//   suite for data_in/data_out/D8-D0/D15-D9/DATA-BUS found nothing on this
//   axis) and is graded fresh, below. `_csync` DOES have one -- see the
//   DECLINED block. This is worth flagging precisely because the omission
//   could be read either way (accidental, or a hint to decline); it turned
//   out to be one of each.
//
// TWO IDENTIFICATIONS CHECKED PER THE BUILD TASK'S OWN INSTRUCTION, not
//   taken on faith:
//   - RRW/ARW* -> `dbwe`: `grep -ri arw` and `grep -ri rrw` over
//     nanomig/src/minimig-aga and nanomig/src/misc (independently re-run
//     for this build, not merely quoted from classify.evidence) return
//     ZERO hits for both. Neither printed name occurs anywhere in the RTL;
//     the binding rests on two RTL comments (agnus.v:71, minimig.v:333)
//     plus the FUNCTION gloss. The ENCODING differs (RRW reads as one
//     level line, write in one state and read in the other; `dbwe` is a
//     plain write-enable). CONCLUSION: the DIRECTION axis survives both
//     facts -- whatever encodes it, Agnus drives it -- and DBWE-IS-OUTPUT
//     below grades exactly that, stating the weak identity in its own
//     label rather than hiding it. Kept, not demoted: unlike CLK below,
//     a genuine stimulus mutation exists (drive a read DMA instead of a
//     write DMA), so there is no vacuity question here.
//   - 28MHZ -> `clk`: THINNEST claim in this package. CONCLUSION: its
//     mutation IS vacuous -- freezing `clk` would be tautologically true
//     by Verilog's own clocked-register semantics, testing the language
//     rather than this RTL -- so it is demoted from a `stimulus red`
//     mutation to `none impossible`, paired with a positive-control
//     liveness measurement, exactly as this package's build task
//     instructed and as hrm/appDE-fire-buttons-userio models. See
//     CLK-IS-INPUT below for the argument in full.
//
// THE DMAL COUNT SUBTLETY the build task named and this file does not
//   rediscover, only confirms and records: p.379/p.381 each print ONE DMAL
//   row ("DMA request line" / "Request audio/disk DMA"). NanoMig implements
//   FIVE separate request wires: `audio_dmal[3:0]` (agnus.v:90, 4 bits) and
//   `disk_dmal` (agnus.v:92, 1 bit). "DMAL is an input" holds for all five
//   and is what DMAL-IS-INPUT grades; "DMAL is one line" does not hold and
//   is recorded, not asserted either way -- see the record() immediately
//   after that assertion.
//
// NOT IN THIS PACKAGE'S 36, CONFIRMED BY READING EVERY ONE OF THE 36
//   CLAIM RECORDS DIRECTLY (not merely trusting the build task's say-so):
//   `LP*`, `CCK`, `CCKQ` and `RGA8-RGA1` -- the appendix contradicts itself
//   on all four across its two Agnus tables (corpus-defects.md's own
//   "Where this appendix disagrees with appendix H" section, and its
//   direction-reversal note for CCK/CCKQ/RGA8-RGA1/LP*) and none of the 36
//   claim records filtered into this package names any of them. If one
//   ever does, that is a stage-3/4 classification defect to report, not a
//   row to grade -- none was found here.
//
// CORPUS DEFECTS READ FOR THIS BUILD (appJ-pinlist/corpus-defects.md,
//   fourth version) and how each claim's own quote was verified against
//   them, not merely trusted:
//   - p.379's FUNCTION column is column-displaced against the PIN column,
//     by an offset that GROWS BY ONE PER ROW (not a fixed two, an earlier
//     version of that note's own error) -- corpus-defects.md's corrected
//     table (independently reproduced there from figures/p379.png) reads:
//       01-09 D8-D0  "Data bus lines 8 to 0"    I/O
//       11    RES*   "System reset"              I
//       12    INT3*  "Interrupt level 3"          O
//       13    DMAL   "DMA request line"           I
//       14    BLS*   "Blitter slowdown"           I
//       15    DBR*   "Data bus request"           O
//       16    ARW*   "Agnus RAM write"             O
//     and pin 39 CSY* / "Composite sync" / O, pin 42-48 D15-D9 / "Data bus
//     lines 15 to 9" / I/O further down -- exactly the quotes each p.379
//     claim below carries. This file trusts those corrected quotes, not
//     the raw displaced text layer.
//   - p.381's FAT AGNUS table is NOT column-displaced (corpus-defects.md:
//     "Its rows bind correctly. What is damaged is characters."). Every
//     p.381 claim's own classify.evidence already states it was "VERIFIED
//     against figures/p381.png directly, not taken from the text layer or
//     from any note" -- trusted as read, since the corpus note independently
//     confirms this page's column integrity.
//   - THE LETTER-O/DIGIT-0 CONFUSION runs in both directions and, in the
//     DEFINITION column specifically, is "the one that matters for a
//     direction claim" (corpus-defects.md). Every direction value this
//     file cites (I/O/I/O) was read from a claim record already marked
//     figure-verified against this exact risk, not re-derived from the
//     text layer.
//   - Pin 18 on p.381 carries THREE rows (DMAL, RD1, RST*) and pins 15,
//     16, 42, 58, 82, 83 carry none -- a print defect, not a scan one; none
//     of this package's claims depend on pin NUMBER at all, so this does
//     not touch any assertion below.
//
// RTL read in full before this file was drafted: agnus.v (the whole
//   module), plus the relevant fragments of agnus_diskdma.v, agnus_blitter.v
//   and agnus_beamcounter.v cited inline below. hrm/ch1-agnus-what-it-
//   contains/test_agnus.cpp and hrm/appDE-ciab-port-signals/
//   test_appDE-ciab-port-signals.cpp were read in full as this package's
//   two named models (Harness/register-bus plumbing from the first, the
//   port-direction-via-behavioural-proxy grading shape from the second).
//   hrm/ch8-video-signal-set/test_minimig.cpp was read in full for the
//   DECLINED block below and for `record()`'s "structural, compile-time
//   fact no runtime stimulus can change" language this package's own build
//   task quotes. hrm/appDE-fire-buttons-userio/test_userio.cpp was read in
//   full as the `none impossible` + positive-control model for CLK-IS-INPUT.
//
// A PORT-DIRECTION FACT IS COMPILE-TIME, NOT RUNTIME, and every mutation
//   below respects that: no assertion "mutates the direction" (that would
//   require editing agnus.v, out of scope -- this package builds the
//   unmodified pinned submodule). Each instead mutates the STIMULUS around
//   a behavioural proxy for that direction -- an input is asserted and
//   Agnus's internal state is observed to respond (or a register write is
//   blocked/allowed by it); an output is driven by Agnus's own internal
//   state and is observed doing so -- exactly the shape
//   hrm/appDE-ciab-port-signals uses for the CIA. The one exception is
//   CLK-IS-INPUT, where even that proxy has no falsifiable stimulus
//   mutation (see above); it is graded `none impossible` instead.
//
// MUTATION METHODOLOGY: every expect() below is guarded by a compile-time
//   `-D` flag (or, for CLK-IS-INPUT, none at all). Every mutation was
//   actually built and run (WSL Verilator, `make test MUT=<NAME>`, full
//   `rm -rf obj_dir` rebuild each time) -- see the inline `// MUTATION`
//   comment immediately after each assertion.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <unordered_map>
#include "Vagnus.h"
#include "verilated.h"

#ifndef MUT_DATABUS_SKIP_WRITE
#define MUT_DATABUS_SKIP_WRITE 0
#endif
#ifndef MUT_RESET_NEVER_ASSERTED
#define MUT_RESET_NEVER_ASSERTED 0
#endif
#ifndef MUT_AEN_ALWAYS_HIGH
#define MUT_AEN_ALWAYS_HIGH 0
#endif
#ifndef MUT_DBR_WITHHOLD_DMAEN
#define MUT_DBR_WITHHOLD_DMAEN 0
#endif
#ifndef MUT_DBWE_READ_INSTEAD
#define MUT_DBWE_READ_INSTEAD 0
#endif
#ifndef MUT_INT3_NO_BLTEN
#define MUT_INT3_NO_BLTEN 0
#endif
#ifndef MUT_DMAL_WITHHELD
#define MUT_DMAL_WITHHELD 0
#endif
#ifndef MUT_BLS_NEVER_ASSERTED
#define MUT_BLS_NEVER_ASSERTED 0
#endif
#ifndef MUT_NTSC_SAME
#define MUT_NTSC_SAME 0
#endif

namespace {

// Register byte addresses (agnus.v / submodule parameter tables), reused
// verbatim from hrm/ch1-agnus-what-it-contains's own constants -- the same
// register bank, the same fixture boundary.
constexpr uint32_t A_DMACON  = 0x096;
constexpr uint32_t A_DMACONR = 0x002;
constexpr uint32_t A_DSKPTH  = 0x020;
constexpr uint32_t A_DSKPTL  = 0x022;
constexpr uint32_t A_AUD0LCH = 0x0A0;
constexpr uint32_t A_AUD0LCL = 0x0A2;
constexpr uint32_t A_BLTCON0 = 0x040;
constexpr uint32_t A_BLTCON1 = 0x042;
constexpr uint32_t A_BLTSIZE = 0x058;
constexpr uint32_t A_BLTDPTH = 0x054;
constexpr uint32_t A_BLTDPTL = 0x056;

constexpr uint16_t DMAEN_BIT = 9;
constexpr uint16_t BPLEN_BIT = 8;
constexpr uint16_t BLTEN_BIT = 6;

// Reproduces amiga_clk.v's own clk7_en/cck division. Identical scheme used
// by several sibling agnus-boundary packages (hrm/ch1-agnus-what-it-
// contains's own ClkGen, hrm/ch8-video-signal-set's). agnus needs only
// clk7_en and cck.
struct ClkGen {
    uint8_t clk7_cnt = 2; // amiga_clk.v reset value 2'b10
    bool clk7_en = true, cck = true;
    void step() {
        uint8_t old_cnt = clk7_cnt;
        bool old_cck = cck;
        clk7_cnt = (old_cnt + 1) & 0x3;
        clk7_en = (old_cnt == 0);
        if (old_cnt == 1) cck = !old_cck; else cck = old_cck;
    }
};

// Harness: register-bus plumbing reused from hrm/ch1-agnus-what-it-contains
// (this package's named model), extended with an `ntsc_init` constructor
// parameter (NTSC-IS-INPUT needs `ntsc` held stable through the initial
// reset, since agnus_beamcounter.v:171 samples it only `if (reset)`) and an
// `aen_high` parameter on write_reg/read_reg (AEN-IS-INPUT needs to drive
// `aen` itself, not have it silently forced high by the helper).
class Harness {
public:
    Vagnus* dut;
    ClkGen cg;
    long long tickcount = 0;
    std::unordered_map<uint32_t, uint16_t> mem;

    explicit Harness(bool ntsc_init = false) : dut(new Vagnus) {
        dut->clk = 0;
        dut->clk7_en = 1; dut->cck = 1;
        dut->reset = 1;
        dut->aen = 0; dut->rd = 0; dut->hwr = 0; dut->lwr = 0;
        dut->data_in = 0; dut->address_in = 0;
        dut->audio_dmal = 0; dut->audio_dmas = 0;
        dut->disk_dmal = 0; dut->disk_dmas = 0;
        dut->bls = 0;
        dut->ntsc = ntsc_init ? 1 : 0;
        dut->a1k = 0; dut->ecs = 0; dut->aga = 0;
        dut->floppy_speed = 1;
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
    }
    ~Harness() { delete dut; }

    uint16_t mem_read(uint32_t a) const {
        auto it = mem.find(a);
        return it == mem.end() ? 0 : it->second;
    }

    void tick() {
        if (!cpu_driving) {
            dut->eval();
            if (dut->dbr) dut->data_in = mem_read(dut->address_out);
        }
        dut->eval();
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
        cg.step();
        dut->clk7_en = cg.clk7_en;
        dut->cck = cg.cck;
        ++tickcount;
    }

    void write_reg(uint32_t byte_addr, uint16_t data, bool aen_high = true,
                    bool hi = true, bool lo = true) {
        cpu_driving = true;
        dut->aen = aen_high ? 1 : 0;
        dut->rd = 0;
        dut->hwr = hi ? 1 : 0; dut->lwr = lo ? 1 : 0;
        dut->address_in = (byte_addr >> 1) & 0xFF;
        dut->data_in = data;
        for (int i = 0; i < 32; ++i) tick();
        dut->aen = 0; dut->rd = 0; dut->hwr = 0; dut->lwr = 0;
        cpu_driving = false;
        tick();
    }

    uint16_t read_reg(uint32_t byte_addr, bool aen_high = true) {
        cpu_driving = true;
        dut->aen = aen_high ? 1 : 0;
        dut->rd = 1; dut->hwr = 0; dut->lwr = 0;
        dut->address_in = (byte_addr >> 1) & 0xFF;
        dut->eval();
        uint16_t val = dut->data_out;
        for (int i = 0; i < 32; ++i) tick();
        dut->aen = 0; dut->rd = 0;
        cpu_driving = false;
        tick();
        return val;
    }

    void dmacon_set(uint16_t bits) { write_reg(A_DMACON, 0x8000 | bits); }

    bool cpu_driving = false;
};

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

} // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // =========================================================================
    // DATA-BUS-IS-BIDIRECTIONAL (p.379 pins 01-09 D8-D0 / 42-48 D15-D9,
    // graded once -- see the file header's merge note).
    // =========================================================================
    {
        Harness h;
#if !MUT_DATABUS_SKIP_WRITE
        h.dmacon_set(1 << BPLEN_BIT);
#endif
        uint16_t readback = h.read_reg(A_DMACONR);
        bool bit_present = (readback & (1 << BPLEN_BIT)) != 0;
        char buf[64];
        std::snprintf(buf, sizeof buf, "DMACONR readback=$%04X (bit%d expected set)",
                      readback, BPLEN_BIT);
        checks.note(std::string("DATA-BUS round trip: ") + buf);
        checks.expect(bit_present,
            "DATA-BUS-IS-BIDIRECTIONAL (p.379 pins 01-09 D8-D0 \"Data bus "
            "lines 8 to 0\" I/O and 42-48 D15-D9 \"Data bus lines 15 to 9\" "
            "I/O -- one printed 16-bit bidirectional bus, two rows only "
            "because of the package's own pin-range split, graded once): "
            "at agnus's own boundary this is `data_in[15:0]` (agnus.v:64, "
            "input) and `data_out[15:0]` (agnus.v:65, output). A CPU "
            "register write of DMACON's BPLEN bit through data_in is read "
            "back set on data_out's DMACONR mirror -- one round trip that "
            "exercises the inward and the outward direction. The pin-range "
            "split (D8-D0 on pins 01-09, D15-D9 on 42-48) has no RTL "
            "counterpart; both printed rows land on this one pair of ports. "
            "sol:J-004 opus:AHJ-135 sol:J-040 opus:AHJ-171");
    }
    // MUTATION DATA-BUS-IS-BIDIRECTIONAL stimulus red MUT_DATABUS_SKIP_WRITE: the DMACON write is skipped entirely -- dmacon's BPLEN bit never sets, so DMACONR's readback stays 0 and the round-trip check goes red.

    // =========================================================================
    // RESET-IS-INPUT (p.379 pin 11 RES*; p.381 pin 18 RST* -- same signal,
    // naming differs across generations, direction does not, graded once).
    // =========================================================================
    {
        Harness h;
        h.dmacon_set((1 << DMAEN_BIT) | (1 << BPLEN_BIT));
        uint16_t before = h.read_reg(A_DMACONR);
#if !MUT_RESET_NEVER_ASSERTED
        h.dut->reset = 1;
        for (int i = 0; i < 16; ++i) h.tick();
        h.dut->reset = 0;
        for (int i = 0; i < 16; ++i) h.tick();
#endif
        uint16_t after = h.read_reg(A_DMACONR);
        bool before_nonzero = (before & 0x1FFF) != 0;
        bool after_cleared = (after & 0x1FFF) == 0;
        char buf[96];
        std::snprintf(buf, sizeof buf, "before=$%04X after=$%04X", before, after);
        checks.note(std::string("RESET round trip: ") + buf);
        checks.expect(before_nonzero && after_cleared,
            "RESET-IS-INPUT (p.379 pin 11 RES* \"System reset\" I; p.381 "
            "pin 18 RST* \"Reset\" I -- same signal, naming differs across "
            "generations, direction does not, graded once): `reset` "
            "(agnus.v:59, input) is driven at minimig.v:444 from the "
            "system reset. A DMACON write is first confirmed to have "
            "landed (DMACONR nonzero); reset is then asserted and released "
            "with the clock still running, and DMACONR is confirmed back "
            "at its reset value (agnus.v:288 `if (reset) dmacon <= 0`) "
            "with no explicit clearing write -- reset alone did it. "
            "POLARITY NOT GRADED: the page's own NOTE reads the printed "
            "`*` as active-low; agnus.v's `reset` port is active-high. "
            "Different axis, not asserted here. sol:J-008 opus:AHJ-139 "
            "sol:J-140 opus:AHJ-274");
    }
    // MUTATION RESET-IS-INPUT stimulus red MUT_RESET_NEVER_ASSERTED: reset is never asserted after the DMACON write -- DMACONR stays at its written, nonzero value instead of returning to 0, so the after_cleared conjunct goes red.

    // =========================================================================
    // AEN-IS-INPUT (p.381 pin 23 RGEN* only -- no p.379 counterpart).
    // =========================================================================
    {
        Harness h;
#if !MUT_AEN_ALWAYS_HIGH
        h.write_reg(A_DMACON, 0x8000 | (1 << DMAEN_BIT), /*aen_high=*/false);
#else
        // MUTATION: the "blocked" phase is also issued with aen already
        // high, so it lands too, instead of staying blocked.
        h.write_reg(A_DMACON, 0x8000 | (1 << DMAEN_BIT), /*aen_high=*/true);
#endif
        uint16_t after_low = h.read_reg(A_DMACONR);
        h.write_reg(A_DMACON, 0x8000 | (1 << DMAEN_BIT), /*aen_high=*/true);
        uint16_t after_high = h.read_reg(A_DMACONR);
        bool blocked_when_low = (after_low & (1 << DMAEN_BIT)) == 0;
        bool lands_when_high = (after_high & (1 << DMAEN_BIT)) != 0;
        char buf[96];
        std::snprintf(buf, sizeof buf, "after_low=$%04X after_high=$%04X", after_low, after_high);
        checks.note(std::string("AEN gate: ") + buf);
        checks.expect(blocked_when_low && lands_when_high,
            "AEN-IS-INPUT (p.381 pin 23 RGEN* \"RG Enable\" I; NAMING "
            "ARGUED, not assumed -- p.381 prints no signal named `aen`, "
            "the identity rests on the FUNCTION column \"RG Enable\" "
            "against agnus.v:60's own \"(register bank)\" comment and "
            "gary's `sel_reg` (\"select chip register bank\"), driven onto "
            "`aen` at minimig.v:446): `aen` (agnus.v:60, input) gates the "
            "register address decoder -- `reg_address_cpu = (aen&(rd|hwr|"
            "lwr)) ? address_in : 8'hFF` (agnus.v:114). A DMACON write "
            "issued with aen held low does NOT land (DMACONR unchanged); "
            "the identical write issued with aen driven high DOES land, "
            "on the same Harness. sol:J-150 opus:AHJ-284");
    }
    // MUTATION AEN-IS-INPUT stimulus red MUT_AEN_ALWAYS_HIGH: the first (supposedly-blocked) write is also issued with aen already high -- it lands too, so DMAEN is already set before the low-aen phase is even checked, and the blocked_when_low conjunct goes red.

    // =========================================================================
    // DBR-IS-OUTPUT (p.379 pin 15 DBR*; p.381 pin 20 DBR* -- same name,
    // same direction, graded once).
    // =========================================================================
    {
        // Measured while designing this assertion, and recorded rather
        // than silently worked around: disk DMA's own grant (`dma_dsk`,
        // wired straight from agnus_diskdma's `dma = dmal & dmaslot &
        // hpos[0]`, agnus_diskdma.v:49) has NO DMACON dependency at all --
        // unlike bitplane/copper/blitter/sprite (agnus.v:299-302, each
        // `... & dmacon[9]`). A first draft of this assertion tried to
        // falsify it by withholding DMAEN with disk_dmal held, built and
        // ran it (`make test MUT=MUT_DBR_WITHHOLD_DMAEN`), and got 10
        // passed / 0 failed -- the mutation was vacuous, not the
        // assertion wrong. Rebuilt on the blitter channel instead, where
        // `blten = dmacon[6] & dmacon[9]` (agnus.v:301) is a real gate.
        Harness h;
        const uint32_t DEST = 0x00500;
        h.write_reg(A_BLTCON1, 0x0000);
        h.write_reg(A_BLTDPTH, (uint16_t)((DEST >> 15) & 0x1F));
        h.write_reg(A_BLTDPTL, (uint16_t)((DEST & 0x7FFF) << 1));
        h.write_reg(A_BLTCON0, 0x01FF); // used=1 only, LF=$FF: channel-D-only, no source data needed
#if !MUT_DBR_WITHHOLD_DMAEN
        h.dmacon_set((1 << BLTEN_BIT) | (1 << DMAEN_BIT));
#endif
        h.write_reg(A_BLTSIZE, 0x0041); // height=1, width=1
        // Checked via address_out == DEST, not merely "dbr && !cpu_custom"
        // -- refresh DMA (agnus_refresh.v) fires unconditionally, with no
        // enable of its own, presents address_out==0 on its own granted
        // cycles, and would otherwise satisfy a generic dbr check even
        // with the blitter fully disabled, making the mutation below a
        // no-op (measured: an earlier draft of this assertion used the
        // generic check and MUT_DBR_WITHHOLD_DMAEN still came out green --
        // fixed here to the same address-qualified check
        // hrm/ch1-agnus-what-it-contains's own SPECIAL-PURPOSE-HARDWARE-
        // TAKES-BUS-FROM-PROCESSOR assertion uses for the same reason).
        bool seen_dbr = false;
        for (int i = 0; i < 8000 && !seen_dbr; ++i) {
            h.tick();
            if (h.dut->dbr && h.dut->address_out == DEST) seen_dbr = true;
        }
        checks.note(std::string("DBR grant: ") + (seen_dbr ? "observed at DEST" : "never observed at DEST") +
                    " over an 8000-tick window after a minimal 1x1 channel-D blit was armed.");
        checks.expect(seen_dbr,
            "DBR-IS-OUTPUT (p.379 pin 15 DBR* \"Data bus request\" O; "
            "p.381 pin 20 DBR* \"Data bus request\" O -- same name, same "
            "direction, graded once): `dbr` (agnus.v:70, `output reg`, "
            "\"agnus requests data bus\") is consumed by gary at "
            "minimig.v:456. Graded via a minimal channel-D-only blit "
            "(DMACON BLTEN+DMAEN, `blten = dmacon[6] & dmacon[9]`, "
            "agnus.v:301) rather than disk DMA -- disk's own grant has no "
            "DMACON dependency at all (agnus_diskdma.v:49, measured while "
            "designing this assertion, see the comment above) and so gave "
            "no falsifiable lever here; DMAL-IS-INPUT below still grades "
            "disk_dmal on its own, distinct lever. dbr rises with "
            "address_out at the blit's own destination pointer -- a real, "
            "driven output that reports agnus's own internal grant state, "
            "not the unconditional refresh grant that would otherwise "
            "satisfy a generic dbr check regardless of the blitter. "
            "sol:J-016 opus:AHJ-147 sol:J-144 opus:AHJ-278");
    }
    // MUTATION DBR-IS-OUTPUT stimulus red MUT_DBR_WITHHOLD_DMAEN: DMACON's BLTEN/DMAEN bits are left clear -- `blten` (agnus.v:301) never asserts, the blitter never wins a grant, dbr never rises at the destination address (only, still, at refresh's own address 0), and seen_dbr goes red.

    // =========================================================================
    // DBWE-IS-OUTPUT (p.379 pin 16 ARW*; p.381 pin 21 RRW -- WEAKEST
    // identity in this package, checked, not assumed; see the file header).
    // =========================================================================
    {
        Harness h;
        const uint32_t PTR = 0x00600;
        h.write_reg(A_DSKPTH, (uint16_t)((PTR >> 15) & 0x1F));
        h.write_reg(A_DSKPTL, (uint16_t)((PTR & 0x7FFF) << 1));
        h.dmacon_set(1 << DMAEN_BIT);
        h.dut->disk_dmal = 1;
#if !MUT_DBWE_READ_INSTEAD
        h.dut->disk_dmas = 0; // wr_dsk = ~dmas = 1: a memory WRITE cycle
#else
        h.dut->disk_dmas = 1; // MUTATION: wr_dsk = 0, a memory READ cycle
#endif
        bool saw_write = false;
        for (int i = 0; i < 4000 && !saw_write; ++i) {
            h.tick();
            if (h.dut->dbr && h.dut->dbwe && !h.dut->cpu_custom) saw_write = true;
        }
        h.dut->disk_dmal = 0;
        checks.note(std::string("DBWE observed: ") +
                    (saw_write ? "asserted during the granted disk cycle" : "never asserted"));
        checks.expect(saw_write,
            "DBWE-IS-OUTPUT (p.379 pin 16 ARW* \"Agnus RAM write\" O; "
            "p.381 pin 21 RRW \"DRAM Write/Read\" O -- WEAKEST identity in "
            "this package, checked rather than assumed: `grep -ri arw` "
            "and `grep -ri rrw` over nanomig/src/minimig-aga and "
            "nanomig/src/misc, independently re-run for this build, "
            "return ZERO hits for both -- neither printed name occurs "
            "anywhere in the RTL, and the binding to `dbwe` rests on two "
            "RTL comments (agnus.v:71 \"agnus does a memory write cycle\", "
            "minimig.v:333 \"data bus write enable, Agnus tells the RAM "
            "it's writing data\") plus the FUNCTION gloss, not a name "
            "match. The ENCODING differs too: p.381's RRW reads as one "
            "level line, write in one state and read in the other; "
            "`dbwe` is a plain write-enable, simply low on reads. The "
            "DIRECTION axis survives both facts -- whatever encodes it, "
            "Agnus drives it -- and that is all this assertion grades): "
            "`dbwe` (agnus.v:71, `output reg`) asserts during a real "
            "disk-DMA WRITE cycle (disk_dmas=0, wr_dsk=1), observed high "
            "alongside dbr on a cpu_custom-low, agnus-granted cycle. "
            "sol:J-018 opus:AHJ-149 sol:J-146 opus:AHJ-280");
    }
    // MUTATION DBWE-IS-OUTPUT stimulus red MUT_DBWE_READ_INSTEAD: disk_dmas is driven 1 instead of 0 -- wr_dsk becomes 0 (a read cycle), so dbwe never asserts during the granted cycle and saw_write goes red.

    // =========================================================================
    // INT3-IS-OUTPUT (p.379 pin 12 INT3*; p.381 pin 17 INT3* -- same name,
    // same direction, graded once).
    // =========================================================================
    {
        Harness h;
        const uint32_t DEST = 0x00700;
        h.write_reg(A_BLTCON1, 0x0000);
        h.write_reg(A_BLTDPTH, (uint16_t)((DEST >> 15) & 0x1F));
        h.write_reg(A_BLTDPTL, (uint16_t)((DEST & 0x7FFF) << 1));
        h.write_reg(A_BLTCON0, 0x01FF); // used=1 only, LF=$FF: channel-D-only, no source data needed
#if !MUT_INT3_NO_BLTEN
        h.dmacon_set((1 << BLTEN_BIT) | (1 << DMAEN_BIT));
#endif
        h.write_reg(A_BLTSIZE, 0x0041); // height=1, width=1
        bool saw_int3 = false;
        for (int i = 0; i < 8000; ++i) {
            h.tick();
            if (h.dut->int3) saw_int3 = true;
        }
        checks.note(std::string("INT3 observed: ") +
                    (saw_int3 ? "fired" : "never fired") +
                    " over an 8000-tick window after a minimal 1x1 channel-D blit was armed.");
        checks.expect(saw_int3,
            "INT3-IS-OUTPUT (p.379 pin 12 INT3* \"Interrupt level 3\" O; "
            "p.381 pin 17 INT3* \"Blitter ready interrupt\" O -- same "
            "name, same direction, graded once): `int3` (agnus.v:89, "
            "output, \"blitter finished interrupt (to Paula)\") is wired "
            "at minimig.v:475 to the net Paula's own int3 input consumes "
            "(minimig.v:511, graded separately at "
            "hrm/paula_intcontroller, not here). A minimal channel-D-only "
            "blit (BLTCON0 LF=$FF, no source channels) is armed and run "
            "to completion; int3 fires. sol:J-010 opus:AHJ-141 sol:J-134 "
            "opus:AHJ-268");
    }
    // MUTATION INT3-IS-OUTPUT stimulus red MUT_INT3_NO_BLTEN: DMACON's BLTEN bit is left clear -- the blitter never wins a DMA grant, so it never runs to completion and int3 never fires.

    // =========================================================================
    // DMAL-IS-INPUT (p.379 pin 13 DMAL; p.381 pin 18 DMAL -- same name,
    // same direction, graded once). Companion record() below for the
    // one-line-vs-five-ports count, which is NOT graded either way.
    // =========================================================================
    {
        Harness h;
        const uint32_t PTR = 0x00800;
        h.write_reg(A_DSKPTH, (uint16_t)((PTR >> 15) & 0x1F));
        h.write_reg(A_DSKPTL, (uint16_t)((PTR & 0x7FFF) << 1));
        h.dmacon_set(1 << DMAEN_BIT);
#if !MUT_DMAL_WITHHELD
        h.dut->disk_dmal = 1;
#endif
        h.dut->disk_dmas = 0;
        bool seen = false; uint32_t seen_addr = 0;
        for (int i = 0; i < 4000 && !seen; ++i) {
            h.tick();
            if (h.dut->dbr && !h.dut->cpu_custom) { seen = true; seen_addr = h.dut->address_out; }
        }
        h.dut->disk_dmal = 0;
        char buf[96];
        std::snprintf(buf, sizeof buf, "seen=%d addr=$%05X (expected $%05X)", seen, seen_addr, PTR);
        checks.note(std::string("disk_dmal grant: ") + buf);

        // Positive-control corroboration for audio_dmal[3:0] -- NOT
        // separately graded (see the record() below for why the width
        // itself is out of scope): a real audio DMA cycle at a distinct,
        // just-written pointer, reusing hrm/ch1-agnus-what-it-contains's
        // own audio-leg recipe (its DMA-CHORES-INDEPENDENT-OF-CPU).
        {
            const uint32_t AUDIO_PTR = 0x00900;
            h.write_reg(A_AUD0LCH, (uint16_t)((AUDIO_PTR >> 15) & 0x1F));
            h.write_reg(A_AUD0LCL, (uint16_t)((AUDIO_PTR & 0x7FFF) << 1));
            h.dut->audio_dmal = 0x1; h.dut->audio_dmas = 0x1;
            bool audio_seen = false;
            for (int i = 0; i < 8000 && !audio_seen; ++i) {
                h.tick();
                if (h.dut->dbr && h.dut->address_out == AUDIO_PTR) audio_seen = true;
            }
            h.dut->audio_dmal = 0; h.dut->audio_dmas = 0;
            checks.note(std::string("audio_dmal[0] corroboration (not graded): a real audio DMA cycle ") +
                        (audio_seen ? "WAS observed" : "was NOT observed") +
                        " at the just-written AUD0LC pointer.");
        }

        checks.expect(seen && seen_addr == PTR,
            "DMAL-IS-INPUT (p.379 pin 13 DMAL \"DMA request line\" I; "
            "p.381 pin 18 DMAL \"Request audio/disk DMA\" I -- same name, "
            "same direction, graded once): every DMA-request port agnus "
            "declares is an input and every one is driven from Paula at "
            "minimig.v:476-479 -- agnus.v:28's own header comment names "
            "it, \"only a dma request input from Paula (dmal input, disk "
            "and audio)\". Graded via disk_dmal (agnus.v:92): asserted "
            "with DMACON DMAEN set, agnus allocates the disk DMA slot "
            "(dbr rises, cpu_custom drops, address_out presents the "
            "written pointer). sol:J-012 opus:AHJ-143 sol:J-136 "
            "opus:AHJ-270");

        checks.record(
            "The page prints ONE DMAL line (singular, \"DMA request "
            "line\" / \"Request audio/disk DMA\"). NanoMig implements "
            "FIVE separate request wires, not one: `audio_dmal[3:0]` "
            "(agnus.v:90, 4 bits) and `disk_dmal` (agnus.v:92, 1 bit) -- "
            "measured directly from the port declarations, not inferred. "
            "\"DMAL is an input\" holds for all five and is what "
            "DMAL-IS-INPUT above grades; \"DMAL is one line\" does not "
            "hold and is NOT asserted anywhere in this file -- the "
            "count/multiplexing difference is recorded here, not graded "
            "either true or false, per this package's own build task. The "
            "corroborating audio_dmal[0] cycle above (note, not graded) "
            "shows the width split is a genuine, separately-drivable "
            "second request path, not a cosmetic renaming of the same "
            "wire.");
    }
    // MUTATION DMAL-IS-INPUT stimulus red MUT_DMAL_WITHHELD: disk_dmal is left deasserted throughout instead of asserted -- no disk DMA request ever reaches agnus, the only dbr grant observed is refresh's own (unconditional, address_out==0), and since seen_addr never matches PTR the conjunct goes red.

    // =========================================================================
    // BLS-IS-INPUT (p.379 pin 14 BLS*; p.381 pin 19 BLS* -- same name,
    // same direction, graded once).
    // =========================================================================
    {
        Harness h;
        const uint32_t DEST = 0x00A00;
        h.write_reg(A_BLTCON1, 0x0000);
        h.write_reg(A_BLTDPTH, (uint16_t)((DEST >> 15) & 0x1F));
        h.write_reg(A_BLTDPTL, (uint16_t)((DEST & 0x7FFF) << 1));
        h.write_reg(A_BLTCON0, 0x01FF);
#if !MUT_BLS_NEVER_ASSERTED
        h.dut->bls = 1; // held from before the blit is even armed
#endif
        h.dmacon_set((1 << BLTEN_BIT) | (1 << DMAEN_BIT));
        h.write_reg(A_BLTSIZE, 0x0041);
        bool saw_write_at_dest = false;
        for (int i = 0; i < 8000; ++i) {
            h.tick();
            if (h.dut->dbr && h.dut->dbwe && h.dut->address_out == DEST) saw_write_at_dest = true;
        }
        h.dut->bls = 0;
        bool blit_stalled = !saw_write_at_dest;
        checks.note(std::string("blit under bls held high: ") +
                    (saw_write_at_dest
                        ? "completed within budget"
                        : "did NOT complete within an 8000-tick budget -- the identical recipe with "
                          "bls at its default 0 (hrm/ch1-agnus-what-it-contains's own "
                          "BLITTER-MOVES-DATA-ACROSS-BOUNDARY) completes comfortably inside it"));
        checks.expect(blit_stalled,
            "BLS-IS-INPUT (p.379 pin 14 BLS* \"Blitter slowdown\" I; "
            "p.381 pin 19 BLS* \"Blitter slowdown\" I -- same name, same "
            "direction, graded once): `bls` (agnus.v:94, input) feeds "
            "`bls_cnt` (agnus.v:429-432, incremented once per CCK while "
            "bls is held and bltpri is clear) which gates both the "
            "blitter's own DMA enable (`ena_blt`, agnus.v:310) and the "
            "top-level grant itself (`dma_blt && bls_cnt!=BLS_CNT_MAX`, "
            "agnus.v:243). Held asserted from before a minimal "
            "channel-D-only blit is even armed, the blitter never wins a "
            "grant and the destination write this same recipe produces "
            "cleanly with bls at its default 0 is never observed here. "
            "sol:J-014 opus:AHJ-145 sol:J-142 opus:AHJ-276");
    }
    // MUTATION BLS-IS-INPUT stimulus red MUT_BLS_NEVER_ASSERTED: bls is left deasserted throughout instead of held high -- bls_cnt never leaves 0, the blitter is never blocked, and the same blit that stalled above now completes within budget, so blit_stalled goes false (red).

    // =========================================================================
    // CLK-IS-INPUT (p.381 pin 34 28MHZ only -- no p.379 counterpart).
    // THINNEST claim in this package, demoted from `stimulus red` to
    // `none impossible`; see the file header for why.
    // =========================================================================
    {
        Harness h;
        int hsync_low = 0, hsync_high = 0;
        const int SAMPLE_TICKS = 2000;
        for (int i = 0; i < SAMPLE_TICKS; ++i) {
            h.tick();
            if (h.dut->_hsync) ++hsync_high; else ++hsync_low;
        }
        bool clk_drives_live_state = hsync_low > 0 && hsync_high > 0;
        char buf[96];
        std::snprintf(buf, sizeof buf, "over %d ticks, _hsync low=%d high=%d",
                      SAMPLE_TICKS, hsync_low, hsync_high);
        checks.note(std::string("CLK-LIVENESS: ") + buf);
        checks.expect(clk_drives_live_state,
            "CLK-IS-INPUT (p.381 pin 34 28MHZ \"Master clock\" I -- "
            "THINNEST claim in this package, named as such rather than "
            "hidden): `clk` (agnus.v:56, input) is wired from "
            "minimig.v:181's own 28.37516 MHz clock, generated one level "
            "above this fixture's boundary (amiga_clk.v, excluded from "
            "this build) -- Agnus takes the master clock, it does not "
            "make it. NO STIMULUS MUTATION exists for this claim: "
            "freezing `clk` (never toggling the wire this harness's own "
            "tick() drives) would be true by Verilog's own clocked-"
            "register semantics alone -- a register on `posedge clk` "
            "provably cannot update without a clk edge, with no need to "
            "run the simulator to know it -- so it would test the "
            "language, not this RTL. Demoted per this package's build "
            "task: graded instead via a positive-control liveness "
            "measurement -- over a normal run, clk-derived sequential "
            "state (`_hsync`, driven from the beam counters "
            "agnus_beamcounter.v clocks on `posedge clk`) reads both low "
            "and high, proving the port is a genuine, consumed input "
            "driving real, live sequential logic, not a name with "
            "nothing behind it. NO RATE is asserted -- the printed cell "
            "says \"Master clock\", minimig.v:181 and nanomig.v:130 "
            "disagree on the actual number (28.37516 vs 28.6875 MHz), and "
            "this record grades neither. sol:J-158 opus:AHJ-292");
    }
    // MUTATION CLK-IS-INPUT none impossible: freezing clk to falsify this claim would be tautologically true by Verilog's own clocked-register semantics (no posedge, no update) and would demonstrate nothing about this RTL that its own language definition does not already guarantee -- not a genuine stimulus mutation, unlike every other assertion in this file. Graded instead via the positive-control CLK-LIVENESS measurement in the same block: a normal run's _hsync genuinely toggles (both low and high observed), proving clk reaches real, live sequential logic rather than being an unconsumed port name.

    // =========================================================================
    // NTSC-IS-INPUT (p.381 pin 41 (shared with TEST) NTSC/PAL only -- no
    // p.379 counterpart).
    // =========================================================================
    {
        auto measure_field_ticks = [](Harness& h, long long budget) -> long long {
            bool prev = false;
            long long first = -1, second = -1;
            for (long long i = 0; i < budget; ++i) {
                h.tick();
                bool now = h.dut->sof != 0;
                if (now && !prev) {
                    if (first < 0) first = i;
                    else { second = i; break; }
                }
                prev = now;
            }
            return (first >= 0 && second >= 0) ? (second - first) : -1;
        };

        Harness h_pal(false);   // ntsc=0 held through reset -> pal=1 (agnus_beamcounter.v:171 `pal <= ~ntsc`)
#if !MUT_NTSC_SAME
        Harness h_ntsc(true);   // ntsc=1 held through reset -> pal=0
#else
        Harness h_ntsc(false);  // MUTATION: both harnesses see the same ntsc value
#endif
        // A PAL field measures ~568,000 raw ticks (312 lines); seeing TWO
        // sof edges needs up to two full fields in the worst case (an
        // unlucky start just after one), so 1,000,000 was measured to be
        // too tight (first run: t_pal=-1, t_ntsc=477,608) -- widened here
        // with comfortable headroom.
        const long long BUDGET = 2500000;
        long long t_pal = measure_field_ticks(h_pal, BUDGET);
        long long t_ntsc = measure_field_ticks(h_ntsc, BUDGET);
        char buf[128];
        std::snprintf(buf, sizeof buf,
                      "t_pal=%lld t_ntsc=%lld ticks (sof-to-sof; -1 = not observed within budget)",
                      t_pal, t_ntsc);
        checks.note(std::string("NTSC field-period measurement: ") + buf);
        bool ntsc_field_meaningfully_shorter =
            (t_pal > 0 && t_ntsc > 0 && t_ntsc < (t_pal * 9) / 10);
        checks.expect(ntsc_field_meaningfully_shorter,
            "NTSC-IS-INPUT (p.381 pin 41 (shared with TEST) NTSC/PAL "
            "\"Select video environment\" I (ECS Fat Agnus) -- the "
            "generation qualifier is representable at this boundary too, "
            "via agnus.v:97 `input ecs`, but is NOT separately graded "
            "here, only noted): `ntsc` (agnus.v:95, input) is captured "
            "into agnus_beamcounter's own `pal` register only while reset "
            "is held (agnus_beamcounter.v:171 `if (reset) pal <= ~ntsc`), "
            "which then selects between VTOTAL_PAL_VAL (311) and "
            "VTOTAL_NTSC_VAL (261) (agnus_beamcounter.v:87-88,229) "
            "whenever the ECS variable-beam path is not engaged (the "
            "default here). Two fresh harnesses, identical except the "
            "value driven on `ntsc` through their own reset, measure a "
            "meaningfully shorter sof-to-sof field period with ntsc=1 "
            "than with ntsc=0 -- the port is read and changes real, "
            "observable timing, not merely present and ignored. NO RATE "
            "OR LINE COUNT is asserted (that is hrm/ch3-beam-counters-and-"
            "blanking's PAL-FIELD-312-NTSC-FIELD-262-LINES, cited not "
            "re-derived); this grades the input-direction half only. "
            "sol:J-174 opus:AHJ-308");
    }
    // MUTATION NTSC-IS-INPUT stimulus red MUT_NTSC_SAME: the second harness is also constructed with ntsc=0 (the same value as the first) instead of ntsc=1 -- both measured field periods come out equal (within noise), so the "meaningfully shorter" conjunct goes red.

    // =========================================================================
    // DECLINED (1 of 36): p.379 pin 39 CSY* -- a prior assertion already
    // grades the SAME proposition.
    // =========================================================================
    checks.record(
        "p.379 pin 39, CSY* \"Composite sync\" O (sol:J-034, opus:AHJ-165): "
        "not graded here -- a prior assertion already grades the SAME "
        "proposition. `_csync` (agnus.v:75, output) is driven at "
        "agnus_beamcounter.v:421 (`assign _csync = _hsync & _vsync | "
        "vser`) and wired straight through, unchanged, at agnus.v:477 -> "
        "minimig.v:461 (`._csync(_csync)`) onto minimig's own top-level "
        "`_csync` output -- the SAME wire, not a re-derived or re-decoded "
        "one. hrm/ch8-video-signal-set's SYNC-OUTPUT-TOGGLES "
        "(test_minimig.cpp:544-553, opus:8-display.c12 sol:8-display.c11) "
        "already grades that this exact wire is a genuine, driven, "
        "externally-observable output -- reading low for part of a "
        "sampled window and high for the rest, rather than tied to a "
        "constant -- the identical behavioural proxy for \"is an output\" "
        "this package's own DBR-IS-OUTPUT / DBWE-IS-OUTPUT / "
        "INT3-IS-OUTPUT assertions use above, applied to the identical "
        "signal one level up (`minimig`, wrapping `agnus`, rather than "
        "`agnus` itself). Per this package's own rule -- a decline is "
        "recorded only where a prior assertion's own label states the "
        "same proposition -- this one does. p.381's Fat Agnus table "
        "prints CSY* as an output too (not a separate claim in this "
        "filter's 36), so no cross-page conflict arises either.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
