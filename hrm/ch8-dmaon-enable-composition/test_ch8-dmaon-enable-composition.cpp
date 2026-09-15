// Test purpose: DSKBYTR bit 14 (DMAON)'s composition -- which of DSKLEN's
//   own DMAEN bit, DMACON's DSKEN bit and DMACON's master DMAEN bit are
//   actually required for DMAON to read 1 -- graded against BOTH of the
//   manual's own conflicting statements of this fact: the chapter's own
//   p.248 DMAON row (three terms) and Appendix A's p.278 DSKBYTR bit-14 row
//   (two terms, omitting DSKEN). 8 claims, from
//   work/ch8-s2-build/ch8-dmaon-enable-composition.jsonl, reduced to 5
//   graded propositions (A1-A5) + fixture liveness (A0) + one
//   mutation-demonstration substitute (A2-SUBSTITUTE, no claim uid). Full
//   derivation in SPECIFICATION.md (committed as c1b7870, before any RTL
//   was opened).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   p.248 (chapter 8's own DMAON row) and Appendix A printed p.278
//   (DSKBYTR bit-14 row).
//
// BOUNDARY: paula.v ($(NANOMIG)/src/minimig-aga/paula.v), reached exactly
//   as hrm/ch5-audio-dmacon-adkcon already reaches it -- paula.v
//   instantiates paula_uart, paula_intcontroller, paula_floppy and
//   paula_audio directly, so all seven of its own submodule files are
//   compiled alongside it (see this package's own Makefile). `paula` is
//   this package's assigned fixture per the build brief specifically
//   because it is the one boundary where DMACON's two disk-relevant bits
//   (DSKEN, DMAEN) and DSKLEN/DSKBYTR are all simultaneously drivable and
//   readable -- paula_floppy alone only exposes a single collapsed
//   `enable` port, which cannot separate DSKEN from the master DMAEN.
//
// RTL READ, IN ORDER, ALL AFTER SPECIFICATION.md WAS COMMITTED (c1b7870):
//   1. hrm/ch8-dsklen-dma-arming/Makefile, test_ch8-dsklen-dma-arming.cpp,
//      SPECIFICATION.md -- for this suite's established harness/mutation
//      boilerplate (the Checks class, the MUTATE env-var convention, the
//      settle_edges()/do_reset()/write_reg()/read_reg() clocking pattern,
//      the fresh-Rig-per-trial rule, the summary-line format). No new
//      paula-specific fact came from this beyond what its own header
//      already discloses (DSKLEN/DSKBYTR addresses, the double-write
//      arming mechanism, the 0x9000/0xB000/0xD000 DSKBYTR values) --
//      those are independently re-derived below from paula.v/
//      paula_floppy.v directly, not assumed from that package.
//   2. hrm/ch5-audio-dmacon-adkcon/Makefile and test_paula.cpp's Harness
//      class (lines ~570-605) -- for the paula-level port default values
//      and the clk7_en/clk7n_en/cck tick() ratios this fixture's top
//      module (Vpaula, not Vpaula_floppy) needs that paula_floppy alone
//      does not have (rxd, ntsc, sof, strhor, vblint, int2/3/6, the audio
//      DAC outputs). Cited for harness plumbing only, not as an oracle for
//      any assertion below.
//   3. nanomig/src/minimig-aga/paula.v -- full file (312 lines). Confirms:
//      DMACON's own word address (parameter DMACON=9'h096, :125), its
//      SET/CLR write convention (data_in[15] selects OR vs AND-NOT,
//      :159-171), that `dsken` (paula_floppy's own `enable` port) is
//      exactly `dmacon[4] & dmaen` (:174, dmacon[4]=DSKEN, dmaen=master
//      DMAEN/bit 9) computed combinationally from the two registered
//      DMACON bits, and that paula.v instantiates paula_floppy (:243-286)
//      and nothing else floppy-related -- paula_floppy_ebr.v and
//      paula_floppy_dma_buf.v are NOT part of this fixture's elaboration
//      (confirmed by reading the whole file: only one floppy instantiation
//      exists).
//   4. nanomig/src/minimig-aga/paula_floppy.v, targeted sections (grepped
//      for register-address parameters and decode points, then read in
//      full around lines 690-760 and 985-1030): DSKBYTR's own word address
//      (parameter DSKBYTR=9'h01a, :120) and its bit layout (:699,
//      `dskbytr = {1'b1,(trackrd|trackwr),dsklen[14],5'b1_0000,8'h00}` --
//      bit15 hardwired 1, bit14=DMAON=(trackrd|trackwr), bit13=dsklen[14]
//      mirror, bit12 hardwired 1, bits 11-0 zero); DSKLEN's own word
//      address (parameter DSKLEN=9'h024, :124) and its two-part write
//      logic (:712-720 loads dsklen[14:0] on EVERY write; :724-733 loads
//      dsklen[15] on every write too, but :735-744's SEPARATE `dmaen` reg
//      -- paula_floppy's own local disk-DMA-armed flag, a different
//      register from paula.v's module-level `dmaen` -- only becomes 1 when
//      the CURRENT write's data_in[15] AND the PREVIOUS already-latched
//      dsklen[15] are both 1, i.e. only on the second of two consecutive
//      writes with bit 15 set); `dma_active = dmaen && !lenzero && enable`
//      (:248, all three of THIS module's own three preconditions -- the
//      DSKLEN-side double-write-armed `dmaen`, a nonzero length, and the
//      `enable` port fed by paula.v's dsken); and the dskstate state
//      machine (:992-1019) which transitions DISKDMA_IDLE->DISKDMA_ACTIVE
//      exactly one qualifying clk7_en edge after dma_active becomes stable
//      (registered dskstate, combinational trackrd/trackwr/dmaon off of
//      it) -- confirming DSKBYTR bit 14 needs a few settled clk7_en edges
//      after the write(s) that make dma_active true before it reads 1.
//
// WHY paula.v's OWN `dmaen` (DMACON-side, master enable bit 9) and
//   paula_floppy.v's OWN `dmaen` (DSKLEN-side, the double-write-armed
//   flag) ARE TWO DIFFERENT REGISTERS IN TWO DIFFERENT MODULES, both
//   named `dmaen`: this is read directly off the two files above, not
//   inferred. This package's own claims and SPECIFICATION.md call the
//   first "DMACON.DMAEN" (or "the master DMAEN bit") and the second
//   "DSKLEN.DMAEN" throughout, to keep them apart in prose; the C++ code
//   below uses `dmacon_master_dmaen` (a bool the test itself drives, via a
//   DMACON write) and relies on paula_floppy's own internal `dmaen` (not
//   directly poked, only driven indirectly via DSKLEN writes) for the
//   other.
//
// MEASUREMENT BASIS: every assertion below reads DSKBYTR (this fixture's
//   `data_out` at word address DSKBYTR_W, i.e. paula.v's own
//   `uartdata_out | intdata_out | diskdata_out | adkconr` OR-mux, :152 --
//   a plain combinational read needing no settled edge of its own once the
//   underlying registers it depends on have settled) and inspects bit 14
//   (0x4000) for DMAON. DMACON is written via its own SET/CLR convention
//   (bit 15 of the write selects OR-in vs AND-out, :159-171); DSKLEN is
//   written via plain sequential loads (:712-744, no SET/CLR). Every write
//   below programs DSKLEN's own direction bit (bit 14, WRITE) as 0 (read
//   direction) and a small nonzero length (0x00A, ten words) -- the WRITE
//   bit and the exact length are not under test here (hrm/ch8-dsklen-dma-
//   arming already grades those) and are held fixed across every trial so
//   only the three enable terms this package exists to test ever vary.
//
// FIXTURE-LIVENESS AND DATA CONTENT (A0): unlike hrm/ch8-dsklen-dma-arming,
//   whose A0 lives at the narrower `paula_floppy` boundary, this package's
//   A0 must prove the data path live at THIS fixture's own boundary
//   (`paula`, not `paula_floppy` directly) -- a write via paula's own
//   reg_address_in/data_in, reaching paula_floppy's internal dsklen
//   register, and back out through paula's own data_out OR-mux. Two
//   distinct, known single DSKLEN writes (WRITE bit clear, then WRITE bit
//   set, DMAEN left low both times so DMA never arms) must arrive at two
//   distinct, individually-predicted DSKBYTR readings (0x9000, then
//   0xB000) before any DMAON-based assertion below is trusted.
//
// RIG HAZARDS (build brief and finding-candidates document, restated,
//   acted on): `dsktrack` has no reset term in paula_floppy.v -- this
//   package never drives `_step` (held permanently inactive, matching
//   hrm/ch8-dsklen-dma-arming's own convention: active-low, held at 1, no
//   edge ever presented), so dsktrack is never expected to move within a
//   trial regardless, but a FRESH Vpaula (and therefore a fresh
//   Vpaula_floppy instance inside it) is still constructed for every
//   single trial below, never one reused instance across multiple writes
//   meant to represent independent trials -- the rig-hazard mandate is
//   followed literally rather than relied-on-not-to-matter.
//   paula_floppy_ebr.v's duplicate `module paula_floppy` cannot arise here
//   at all: paula.v (read in full above) does not instantiate it, so it is
//   not part of this fixture's Verilator elaboration, not merely
//   `-Wno-fatal`'d around.
//
// MUTATION METHODOLOGY: every `expect()` below is guarded by a boolean
//   read once, at the top of its own block, from `mutating("<ID>")` (the
//   MUTATE environment variable, checked at process startup into
//   `g_mutate`) -- never a string literal inside the `expect()` call's own
//   condition argument itself, which would hand the gate's first-string-
//   literal id-reader the wrong text and orphan the real label (named as a
//   hazard hit by two sibling builders today). One build serves both the
//   real run (MUTATE unset) and every mutation run (MUTATE=<id>, one id
//   per assertion). Baseline with MUTATE unset: 6 passed, 1 failed (A2 is
//   the one expected failure -- the p.278 two-term reading, graded
//   literally, does not hold; see A2's own header comment and
//   SPECIFICATION.md). Actual run counts and per-mutation results are
//   recorded in the commit message and build report, not predicted here.
//
// A2 is expected RED at baseline (this package's own claims file and the
//   finding-candidates document's C11 both name this in advance: DMACON's
//   DSKEN bit is required in addition to the two terms p.278 prints, so
//   the two-term reading, graded as printed, is false against NanoMig).
//   Per the build brief, a stimulus mutation on an already-red assertion
//   proves nothing; the substitute rule applies instead --
//   A2-SUBSTITUTE-TWO-TERM-COMPARATOR-LIVE is its own, separate,
//   GREEN-at-baseline assertion that applies the SAME two-term-AND-
//   predicts-output comparator SHAPE to a different pair of terms
//   (DSKLEN.DMAEN and DMACON.DSKEN, with DMACON's master DMAEN pinned true
//   throughout) that DOES fully determine DMAON -- demonstrating the
//   comparator apparatus is not simply broken or vacuously red. It carries
//   its own ordinary stimulus mutation (MUTATE=A2S), since it is green at
//   baseline, unlike A2.
//
// MUTATION A0-LIVENESS-PAULA-DSKLEN-WRITE-TO-DSKBYTR-READBACK stimulus red: second known value driven identical to the first (0x0000 both times, WRITE bit clear both times) so the two readings are indistinguishable (MUTATE=A0).
// MUTATION A1-DMAON_P248_THREE_TERM_COMPOSITION stimulus red: the "all three true" row's own DSKLEN arming is skipped (single write only, not double) even though the row's own predicted value stays true, so the measured/predicted truth table stops matching on that one row (MUTATE=A1).
// MUTATION A2-DMAON_P278_TWO_TERM_COMPOSITION none impossible: already red at baseline -- NanoMig requires DMACON's DSKEN bit in addition to the two terms p.278 names, so the two-term reading, graded as printed (DSKEN irrelevant), does not hold. A stimulus mutation cannot fail an assertion that already fails, so none is recorded for this label; the comparator that grades it is instead proven live via the substitute A2-SUBSTITUTE-TWO-TERM-COMPARATOR-LIVE below.
// MUTATION A2-SUBSTITUTE-TWO-TERM-COMPARATOR-LIVE stimulus red: with DSKLEN.DMAEN and the pinned master DMAEN both held true, the DSKEN=false row's own stimulus is overridden to also set DSKEN true, while the check still expects that row's DMAON to read 0 (MUTATE=A2S).
// MUTATION A3-DMAON_REQUIRES_DSKLEN_DMAEN stimulus red: the "term false" sub-case's single DSKLEN write is followed by a second, accidentally arming it, while the check still expects the not-yet-armed (DMAON=0) reading (MUTATE=A3).
// MUTATION A4-DMAON_REQUIRES_DMACON_DSKEN stimulus red: the "term false" sub-case's DMACON write also sets the DSKEN bit, while the check still expects the DSKEN-clear (DMAON=0) reading (MUTATE=A4).
// MUTATION A5-DMAON_REQUIRES_DMACON_DMAEN stimulus red: the "term false" sub-case's DMACON write also sets the master DMAEN bit, while the check still expects the DMAEN-clear (DMAON=0) reading (MUTATE=A5).

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "Vpaula.h"
#include "verilated.h"

