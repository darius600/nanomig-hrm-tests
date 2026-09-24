// aa-disk-status-bits -- paula_floppy conformance test
//
// Test purpose: the AA specification's DSKBYTR entry prints a five-field bit
//   table -- DSKBYT at 15, DMAON at 14, DISKWRITE at 13, WORDEQUAL at 12,
//   four unused cells at 11 to 8 and the disk byte at 7 to 0 -- and says in
//   prose what sets and clears the first of them. This grades the fields that
//   can be reached by writing DSKLEN and reading $01A.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF page 22, the DSKBYTR entry and
//   its bit table. The document prints no page numbers of its own; the corpus
//   numbering is the PDF's.
// Non-goals: the disk byte itself -- putting a real byte into bits 7 to 0
//   means driving this module's SD-card side, which `hrm/ch8-dskbytr-status-
//   register` already does -- the DMACON half of DMAON, which is not a port
//   here, and everything about DSKDAT, DSKDATR and the FIFO.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the AA specification independently, and ids such as
//   `opus:AA-dskbytr.c9` and `opus:AA-dsklen.c6` each name one reader's
//   reading of one sentence. They resolve in the claim files that
//   hrm-testpipeline/acceptance/aa-s1.json lists under `inventories`.
//
// THIS PACKAGE CORROBORATES NM-HRM-041 FROM A SECOND DOCUMENT. That finding
//   was raised against the Hardware Reference Manual: DSKBYTR is a constant
//   word in three of its five fields. The AA specification, written for a
//   different chip set five years later, prints the same behaviour for two of
//   them, and the two reds below are the second document saying it. They are
//   recorded on NM-HRM-041's page rather than filed as an AA finding of their
//   own -- a corroboration is not a new key.
//
// WHAT THE MODULE READS BACK. One continuous assignment, and four of the six
//   fields are constants in it:
//
//     assign dskbytr = reg_address_in[8:1]==DSKBYTR[8:1]
//                    ? {1'b1,(trackrd|trackwr),dsklen[14],5'b1_0000,8'h00}
//                    : 16'h00_00;                                    (:699)
//
//   Bit 15 is the literal 1, bit 12 is the literal 1, bits 11 to 8 are
//   literal zeros and so are bits 7 to 0. Only bits 14 and 13 carry state.
//   The module's own history notes say so: ":60 WORDEQUAL in DSKBYTR register
//   is always set now" and ":65 set BYTEREADY of DSKBYTR (required by Kick
//   Off 2 loader)".
//
// WHY A READ IS TAKEN ACROSS A CLOCK EDGE. `data_out` is combinational, so a
//   bare eval() would read the register without ever presenting a bus cycle
//   the module could react to -- and "cleared when read" is exactly a
//   reaction. Every read below holds the address across a qualifying clk7_en
//   edge, the same shape every write in this module is gated on, so a
//   synchronous clear would have a real chance to happen.

#include "Vpaula_floppy.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
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

// Word addresses as the AA alphabetical entries print them.
// The port is `reg_address_in[8:1]`, so what it carries is the word index,
// not the byte address the page prints. Passing $01A here reads nothing and
// says nothing about why.
constexpr int A_DSKBYTR = 0x01A >> 1;
constexpr int A_DSKSYNC = 0x07E >> 1;
constexpr int A_DSKLEN  = 0x024 >> 1;

constexpr uint16_t DMAEN = 1u << 15;   // DSKLEN bit 15
constexpr uint16_t WRITE = 1u << 14;   // DSKLEN bit 14
constexpr uint16_t SYNC_WORD = 0x4489; // the standard MFM sync the page names

