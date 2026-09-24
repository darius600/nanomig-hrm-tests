// aa-disk-length-and-status -- paula_floppy conformance test
//
// Test purpose: the AA specification's address-ordered register list gives
//   DSKBYTR, DSKLEN and DSKSYNC their addresses and access letters, and this
//   is the module that decodes all three. It grades the two whose effect
//   reaches this boundary, and says in the lot why the third does not.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 6 and 7, the
//   address-ordered register list. The document prints no page numbers of its
//   own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the disk data path, sector decoding,
//   MFM, the sd-card back end, head stepping, or the drive sense lines. It
//   grades register decoding and two status bits.
//
// DSKBYTR IS THE ONLY READ PATH THIS MODULE OFFERS FOR ITS OWN STATE, and it
//   is a pure combinational word (paula_floppy.v:699):
//
//     dskbytr = address decodes ? {1'b1, (trackrd|trackwr), dsklen[14],
//                                  5'b1_0000, 8'h00} : 16'h0000
//
//   so out of reset it reads $9000 -- bit 15 forced (the 2010-08-14 note says
//   Kick Off 2's loader needs BYTEREADY set) and bit 12 forced (the 2009-07-21
//   note says WORDEQUAL is always set). Bit 14 follows the disk state machine
//   and bit 13 mirrors DSKLEN's own bit 14, which is how a write at one
//   address is observed at another.
//
// THE LOW BYTE IS WHERE THIS RTL AND THE DOCUMENT PART COMPANY. The list calls
//   DSKBYTR a `Disk Data byte and status register`, and the status half is
//   here, bit for bit. The data byte is not: DSKBYTR[7:0] is the constant
//   `8'h00`, and disk data reaches the bus through DSKDATR instead. This
//   package does not grade that claim green on the strength of the half it can
//   see -- the lot declines it and names the measurement, and what the
//   difference means is for Amiga people to decide.
//
// DSKLEN ARMS ON THE SECOND WRITE, which is why assertion 4 writes twice and
//   assertion 3 does not care. paula_floppy.v:742 is
//
//     dmaen <= data_in[15] & dsklen[15];
//
//   so the value already latched by the first write and the value arriving on
//   the second must both carry bit 15. A single write loads the length and the
//   direction and arms nothing, and DSKBYTR shows exactly that difference.
//
// `enable` IS AN INPUT AT THIS BOUNDARY. In paula it is `dmacon[4] & dmaen`,
//   and aa-audio-disk-control-registers grades it from there. Here it is a pin
//   this fixture holds high, so DSKLEN's own arming can be seen without
//   DMACON in the way.

#include "Vpaula_floppy.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>

namespace {
// Per-assertion mutation selector, the same form the rest of the suite uses:
// MUTATE names ONE assertion's own label, and only that assertion's stimulus
// site changes. Every OTHER assertion stays on its real stimulus and is a
// live control on each run.
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
}  // namespace

static std::string hex(uint32_t v) {
    std::ostringstream o;
    o << "0x" << std::hex << std::uppercase << v;
    return o.str();
}

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

// Word addresses as the AA register list prints them, read from the corpus.
constexpr int A_DSKBYTR = 0x01A >> 1;   // paula_floppy.v:120
constexpr int A_DSKLEN  = 0x024 >> 1;   // paula_floppy.v:124
constexpr int A_DSKSYNC = 0x07E >> 1;   // paula_floppy.v:123
constexpr int A_IDLE    = 0x000;        // decodes to no register in this module

constexpr uint16_t IDLE_STATUS = 0x9000;  // BYTEREADY and WORDEQUAL, both forced
constexpr uint16_t DSKWRITE    = 1u << 13;
constexpr uint16_t DMAON       = 1u << 14;

// One fresh Verilated model per trial: dsktrack carries no reset term anywhere
// in paula_floppy.v, so a reset pulse is not enough to guarantee a known start.
class Harness {
public:
    Harness() : dut(new Vpaula_floppy) { reset(); }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

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