namespace {

const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}

std::string hex4(uint16_t v) {
    char b[8];
    std::snprintf(b, sizeof b, "%04X", v);
    return std::string(b);
}

// Register word addresses: reg_address_in is [8:1], compared directly
// against each register's own 9-bit parameter's own [8:1] slice -- i.e.
// word_address == byte_address >> 1, read directly off paula.v's/
// paula_floppy.v's own parameter declarations (DMACON=9'h096, DSKLEN=
// 9'h024, DSKBYTR=9'h01a), not inferred.
constexpr int DMACON_W  = 0x096 >> 1;  // paula.v:125
constexpr int DSKLEN_W  = 0x024 >> 1;  // paula_floppy.v:124
constexpr int DSKBYTR_W = 0x01a >> 1;  // paula_floppy.v:120
constexpr int IDLE_W    = 0x000;       // Unused address: matches no register above.

// DSKLEN's own direction (WRITE) bit and length field, held fixed across
// every trial in this package (not under test here -- hrm/ch8-dsklen-dma-
// arming already grades WRITE and LENGTH). WRITE=0 (read direction),
// LENGTH=10 (nonzero, so `lenzero` never blocks dma_active).
constexpr uint16_t DSKLEN_BODY = 0x000A;  // WRITE=0, LENGTH=10.
constexpr uint16_t DSKLEN_ARM_BIT = 0x8000;  // bit 15, DMAEN, loaded every write.

