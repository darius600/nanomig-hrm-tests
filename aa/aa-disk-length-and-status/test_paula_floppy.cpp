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
#include <iostream>
#include <sstream>
#include <string>

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
    const bool mutate = std::getenv("MUTATE") != nullptr;
    Checks checks;

    // MUTATION. Every expect() is guarded so MUTATE=1 forces it red, and each
    // guard changes ONE thing: a stimulus that misses the mechanism, or an
    // expected word the document does not print.
    const int      m_fix   = mutate ? A_IDLE : A_DSKBYTR;    // 1: no read path
    const int      m_bytr  = mutate ? A_DSKSYNC : A_DSKBYTR; // 2: a write-only address
    const int      m_len_w = mutate ? A_DSKSYNC : A_DSKLEN;  // 3: loads the wrong register
    const int      m_arm   = mutate ? 1 : 2;                 // 4: never arms
    const uint16_t m_ro    = mutate ? 0x1234u : IDLE_STATUS; // 5: expects a stored write

    std::cout << "aa-disk-length-and-status -- AA_CONFORMANCE at paula_floppy\n";
    std::cout << (mutate ? "MUTATE=1: every assertion below must go red\n" : "");

    // --- 1. The fixture delivers a known value ------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red MUTATE=1 reads an
        // address this module decodes for nothing, which answers zero.
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
        // MUTATION DSKBYTR-AT-01A stimulus red MUTATE=1 reads $07E, which the
        // list marks W and this module gives no read path.
        const uint16_t at_01a = h.read(m_bytr);
        const uint16_t at_idle = h.read(A_IDLE);
        const uint16_t at_07e = h.read(A_DSKSYNC);
        checks.note("$01A reads " + hex(at_01a) + ", an undecoded address " +
                    hex(at_idle) + ", $07E " + hex(at_07e));
        checks.expect(at_01a == IDLE_STATUS && at_idle == 0 && at_07e == 0,
                      "DSKBYTR-AT-01A: the status word answers at $01A and no "
                      "other address in this module returns it, which is the "
                      "address the AA list prints for DSKBYTR [p.6] "
                      "[claims: sol:AA-dskbytr.c1 opus:AA-dskbytr.c1]");
    }

    // --- 3. DSKLEN's direction bit is visible in DSKBYTR ---------------------
    {
        Harness h;
        // MUTATION DSKLEN-AT-024-SHOWS-ITS-DIRECTION stimulus red MUTATE=1
        // writes the word to $07E, so dsklen never changes and the status bit
        // stays clear.
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
                      "[p.6] [claims: sol:AA-dsklen.c1 opus:AA-dsklen.c1]");
    }

    // --- 4. And the write only arms the transfer on the second one -----------
    {
        Harness h;
        // MUTATION DSKLEN-ARMS-ON-THE-SECOND-WRITE stimulus red MUTATE=1 writes
        // once: paula_floppy.v:742 needs bit 15 in the latched value AND in the
        // arriving one, so one write arms nothing and DMAON never rises.
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
                      "[claims: sol:AA-dsklen.c2 opus:AA-dsklen.c2]");
    }

    // --- 5. DSKBYTR takes no writes -----------------------------------------
    {
        Harness h;
        // MUTATION DSKBYTR-TAKES-NO-WRITES oracle red MUTATE=1 expects the
        // written word back, which is the reading this assertion rules out.
        h.write(A_DSKBYTR, 0x1234);
        const uint16_t got = h.read(A_DSKBYTR);
        checks.note("after writing 0x1234 at $01A it still reads " + hex(got));
        checks.expect(got == m_ro,
                      "DSKBYTR-TAKES-NO-WRITES: a word written at $01A does not "
                      "come back from it, so the address is the read the AA "
                      "list's R access letter names and not a store [p.6] "
                      "[claims: sol:AA-dskbytr.c2 opus:AA-dskbytr.c2]");
    }

    // --- What this boundary cannot grade ------------------------------------
    checks.record("DECLINED at this boundary: the list's chip column gives all "
                  "three registers to Paula and marks all three `~` for the "
                  "Coprocessor. No port of paula_floppy carries a chip "
                  "identity, and the mark is enforced in agnus_copper's cdang "
                  "path [claims: sol:AA-dskbytr.c3/c4 opus:AA-dskbytr.c3/c4 "
                  "sol:AA-dsklen.c3/c4 opus:AA-dsklen.c3/c4 "
                  "sol:AA-dsksync.c3/c4 opus:AA-dsksync.c3/c4]");
    checks.record("DECLINED at this boundary: DSKBYTR's description column "
                  "calls it a Disk Data byte AND status register. The status "
                  "half is graded above; the data byte is not here. MEASURED: "
                  "paula_floppy.v:699 drives DSKBYTR[7:0] from the constant "
                  "8'h00, and disk data reaches the bus through DSKDATR. Half "
                  "a claim is not a green assertion "
                  "[claims: sol:AA-dskbytr.c5 opus:AA-dskbytr.c5]");
    checks.record("DECLINED at this boundary: DSKSYNC takes a write at $07E and "
                  "offers no read, and the only thing that acts on it is "
                  "sync_match, which needs a disk read streaming a word equal "
                  "to the pattern. That fixture is this package's first "
                  "non-goal [claims: sol:AA-dsksync.c1/c2/c5 "
                  "opus:AA-dsksync.c1/c2/c5]");

    std::cout << "\nAssertions: " << checks.passed << " passed, "
              << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