    void settle(int n) {
        int seen = 0;
        while (seen < n) {
            bool q = dut->clk7_en;
            tick();
            if (q) seen++;
        }
    }

    void reset() {
        clk7_phase = 0;
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->clk7n_en = 0;
        dut->reset = 1;
        dut->ntsc = 0;
        dut->sof = 0;
        dut->enable = 1;      // an input here; DMACON's half is graded elsewhere
        dut->reg_address_in = A_IDLE;
        dut->data_in = 0;
        dut->_step = 1;       // inactive (active low): no head stepping
        dut->direc = 0;
        dut->_sel = 0xE;      // drive 0 selected
        dut->side = 0;
        dut->_motor = 1;      // inactive
        dut->wordsync = 0;
        dut->floppy_drives = 0;
        dut->floppy_wrprot = 0;
        dut->sdc_image_mounted = 0;
        dut->sdc_image_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        settle(8);
        dut->reset = 0;
        settle(8);
    }

    void write(int addr, uint16_t value) {
        dut->reg_address_in = addr;
        dut->data_in = value;
        settle(1);
        dut->reg_address_in = A_IDLE;
        dut->data_in = 0;
        settle(2);
    }

    uint16_t read(int addr) {
        dut->reg_address_in = addr;
        dut->eval();
        const uint16_t got = (uint16_t)dut->data_out;
        dut->reg_address_in = A_IDLE;
        dut->eval();
        return got;
    }

    void write_n(int addr, uint16_t value, int times) {
        for (int i = 0; i < times; ++i) write(addr, value);
    }

private:
    Vpaula_floppy* dut;
    int clk7_phase = 0;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // MUTATION. Each guard below tests its own assertion's label and changes
    // ONE thing: a stimulus that misses the mechanism, or an expected word
    // the document does not print. MUTATE=<label> selects exactly one.
    const int      m_fix   = mutating("FIXTURE-DELIVERS-KNOWN-VALUE") ? A_IDLE : A_DSKBYTR;
    const int      m_bytr  = mutating("DSKBYTR-AT-01A") ? A_DSKSYNC : A_DSKBYTR;
    const int      m_len_w = mutating("DSKLEN-AT-024-SHOWS-ITS-DIRECTION") ? A_DSKSYNC : A_DSKLEN;
    const int      m_arm   = mutating("DSKLEN-ARMS-ON-THE-SECOND-WRITE") ? 1 : 2;
    const uint16_t m_ro    = mutating("DSKBYTR-TAKES-NO-WRITES") ? 0x1234u : IDLE_STATUS;
    // Redirects the write from DSKBYTR to DSKLEN with bit 14 set (the same
    // 0x4001 payload DSKLEN-AT-024-SHOWS-ITS-DIRECTION uses below), which
    // paula_floppy.v:699 mirrors onto DSKBYTR bit 13 (DSKWRITE). Expected
    // word left at IDLE_STATUS.
    const int      m_ro_wtgt = mutating("DSKBYTR-TAKES-NO-WRITES-STIMULUS") ? A_DSKLEN : A_DSKBYTR;
    const uint16_t m_ro_wval = mutating("DSKBYTR-TAKES-NO-WRITES-STIMULUS") ? 0x4001u  : 0x1234u;

    std::cout << "aa-disk-length-and-status -- AA_CONFORMANCE at paula_floppy\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red, the "
                      "others must stay on their baseline verdict\n";