bool dmaon_bit(uint16_t dskbytr) { return (dskbytr & 0x4000) != 0; }

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

// A fresh Verilated model per trial, per the build brief's rig hazard
// (dsktrack has no reset term anywhere in paula_floppy.v). Every assertion
// block below constructs its own Rig, even though this package never
// drives `_step` and dsktrack is therefore not expected to move -- the
// mandate is followed literally, not relied on not to matter.
struct Rig {
    Vpaula* dut;
    int clk7_phase = 0;

    Rig() { dut = new Vpaula; }
    ~Rig() { dut->final(); delete dut; }
    Rig(const Rig&) = delete;
    Rig& operator=(const Rig&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_phase = (clk7_phase + 1) % 4;
        dut->clk7_en  = (clk7_phase == 0);
        dut->clk7n_en = (clk7_phase == 2);
    }

    // Advances until exactly n qualifying (clk7_en-active) rising edges
    // have been applied. clk7_en, as read here, is the value that will
    // gate the UPCOMING tick()'s rising edge (set by the previous tick's
    // tail) -- same convention as hrm/ch8-dsklen-dma-arming's own Rig.
    void settle_edges(int n) {
        int seen = 0;
        while (seen < n) {
            bool q = dut->clk7_en;
            tick();
            if (q) seen++;
        }
    }