class Harness {
public:
    Harness() : dut(new Vpaula_floppy) {
        dut->clk = 0; dut->clk7_en = 0; dut->clk7n_en = 0;
        dut->reset = 1; dut->ntsc = 0; dut->sof = 0;
        dut->enable = 1;          // DMACON's own product, held at this boundary
        dut->reg_address_in = 0; dut->data_in = 0;
        dut->_step = 1; dut->direc = 1; dut->_sel = 0xF; dut->side = 0;
        dut->_motor = 1; dut->wordsync = 0;
        dut->floppy_drives = 1; dut->floppy_wrprot = 0;
        dut->sdc_image_mounted = 0; dut->sdc_image_size = 0;
        dut->sdc_busy = 0; dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0; dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        for (int i = 0; i < 32; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 32; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
        phase = (phase + 1) % 4;
        dut->clk7_en = (phase == 0);
        dut->clk7n_en = (phase == 2);
    }
    void ticks(int n) { for (int i = 0; i < n; ++i) tick(); }

    void write_reg(int addr_w, uint16_t data) {
        dut->reg_address_in = (uint16_t)(addr_w & 0x1FF);
        dut->data_in = data;
        ticks(4);
        dut->reg_address_in = 0;
        dut->data_in = 0;
        ticks(4);
    }

    // A read that is a bus cycle, not a peek: the address is held across a
    // qualifying edge, so a synchronous read-side-effect could fire.
    uint16_t read_clocked(int addr_w) {
        dut->reg_address_in = (uint16_t)(addr_w & 0x1FF);
        dut->data_in = 0;
        while (!dut->clk7_en) tick();
        tick();
        const uint16_t v = (uint16_t)dut->data_out;
        dut->reg_address_in = 0;
        ticks(4);
        return v;
    }

    // DSKLEN arms on the second matching write; aa-disk-length-and-status
    // grades that rule itself, and this package relies on it.
    void arm_dsklen(uint16_t word) {
        write_reg(A_DSKLEN, word);
        write_reg(A_DSKLEN, word);
    }

    Vpaula_floppy* dut;
    int phase = 0;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    g_mutate = std::getenv("MUTATE");

    // MUTATION. Each guard below tests its own assertion's label. The four
    // green assertions are guarded so selecting their own id forces them
    // red; the two red ones (already red at baseline -- they corroborate
    // NM-HRM-041) are guarded the other way, so selecting their own id asks
    // the module for what it does instead of what the page says and they go
    // green -- a mutation that made a red redder would prove nothing.
    // MUTATE=<label> selects exactly one.
    const uint16_t m_write = mutating("DISKWRITE-MIRRORS-DSKLEN-WRITE") ? (uint16_t)0u : WRITE;
    const uint16_t m_arm   = mutating("DMAON-FOLLOWS-THE-DSKLEN-ENABLE") ? (uint16_t)0u : DMAEN;
    const uint16_t m_mask  = mutating("DSKBYTR-BITS-11-TO-8-READ-ZERO")
                                  ? (uint16_t)0xF000u : (uint16_t)0x0F00u;

    std::cout << "aa-disk-status-bits -- AA_CONFORMANCE at paula_floppy\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must move off its "
                      "baseline verdict, the others must stay on theirs\n";

    // --- 1. The register answers, and its answer moves ----------------------
    {
        Harness h;
        const uint16_t idle = h.read_clocked(A_DSKBYTR);
        // MUTATION FIXTURE-READS-A-LIVE-REGISTER stimulus red: selecting
        // this assertion's own id arms DSKLEN with a zero word, so nothing
        // between the two reads changes and a dead read path would look the
        // same as a live one.
        h.arm_dsklen(mutating("FIXTURE-READS-A-LIVE-REGISTER")
                          ? (uint16_t)0u
                          : (uint16_t)(DMAEN | WRITE | 0x0010));
        const uint16_t armed = h.read_clocked(A_DSKBYTR);
        c.note("DSKBYTR reads " + hex(idle) + " idle and " + hex(armed) +
               " after DSKLEN is armed for a write");
        c.expect(idle != armed,
                 "FIXTURE-READS-A-LIVE-REGISTER: the word at $01A changes when "
                 "DSKLEN changes, so every field measured below is read from a "
                 "register this fixture reached [no page -- infrastructure]");
    }

    // --- 2. DISKWRITE is DSKLEN's WRITE bit ---------------------------------
    {
        Harness h;
        h.arm_dsklen((uint16_t)(DMAEN | 0x0010));            // read direction
        const uint16_t reading = h.read_clocked(A_DSKBYTR);
        // MUTATION DISKWRITE-MIRRORS-DSKLEN-WRITE stimulus red: selecting
        // this assertion's own id arms DSKLEN for a read in both trials, so
        // bit 13 stays low in both and the two directions are no longer
        // told apart.
        h.arm_dsklen((uint16_t)(DMAEN | m_write | 0x0010));  // write direction
        const uint16_t writing = h.read_clocked(A_DSKBYTR);
        c.note("DSKBYTR bit 13 reads " + std::string((reading >> 13) & 1 ? "1" : "0") +
               " with DSKLEN's WRITE bit clear and " +
               std::string((writing >> 13) & 1 ? "1" : "0") + " with it set");
        c.expect(((reading >> 13) & 1) == 0 && ((writing >> 13) & 1) == 1,
                 "DISKWRITE-MIRRORS-DSKLEN-WRITE: bit 13 of DSKBYTR is bit 14 "
                 "of DSKLEN, which is what the AA entry means by calling it "
                 "DISKWRITE [p.22] [claims: opus:AA-dskbytr.c9 opus:AA-dsklen.c6/c8 sol:AA-dskbytr.c10 sol:AA-dsklen.c8/c10]");
    }

    // --- 3. DMAON follows the enable DSKLEN carries -------------------------
    {
        Harness h;
        const uint16_t idle = h.read_clocked(A_DSKBYTR);
        // MUTATION DMAON-FOLLOWS-THE-DSKLEN-ENABLE stimulus red: selecting
        // this assertion's own id arms DSKLEN with the enable bit clear, so
        // the engine never leaves idle and bit 14 reads the same in both
        // trials.
        h.arm_dsklen((uint16_t)(m_arm | 0x0010));
        const uint16_t armed = h.read_clocked(A_DSKBYTR);
        c.note("DSKBYTR bit 14 reads " + std::string((idle >> 14) & 1 ? "1" : "0") +
               " before DSKLEN is armed and " +
               std::string((armed >> 14) & 1 ? "1" : "0") + " after");
        c.expect(((idle >> 14) & 1) == 0 && ((armed >> 14) & 1) == 1,
                 "DMAON-FOLLOWS-THE-DSKLEN-ENABLE: bit 14 rises with the DMAEN "
                 "bit of DSKLEN, which is one of the three terms the AA entry "
                 "ANDs into DMAON -- the other two are DMACON's, and DMACON is "
                 "not a port of this module [p.22] [claims: opus:AA-dskbytr.c1/c2/c8 opus:AA-dsklen.c1/c7 sol:AA-dskbytr.c1/c9 sol:AA-dsklen.c1/c7/c9]");
    }

    // --- 4. The four unused cells read as the printed zero ------------------
    {
        Harness h;
        h.arm_dsklen((uint16_t)(DMAEN | WRITE | 0x0010));
        const uint16_t got = h.read_clocked(A_DSKBYTR);
        // MUTATION DSKBYTR-BITS-11-TO-8-READ-ZERO stimulus red: selecting
        // this assertion's own id looks at bits 15 to 12 instead, which are
        // the two constant ones and the two live ones, so the mask can
        // never be clear.
        c.note("DSKBYTR read " + hex(got) + "; the four cells the AA table "
               "leaves unused hold " +
               hex((got & m_mask) >>
                   (mutating("DSKBYTR-BITS-11-TO-8-READ-ZERO") ? 12 : 8)));
        c.expect((got & m_mask) == 0,
                 "DSKBYTR-BITS-11-TO-8-READ-ZERO: the four cells the AA bit "
                 "table prints as unused read as the zero it prints for them "
                 "[p.22] [claims: opus:AA-dskbytr.c11 sol:AA-dskbytr.c12]");
    }

    // --- 5. DSKBYT is supposed to be cleared by the read --------------------
    {
        Harness h;
        h.arm_dsklen((uint16_t)(DMAEN | 0x0010));
        const uint16_t first = h.read_clocked(A_DSKBYTR);
        const uint16_t second = h.read_clocked(A_DSKBYTR);
        // MUTATION DSKBYT-IS-CLEARED-BY-THE-READ oracle green: this
        // assertion is RED against the tree (it corroborates NM-HRM-041), so
        // its mutation is inverted -- a failing assertion cannot be made to
        // fail harder. Selecting this assertion's own id compares bit 15
        // against what the module returns rather than against the cleared
        // bit the entry promises, and the assertion goes green, which is
        // this fixture's demonstration that the comparison is live and not
        // vacuous. Baseline (unmutated) stays red, as it must.
        const int want = mutating("DSKBYT-IS-CLEARED-BY-THE-READ")
                              ? ((second >> 15) & 1) : 0;
        c.note("two consecutive bus reads of DSKBYTR return " + hex(first) +
               " and " + hex(second) + "; bit 15 is " +
               std::string((second >> 15) & 1 ? "still set" : "clear") +
               " after the first read");
        c.expect(((second >> 15) & 1) == want,
                 "DSKBYT-IS-CLEARED-BY-THE-READ: the AA entry says a loaded "
                 "disk byte sets bit 15 and that reading the register resets "
                 "it, so a second read with no byte in between must find it "
                 "clear. Here bit 15 is the literal one and no read moves it "
                 "[p.22] [claims: opus:AA-dskbytr.c7 sol:AA-dskbytr.c7/c8]");
    }

    // --- 6. WORDEQUAL is supposed to track the sync comparison --------------
    {
        Harness h;
        h.write_reg(A_DSKSYNC, SYNC_WORD);
        h.arm_dsklen((uint16_t)(DMAEN | 0x0010));
        const uint16_t got = h.read_clocked(A_DSKBYTR);
        // MUTATION WORDEQUAL-IS-TRUE-ONLY-WHILE-DSKSYNC-MATCHES oracle green:
        // this assertion is RED against the tree (it corroborates
        // NM-HRM-041), so its mutation is inverted the same way as
        // DSKBYT-IS-CLEARED-BY-THE-READ above. Selecting this assertion's
        // own id asks for the bit the module returns instead of the clear
        // one the entry requires while nothing is arriving, so the
        // assertion goes green. Baseline (unmutated) stays red, as it must.
        const int want = mutating("WORDEQUAL-IS-TRUE-ONLY-WHILE-DSKSYNC-MATCHES")
                              ? ((got >> 12) & 1) : 0;
        c.note("with DSKSYNC = " + hex(SYNC_WORD) + " and no disk data "
               "arriving at all, WORDEQUAL reads " +
               std::string((got >> 12) & 1 ? "1" : "0"));
        c.expect(((got >> 12) & 1) == want,
                 "WORDEQUAL-IS-TRUE-ONLY-WHILE-DSKSYNC-MATCHES: the AA entry "
                 "makes bit 12 true only while the incoming disk data equals "
                 "DSKSYNC, and no drive is selected, no motor is running and "
                 "no byte is arriving. Here bit 12 is the literal one [p.22] "
                 "[claims: opus:AA-dskbytr.c10 sol:AA-dskbytr.c11]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about "
             "DSKDAT or DSKDATR and the word FIFO between them. Filling that "
             "FIFO means driving this module's SD-card side and running a "
             "transfer; this package presents bus cycles and reads a status "
             "word. [claims: opus:AA-dskdat.c1/c2 "
             "opus:AA-dskdatr.c1/c2/c4/c7/c8/c9/c10 "
             "sol:AA-disk-data.c1/c2/c3/c4/c5 sol:AA-dskdat.c1/c2 "
             "sol:AA-dskdatr.c1/c2/c6]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about "
             "DSKLEN's fourteen-bit length field, which is spent by a DMA "
             "transfer. This package writes DSKLEN to arm the engine and reads a "
             "status bit; it moves no words. [claims: opus:AA-dsklen.c5/c9 "
             "sol:AA-dsklen.c5/c6/c11]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about read "
             "or write access to the register. data_out here is one OR of two "
             "address-gated words, so an address that is not decoded reads as "
             "zero exactly like one that is decoded and happens to hold zero. "
             "[claims: opus:AA-dsklen.c2 sol:AA-dskbytr.c2 sol:AA-dsklen.c2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is about real "
             "disk data -- a byte arriving, the sync word being matched, the "
             "data field carrying it. No drive is selected here and no byte "
             "arrives; hrm/ch8-dskbytr-status-register drives that path and "
             "grades the same field. [claims: opus:AA-dskbytr.c5/c6/c12 "
             "opus:AA-dsksync.c1/c2/c5 sol:AA-dskbytr.c5/c6/c13 "
             "sol:AA-dsksync.c1/c2/c5]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. No port of paula_floppy carries a "
             "chip identity. [claims: opus:AA-adkcon.c3 opus:AA-adkconr.c3 "
             "opus:AA-dskbytr.c3 opus:AA-dskdat.c3 opus:AA-dskdatr.c3 "
             "opus:AA-dsklen.c3 opus:AA-dskpth.c3/c5 opus:AA-dskptl.c3 "
             "opus:AA-dsksync.c3 sol:AA-dskbytr.c3 sol:AA-dskdat.c3 "
             "sol:AA-dskdatr.c3 sol:AA-dsklen.c3 sol:AA-dskpth.c3/c7 "
             "sol:AA-dskptl.c3 sol:AA-dsksync.c3]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is about the "
             "document -- a misprint, a conflict between two of its statements, "
             "or a legend mark. [claims: opus:AA-adkconr.c4 opus:AA-dskbytr.c4 "
             "opus:AA-dskdat.c4/c5 opus:AA-dskdatr.c5/c6 opus:AA-dsklen.c4 "
             "opus:AA-dskpth.c6/c7/c8 opus:AA-dskptl.c5/c6 opus:AA-dsksync.c4 "
             "sol:AA-dskbytr.c4 sol:AA-dskdat.c4/c5 sol:AA-dskdatr.c4/c5 "
             "sol:AA-dsklen.c4 sol:AA-dskpth.c4/c5/c6 sol:AA-dskptl.c4/c5 "
             "sol:AA-dsksync.c4]");
    c.record(
             "DECLINED at this boundary (outside-uut): ADKCON and ADKCONR are "
             "decoded in paula itself and graded there by "
             "aa-audio-disk-control-registers. [claims: "
             "opus:AA-adkcon.c1/c2/c4/c5/c6/c7/c8/c9/c10/c11/c12/c13 "
             "opus:AA-adkconr.c1/c2]");
    c.record(
             "DECLINED at this boundary (outside-uut): The disk pointer is "
             "decoded in agnus_diskdma, not in the floppy controller, and is "
             "graded there by aa-disk-pointer-pair. [claims: "
             "opus:AA-dskpth.c1/c2/c4/c9/c10 opus:AA-dskptl.c1/c2/c4 "
             "sol:AA-disk-pointer.c1/c2 sol:AA-dskpth.c1/c2 "
             "sol:AA-dskptl.c1/c2/c6]");

    std::cout << "\nAssertions: " << c.passed << " passed, " << c.failed
              << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