    // --- 1. The fixture delivers a known value ------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: selecting this
        // assertion's own id reads an address this module decodes for
        // nothing, which answers zero.
        const uint16_t got = h.read(m_fix);
        checks.note("DSKBYTR out of reset reads " + hex(got));
        checks.expect(got == IDLE_STATUS,
                      "FIXTURE-DELIVERS-KNOWN-VALUE: the status register reads "
                      "0x9000 out of reset -- BYTEREADY and WORDEQUAL forced, "
                      "the two state bits clear -- so every assertion below "
                      "starts from a word this fixture can account for");
    }

    // --- 2. DSKBYTR answers at $01A and nowhere else -------------------------
    {
        Harness h;
        // MUTATION DSKBYTR-AT-01A stimulus red: selecting this assertion's
        // own id reads $07E, which the list marks W and this module gives
        // no read path.
        const uint16_t at_01a = h.read(m_bytr);
        const uint16_t at_idle = h.read(A_IDLE);
        const uint16_t at_07e = h.read(A_DSKSYNC);
        checks.note("$01A reads " + hex(at_01a) + ", an undecoded address " +
                    hex(at_idle) + ", $07E " + hex(at_07e));
        checks.expect(at_01a == IDLE_STATUS && at_idle == 0 && at_07e == 0,
                      "DSKBYTR-AT-01A: the status word answers at $01A and no "
                      "other address in this module returns it, which is the "
                      "address the AA list prints for DSKBYTR [p.6] "
                      "[claims: opus:AA-dskbytr.c1 sol:AA-dskbytr.c1]");
    }

    // --- 3. DSKLEN's direction bit is visible in DSKBYTR ---------------------
    {
        Harness h;
        // MUTATION DSKLEN-AT-024-SHOWS-ITS-DIRECTION stimulus red: selecting
        // this assertion's own id writes the word to $07E, so dsklen never
        // changes and the status bit stays clear.
        h.write(m_len_w, 0x4001);                 // write direction, length 1
        const uint16_t writing = h.read(A_DSKBYTR);
        h.write(A_DSKLEN, 0x0001);                // read direction, length 1
        const uint16_t reading = h.read(A_DSKBYTR);
        checks.note("after a DSKLEN write with bit 14 set: " + hex(writing) +
                    ", with it clear: " + hex(reading));
        checks.expect((writing & DSKWRITE) && !(reading & DSKWRITE),
                      "DSKLEN-AT-024-SHOWS-ITS-DIRECTION: a word written at "
                      "$024 turns a bit of the status register on and off, "
                      "which is the address the AA list prints for DSKLEN "
                      "[p.6] [claims: opus:AA-dsklen.c1 sol:AA-dsklen.c1]");
    }

    // --- 4. And the write only arms the transfer on the second one -----------
    {
        Harness h;
        // MUTATION DSKLEN-ARMS-ON-THE-SECOND-WRITE stimulus red: selecting
        // this assertion's own id writes once: paula_floppy.v:742 needs bit
        // 15 in the latched value AND in the arriving one, so one write
        // arms nothing and DMAON never rises.
        h.write_n(A_DSKLEN, 0x8001, 1);
        const uint16_t once = h.read(A_DSKBYTR);
        h.reset();
        h.write_n(A_DSKLEN, 0x8001, m_arm);
        h.settle(4);
        const uint16_t twice = h.read(A_DSKBYTR);
        checks.note("after one write: " + hex(once) + ", after two: " + hex(twice));
        checks.expect(!(once & DMAON) && (twice & DMAON),
                      "DSKLEN-ARMS-ON-THE-SECOND-WRITE: the same word written "
                      "twice starts a transfer that one write does not, so the "
                      "address takes writes and returns nothing, which is the W "
                      "access letter the AA list prints for DSKLEN [p.6] "
                      "[claims: opus:AA-dsklen.c2 sol:AA-dsklen.c2]");
    }

    // --- 5. DSKBYTR takes no writes -----------------------------------------
    {
        Harness h;
        // MUTATION DSKBYTR-TAKES-NO-WRITES oracle red: selecting this
        // assertion's own id expects the written word back, which is the
        // reading this assertion rules out.
        // MUTATION DSKBYTR-TAKES-NO-WRITES stimulus red: redirects the write
        // from $01A to $024 (DSKLEN) with bit 14 set instead, which mirrors
        // onto DSKBYTR bit 13 (DSKWRITE)
        // (MUTATE=DSKBYTR-TAKES-NO-WRITES-STIMULUS: this line alone goes FAIL, 4 passed, 1 failed).
        h.write(m_ro_wtgt, m_ro_wval);
        const uint16_t got = h.read(A_DSKBYTR);
        checks.note("after writing " + hex(m_ro_wval) + " at " +
                    (m_ro_wtgt == A_DSKBYTR ? "$01A" : "$024") +
                    " DSKBYTR reads " + hex(got));
        checks.expect(got == m_ro,
                      "DSKBYTR-TAKES-NO-WRITES: a word written at $01A does not "
                      "come back from it, so the address is the read the AA "
                      "list's R access letter names and not a store [p.6] "
                      "[claims: opus:AA-dskbytr.c2 sol:AA-dskbytr.c2]");
    }

    // --- What this boundary cannot grade ------------------------------------
    checks.record(
             "DECLINED at this boundary (insufficient): DSKSYNC takes a write at "
             "$07E and offers no read, so nothing about it is visible without "
             "acting on it. Its only consumer is `sync_match` at "
             "paula_floppy.v:915, which compares the pattern against a word "
             "arriving from a disk read. [claims: opus:AA-dsksync.c1/c2/c5 "
             "sol:AA-dsksync.c1/c2/c5]");
    checks.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "chip column, which says which 1985 silicon a register lives in. No "
             "port of any module in this design carries a chip identity. "
             "[claims: opus:AA-adkcon-entry.c3 opus:AA-adkconr-entry.c3]");
    checks.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. Which silicon a block is fabricated "
             "in is a fact about the source tree and no port of paula_floppy "
             "carries it. [claims: opus:AA-dskbytr.c3 opus:AA-dsklen.c3 "
             "opus:AA-dsksync.c3 sol:AA-dskbytr.c3 sol:AA-dsklen.c3 "
             "sol:AA-dsksync.c3]");
    checks.record(
             "DECLINED at this boundary (insufficient): The description column "
             "names two things and this RTL answers one. The status half is "
             "graded by DSKBYTR-AT-01A and the two DSKLEN assertions. MEASURED: "
             "paula_floppy.v:699 drives DSKBYTR[7:0] from the constant 8'h00, "
             "and disk data reaches the bus through DSKDATR. A claim whose "
             "second half meets a constant is not a green assertion. [claims: "
             "opus:AA-dskbytr.c5 sol:AA-dskbytr.c5]");
    checks.record(
             "DECLINED at this boundary (insufficient): The register is decoded "
             "at this boundary, but no assertion in this package writes or reads "
             "it. The package grades the rows its own subject names; this row is "
             "another register of the same module. [claims: "
             "opus:AA-adkcon-bits.c1/c2/c3/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18 "
             "opus:AA-adkcon-entry.c1/c2/c4 opus:AA-adkconr-entry.c1/c2/c5]");
    checks.record(
             "DECLINED at this boundary (not-hardware): The claim is a legend "
             "statement, a gap between two printed rows, or a reserved line -- "
             "the document describing its own notation and layout. The one "
             "legend mark that IS a hardware restriction, the `~` Coprocessor "
             "limit, is graded by this package's own assertion. [claims: "
             "opus:AA-adkconr-entry.c4]");
    checks.record(
             "DECLINED at this boundary (outside-uut): The `~` mark is a "
             "Coprocessor write restriction enforced in agnus_copper's cdang "
             "path and graded by aa-copper-register-map. [claims: "
             "opus:AA-dskbytr.c4 opus:AA-dsklen.c4 opus:AA-dsksync.c4 "
             "sol:AA-dskbytr.c4 sol:AA-dsklen.c4 sol:AA-dsksync.c4]");

    std::cout << "\nAssertions: " << checks.passed << " passed, "
              << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