    void do_reset() {
        clk7_phase = 0;
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->reg_address_in = IDLE_W;
        dut->data_in = 0;
        dut->rxd = 1;
        dut->ntsc = 0;
        dut->sof = 0;
        dut->strhor = 0;
        dut->vblint = 0;
        dut->int2 = 0;
        dut->int3 = 0;
        dut->int6 = 0;
        dut->_step = 1;      // inactive (active low), never driven low: no head stepping.
        dut->direc = 0;
        dut->_sel = 0xE;     // drive 0 selected (active low, 4'b1110); not consulted by dma_active.
        dut->side = 0;
        dut->_motor = 1;     // inactive; not consulted by dma_active.
        dut->sdc_image_mounted = 0;
        dut->sdc_image_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        dut->floppy_drives = 0;
        dut->floppy_wrprot = 0;
        settle_edges(8);
        dut->reset = 0;
        settle_edges(8);
    }

    // Exactly one qualifying-edge write, then the address is withdrawn so
    // no further qualifying edge repeats it.
    void write_reg(int addr, uint16_t data) {
        dut->reg_address_in = addr;
        dut->data_in = data;
        settle_edges(1);
        dut->reg_address_in = IDLE_W;
        dut->data_in = 0;
        settle_edges(2);
    }

    // DMACON's own SET/CLR convention (paula.v:159-171): bit 15 of the
    // written word selects OR-in (set) vs AND-NOT-out (clear); bit 9
    // carries the master DMAEN term, bit 4 the DSKEN term. Every write
    // this package performs names exactly the bit(s) it means to change;
    // the other bit is left at 0 in the written word, which under SET
    // leaves it unaffected-if-already-0 and under CLR leaves it entirely
    // unaffected (CLR only clears bits that are 1 in the written word).
    void write_dmacon(bool set, bool dsken_bit, bool master_dmaen_bit) {
        uint16_t v = (set ? 0x8000 : 0x0000) |
                     (dsken_bit ? 0x0010 : 0x0000) |
                     (master_dmaen_bit ? 0x0200 : 0x0000);
        write_reg(DMACON_W, v);
    }

    // Two consecutive single-edge writes of the same DSKLEN value, the
    // established double-write arming sequence (paula_floppy.v:735-744):
    // the SECOND write's data_in[15] AND the dsklen[15] already latched by
    // the FIRST write must both be 1 for paula_floppy's own local `dmaen`
    // to become 1.
    void dsklen_write_double(uint16_t value) {
        write_reg(DSKLEN_W, value);
        write_reg(DSKLEN_W, value);
    }

    // A single write only -- loads dsklen[14:0] and dsklen[15], but
    // paula_floppy's own local `dmaen` stays 0 (the previous dsklen[15],
    // from a fresh reset, is 0).
    void dsklen_write_single(uint16_t value) {
        write_reg(DSKLEN_W, value);
    }

    uint16_t read_reg(int addr) {
        dut->reg_address_in = addr;
        dut->eval();
        uint16_t v = (uint16_t)dut->data_out;
        dut->reg_address_in = IDLE_W;
        dut->eval();
        return v;
    }

    uint16_t read_dskbytr() { return read_reg(DSKBYTR_W); }
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // ========================================================================
    // A0 -- FIXTURE DATA-PATH LIVENESS, at the `paula` boundary specifically
    // (not the narrower `paula_floppy` boundary hrm/ch8-dsklen-dma-arming's
    // own A0 already proves). No claim id, no manual page. Two distinct,
    // known single DSKLEN writes (WRITE bit clear, then WRITE bit set, DMAEN
    // left low both times) must arrive at two distinct, individually-
    // predicted DSKBYTR readings.
    // ========================================================================
    {
        Rig rig;
        rig.do_reset();
        rig.dsklen_write_single(0x0000);
        uint16_t v1 = rig.read_dskbytr();
        checks.note("A0-LIVENESS-WORD1: DSKLEN<=0x0000 (single write), DSKBYTR readback=0x" + hex4(v1));

        const bool mutA0 = mutating("A0");
        uint16_t word2 = mutA0 ? 0x0000 : 0x4000;
        rig.dsklen_write_single(word2);
        uint16_t v2 = rig.read_dskbytr();
        checks.note("A0-LIVENESS-WORD2: DSKLEN<=0x" + hex4(word2) + " (single write), DSKBYTR readback=0x" + hex4(v2));

        checks.expect(v1 == 0x9000 && v2 == (mutA0 ? 0x9000 : 0xB000) && v1 != v2,
            "A0-LIVENESS-PAULA-DSKLEN-WRITE-TO-DSKBYTR-READBACK: two distinct, known DSKLEN writes (0x0000 then "
            "0x4000), made through paula's own reg_address_in/data_in ports, arrive unmodified and mutually "
            "distinguishable at the DSKBYTR mirror bit read back through paula's own data_out OR-mux -- this "
            "fixture's write-then-read data path is live at the `paula` boundary itself, not stuck");
    }

    // ========================================================================
    // A1 -- opus:8-dskbytr.c9 (p.248) / sol:8-dskbytr.c10 (p.248). The
    // chapter's own THREE-term reading: DMAON is true if and only if ALL
    // THREE of DSKLEN.DMAEN, DMACON.DSKEN and DMACON.DMAEN(master) are true
    // -- graded as an exhaustive 8-row truth table over the three terms,
    // each row a fresh Rig.
    // ========================================================================
    {
        struct Combo { bool dsklen_dmaen, dsken, master_dmaen; };
        const Combo combos[8] = {
            {false, false, false}, {false, false, true}, {false, true, false}, {false, true, true},
            {true,  false, false}, {true,  false, true}, {true,  true, false}, {true,  true, true},
        };

        // MUTATE=A1: the "all three true" row (index 7) skips its own
        // DSKLEN arming (single write, not double) even though the row's
        // own predicted value stays true -- hoisted before the loop, no
        // string literal inside the expect() condition below.
        const bool skipArmRow7 = mutating("A1");

        bool all_match = true;
        for (int i = 0; i < 8; ++i) {
            const Combo& c = combos[i];
            Rig rig;
            rig.do_reset();
            rig.write_dmacon(true, c.dsken, c.master_dmaen);
            uint16_t dsklen_val = DSKLEN_BODY;  // WRITE=0, LENGTH=10; bit15 set on each write attempt.
            if (c.dsklen_dmaen) {
                if (i == 7 && skipArmRow7) rig.dsklen_write_single(DSKLEN_ARM_BIT | dsklen_val);
                else                       rig.dsklen_write_double(DSKLEN_ARM_BIT | dsklen_val);
            }
            // else: DSKLEN never written at all -- dsklen[15] and the local
            // `dmaen` both stay at their reset value, 0.
            rig.settle_edges(4);
            uint16_t v = rig.read_dskbytr();
            bool measured = dmaon_bit(v);
            bool predicted = c.dsklen_dmaen && c.dsken && c.master_dmaen;
            bool row_ok = (measured == predicted);
            all_match = all_match && row_ok;
            checks.note("A1-ROW[" + std::to_string(i) + "] DSKLEN.DMAEN=" + std::to_string(c.dsklen_dmaen) +
                        " DMACON.DSKEN=" + std::to_string(c.dsken) + " DMACON.DMAEN=" + std::to_string(c.master_dmaen) +
                        " predicted=" + std::to_string(predicted) + " measured=" + std::to_string(measured) +
                        " (DSKBYTR=0x" + hex4(v) + ")" + (row_ok ? "" : " MISMATCH"));
        }

        checks.expect(all_match,
            "A1-DMAON_P248_THREE_TERM_COMPOSITION (p.248, opus:8-dskbytr.c9, quote '     14      DMAON      "
            "Indicates when DMA is actually enabled.  All the\\n                        various DMA bits must be "
            "true.  This means the\\n                        DMAEN bit in  DSKLEN , and the DSKEN & DMAEN bits\\n "
            "                       in  DMACON .'; sol:8-dskbytr.c10, quote 'All the\\n                        "
            "various DMA bits must be true.'): across all 8 combinations of DSKLEN.DMAEN, DMACON.DSKEN and "
            "DMACON.DMAEN(master), DMAON reads 1 if and only if all three are true, exactly matching this page's "
            "own three-way reading -- this package's control case, on the same DSKBYTR-bit-14 measurement basis as "
            "A2's contested two-term reading");
    }

    // ========================================================================
    // A2 -- opus:8-appA-dskbytr.c11 / sol:8-appa-dskbytr.c11 (p.278).
    // Appendix A's own TWO-term reading, graded LITERALLY: DMAON is the AND
    // of DSKLEN.DMAEN and DMACON.DMAEN(master) ALONE -- DMACON's DSKEN bit
    // is not named and, as printed, is irrelevant. Held with DSKLEN.DMAEN=1
    // and DMACON.DMAEN=1 fixed, DSKEN is driven both false and true; p.278
    // predicts DMAON=1 in BOTH rows (DSKEN irrelevant). Expected RED: the
    // DSKEN=false row's real DMAON is 0, contradicting the page as printed.
    // ========================================================================
    {
        bool row_dsken_false_dmaon = false;
        bool row_dsken_true_dmaon = false;
        for (int i = 0; i < 2; ++i) {
            bool dsken_bit = (i == 1);
            Rig rig;
            rig.do_reset();
            rig.write_dmacon(true, dsken_bit, true);  // DMACON.DMAEN(master) always true; DSKEN varies.
            rig.dsklen_write_double(DSKLEN_ARM_BIT | DSKLEN_BODY);
            rig.settle_edges(4);
            uint16_t v = rig.read_dskbytr();
            bool measured = dmaon_bit(v);
            checks.note("A2-ROW DSKEN=" + std::to_string(dsken_bit) + " (DSKLEN.DMAEN=1, DMACON.DMAEN=1 fixed): "
                        "DSKBYTR=0x" + hex4(v) + " DMAON=" + std::to_string(measured));
            if (dsken_bit) row_dsken_true_dmaon = measured;
            else           row_dsken_false_dmaon = measured;
        }

        // p.278 predicts DMAON=1 in both rows (DSKEN not named, therefore
        // irrelevant to the page's own two-term AND).
        checks.expect(row_dsken_false_dmaon && row_dsken_true_dmaon,
            "A2-DMAON_P278_TWO_TERM_COMPOSITION (p.278, opus:8-appA-dskbytr.c11 / sol:8-appa-dskbytr.c11, quote "
            "'                 14    DMAON      Mirror of bit 15 (DMAEN) in  DSKLEN ,\\n                          "
            "        ANDed with Bit09 (DMAEN) in  DMACON'): graded as printed -- with DSKLEN.DMAEN=1 and "
            "DMACON.DMAEN(master)=1 held, this page's own two-term reading predicts DMAON=1 regardless of "
            "DMACON.DSKEN, since DSKEN is not one of the two named terms. Measured: DMAON follows DSKEN instead "
            "(0 when DSKEN=0, 1 when DSKEN=1) -- the DSKEN=0 row contradicts the page as printed. This is the "
            "page-versus-page result named in SPECIFICATION.md and finding candidate C11: p.278 prints an "
            "incomplete restatement of what p.248 (A1 above, green) states in full. The red here is not a defect "
            "in NanoMig; it is this page omitting a term the chapter's own p.248 names.");
    }

    // ========================================================================
    // A2-SUBSTITUTE-TWO-TERM-COMPARATOR-LIVE -- mutation-demonstration only,
    // no manual citation, no claim uid. Substitute rule (build brief): a
    // stimulus mutation on an already-red assertion (A2) proves nothing. Put
    // the SAME two-term-AND-predicts-output comparator SHAPE through a
    // DIFFERENT pair that DOES fully determine DMAON -- DSKLEN.DMAEN and
    // DMACON.DSKEN, with DMACON's master DMAEN pinned true throughout (so,
    // per A1's own proven three-term composition, DMAON must exactly follow
    // DSKEN once the other two terms are true) -- and show it goes green.
    // ========================================================================
    {
        // MUTATE=A2S: the DSKEN=false row's own stimulus is overridden to
        // also set DSKEN true, while the check below still expects that
        // row's DMAON to read 0.
        const bool mutA2S = mutating("A2S");

        bool row_dsken_false_dmaon = false;
        bool row_dsken_true_dmaon = false;
        for (int i = 0; i < 2; ++i) {
            bool intended_dsken = (i == 1);
            bool stim_dsken = (i == 0 && mutA2S) ? true : intended_dsken;
            Rig rig;
            rig.do_reset();
            rig.write_dmacon(true, stim_dsken, true);  // master DMAEN pinned true throughout.
            rig.dsklen_write_double(DSKLEN_ARM_BIT | DSKLEN_BODY);
            rig.settle_edges(4);
            uint16_t v = rig.read_dskbytr();
            bool measured = dmaon_bit(v);
            checks.note("A2-SUBSTITUTE-ROW intended_dsken=" + std::to_string(intended_dsken) + " stim_dsken=" +
                        std::to_string(stim_dsken) + ": DSKBYTR=0x" + hex4(v) + " DMAON=" + std::to_string(measured));
            if (i == 1) row_dsken_true_dmaon = measured;
            else        row_dsken_false_dmaon = measured;
        }

        checks.expect(!row_dsken_false_dmaon && row_dsken_true_dmaon,
            "A2-SUBSTITUTE-TWO-TERM-COMPARATOR-LIVE (mutation-demonstration only, no manual citation): the SAME "
            "two-term-AND-predicts-output comparator shape as A2's own check, applied here to DSKLEN.DMAEN and "
            "DMACON.DSKEN (with DMACON.DMAEN(master) pinned true throughout, per A1's own proven three-term "
            "composition), grades green -- DMAON follows DSKEN exactly (0 then 1) -- the comparator apparatus is "
            "not simply broken or vacuously red; A2's own red above is a real, measured divergence between p.278 "
            "and p.248, not a stuck check");
    }

    // ========================================================================
    // A3 -- sol:8-dskbytr.c11 (p.248). Sub-term necessity: DSKLEN.DMAEN is
    // one of DMAON's required terms. Reference sub-case (all three true) vs
    // target sub-case (DSKLEN.DMAEN held false via a single, un-armed
    // write, the other two true).
    // ========================================================================
    {
        bool ref_dmaon, target_dmaon;
        {
            Rig rig;
            rig.do_reset();
            rig.write_dmacon(true, true, true);
            rig.dsklen_write_double(DSKLEN_ARM_BIT | DSKLEN_BODY);
            rig.settle_edges(4);
            uint16_t v = rig.read_dskbytr();
            ref_dmaon = dmaon_bit(v);
            checks.note("A3-REFERENCE (all three true): DSKBYTR=0x" + hex4(v) + " DMAON=" + std::to_string(ref_dmaon));
        }
        {
            // MUTATE=A3: the single write is followed by a second,
            // accidentally arming DSKLEN.DMAEN.
            const bool mutA3 = mutating("A3");
            Rig rig;
            rig.do_reset();
            rig.write_dmacon(true, true, true);
            rig.dsklen_write_single(DSKLEN_ARM_BIT | DSKLEN_BODY);
            if (mutA3) rig.dsklen_write_single(DSKLEN_ARM_BIT | DSKLEN_BODY);
            rig.settle_edges(4);
            uint16_t v = rig.read_dskbytr();
            target_dmaon = dmaon_bit(v);
            checks.note("A3-TARGET (DSKLEN.DMAEN held false via single write, " + std::to_string(mutA3 ? 2 : 1) +
                        " write(s) applied): DSKBYTR=0x" + hex4(v) + " DMAON=" + std::to_string(target_dmaon));
        }
        checks.expect(ref_dmaon && !target_dmaon,
            "A3-DMAON_REQUIRES_DSKLEN_DMAEN (p.248, sol:8-dskbytr.c11, quote 'This means the\\n                    "
            "    DMAEN bit in  DSKLEN , and the DSKEN & DMAEN bits\\n                        in  DMACON .'): with "
            "DMACON.DSKEN and DMACON.DMAEN(master) both held true, DMAON reads 1 when DSKLEN.DMAEN is armed "
            "(double-written) and reads 0 when it is not (single write only) -- DSKLEN.DMAEN is a required term, "
            "not a formality. Both p.248 and p.278 agree on this specific term; it is not part of the "
            "page-versus-page divergence A1/A2 grade.");
    }

    // ========================================================================
    // A4 -- sol:8-dskbytr.c12 (p.248, flagged out_of_slice by the
    // classifier; the half graded here is the disk-side consequence at this
    // fixture's data_out, per the claims file's own evidence). Sub-term
    // necessity: DMACON.DSKEN is one of DMAON's required terms -- the exact
    // term p.278's two-term reading (A2) omits.
    // ========================================================================
    {
        bool ref_dmaon, target_dmaon;
        {
            Rig rig;
            rig.do_reset();
            rig.write_dmacon(true, true, true);
            rig.dsklen_write_double(DSKLEN_ARM_BIT | DSKLEN_BODY);
            rig.settle_edges(4);
            uint16_t v = rig.read_dskbytr();
            ref_dmaon = dmaon_bit(v);
            checks.note("A4-REFERENCE (all three true): DSKBYTR=0x" + hex4(v) + " DMAON=" + std::to_string(ref_dmaon));
        }
        {
            // MUTATE=A4: the target's own DMACON write also sets DSKEN.
            const bool mutA4 = mutating("A4");
            Rig rig;
            rig.do_reset();
            rig.write_dmacon(true, mutA4 ? true : false, true);  // DSKEN held false unless mutated; master DMAEN true.
            rig.dsklen_write_double(DSKLEN_ARM_BIT | DSKLEN_BODY);
            rig.settle_edges(4);
            uint16_t v = rig.read_dskbytr();
            target_dmaon = dmaon_bit(v);
            checks.note("A4-TARGET (DMACON.DSKEN held false" + std::string(mutA4 ? ", MUTATED to true" : "") +
                        "): DSKBYTR=0x" + hex4(v) + " DMAON=" + std::to_string(target_dmaon));
        }
        checks.expect(ref_dmaon && !target_dmaon,
            "A4-DMAON_REQUIRES_DMACON_DSKEN (p.248, sol:8-dskbytr.c12, quote 'This means the\\n                   "
            "     DMAEN bit in  DSKLEN , and the DSKEN & DMAEN bits\\n                        in  DMACON .'): with "
            "DSKLEN.DMAEN armed and DMACON.DMAEN(master) held true, DMAON reads 1 when DMACON.DSKEN is also true "
            "and reads 0 when it is not -- DMACON.DSKEN is a required term. This is the specific term p.278's "
            "two-term reading (A2) omits, and is the crux of the page-versus-page divergence: p.248 names it, "
            "p.278 does not.");
    }

    // ========================================================================
    // A5 -- sol:8-dskbytr.c13 (p.248, flagged out_of_slice, same reason as
    // A4) / sol:8-appa-dskbytr.c12 (p.278, "DMACON bit 9 is the global
    // DMAEN term used in the DSKBYTR.DMAON indication" -- both pages agree
    // on this specific term). Sub-term necessity: DMACON.DMAEN(master) is
    // one of DMAON's required terms.
    // ========================================================================
    {
        bool ref_dmaon, target_dmaon;
        {
            Rig rig;
            rig.do_reset();
            rig.write_dmacon(true, true, true);
            rig.dsklen_write_double(DSKLEN_ARM_BIT | DSKLEN_BODY);
            rig.settle_edges(4);
            uint16_t v = rig.read_dskbytr();
            ref_dmaon = dmaon_bit(v);
            checks.note("A5-REFERENCE (all three true): DSKBYTR=0x" + hex4(v) + " DMAON=" + std::to_string(ref_dmaon));
        }
        {
            // MUTATE=A5: the target's own DMACON write also sets the
            // master DMAEN bit.
            const bool mutA5 = mutating("A5");
            Rig rig;
            rig.do_reset();
            rig.write_dmacon(true, true, mutA5 ? true : false);  // DSKEN true; master DMAEN held false unless mutated.
            rig.dsklen_write_double(DSKLEN_ARM_BIT | DSKLEN_BODY);
            rig.settle_edges(4);
            uint16_t v = rig.read_dskbytr();
            target_dmaon = dmaon_bit(v);
            checks.note("A5-TARGET (DMACON.DMAEN(master) held false" + std::string(mutA5 ? ", MUTATED to true" : "") +
                        "): DSKBYTR=0x" + hex4(v) + " DMAON=" + std::to_string(target_dmaon));
        }
        checks.expect(ref_dmaon && !target_dmaon,
            "A5-DMAON_REQUIRES_DMACON_DMAEN (p.248, sol:8-dskbytr.c13, quote 'This means the\\n                   "
            "     DMAEN bit in  DSKLEN , and the DSKEN & DMAEN bits\\n                        in  DMACON .'; "
            "p.278, sol:8-appa-dskbytr.c12, quote '                 14    DMAON      Mirror of bit 15 (DMAEN) in  "
            "DSKLEN ,\\n                                  ANDed with Bit09 (DMAEN) in  DMACON'): with DSKLEN.DMAEN "
            "armed and DMACON.DSKEN held true, DMAON reads 1 when DMACON.DMAEN(master) is also true and reads 0 "
            "when it is not -- DMACON's master DMAEN bit is a required term, agreed by both pages.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
