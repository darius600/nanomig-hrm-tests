// aa-audio-disk-control-registers -- paula conformance test
//
// Test purpose: the AA specification's register list gives ADKCON, ADKCONR and
//   DMACON their addresses, their access letters and -- for ADKCON -- a bit
//   table, and this is the module that decodes all three. It grades where the
//   registers answer, the SET/CLEAR convention the list names in DMACON's own
//   description, and the one ADKCON bit whose named effect reaches a port.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 6, 7 and 10 -- the two
//   address-ordered rows, the alphabetical headers, and ADKCON's bit table.
//   The document prints no page numbers of its own; the corpus numbering is
//   the PDF's.
// Non-goals: this package does not test the audio channels, the UART's framing
//   or baud rate, the disk data path, sector decoding, or the sd-card back
//   end. It grades register decoding, one write convention and one bit.
//
// THE SET/CLEAR CONVENTION IS THE POINT OF THESE ROWS. DMACON's description
//   column reads `DMA control write(clear or set)` and ADKCON's bit table
//   gives bit 15 as `SET/CLR  Set/Clear control bit. Determines if bits
//   written with a 1 get set or cleared`. Both are implemented the same way
//   here -- paula.v:164-171 for DMACON and :187-193 for ADKCON:
//
//     if (data_in[15]) adkcon <= adkcon |  data_in[14:0];
//     else             adkcon <= adkcon & ~data_in[14:0];
//
//   so a word written with bit 15 low never sets anything, and a bit written
//   as zero is untouched either way. Assertions 3, 4 and 8 grade those three
//   sentences separately, because they are three separate claims in the
//   document and one stimulus can satisfy two of them by accident.
//
// ADKCONR IS THE ONLY WAY TO SEE ADKCON AT ALL. `adkconr = (address decodes)
//   ? {1'b0, adkcon[14:0]} : 16'h0000` (paula.v:198), and ADKCON itself is
//   write-only -- the list says `W` and the module agrees by having no read
//   path at $09E. So every ADKCON assertion here writes at $09E and reads at
//   $010, which is exactly the pair of rows the document prints.
//
//   And the leading `1'b0` is why assertion 5 exists: bit 15 is the SET/CLR
//   control, not data, so a program that writes $FFFF and reads $7FFF has
//   learned the convention from the silicon.
//
// ONLY TWO OF ADKCON'S BITS HAVE A CONSUMER IN THIS RTL. MEASURED by grep for
//   `adkcon` over every .v file under src/minimig-aga: the identifier appears
//   in paula.v and nowhere else, and the only two uses are
//
//     .uartbrk(adkcon[11])    (paula.v:211)
//     .wordsync(adkcon[10])   (paula.v:269)
//
//   Bits 14-12 (PRECOMP 1-0 and MFMPREC), bit 9 (MSBSYNC) and bits 8-0 are
//   stored and read back, and drive nothing. The lot records every claim about
//   what those bits DO as declined with `absent` and names the measurement;
//   the claims about what the bits are CALLED are declined as insufficient,
//   because a stored bit at a boundary carries no name. That is a statement
//   about this RTL, not a verdict on it -- write precompensation is an
//   analogue concern a modern floppy back end has no use for.
//
// UARTBRK IS THE ONE BIT THIS BOUNDARY CAN SHOW. The table says `11 UARTBRK
//   Forces a UART break (clears TXD) if true`, and paula_uart.v:136 is
//   `if (uartbrk) tx_txd <= 1'b0;` with `assign txd = tx_txd` at :255. So
//   assertion 6 writes one bit and reads one pin, and the document's sentence
//   is the whole oracle.
//
// WHY DMACON NEEDS THE FLOPPY TO SHOW ITSELF. DMACON is write-only at this
//   boundary too -- DMACONR lives in Agnus -- so its address can only be
//   graded through an effect. `dsken = dmacon[4] & dmaen` (paula.v:174) is
//   paula_floppy's `enable`, `dma_active = dmaen && !lenzero && enable`
//   (paula_floppy.v:248) is what moves the disk state machine out of idle, and
//   `fdd_led = (dskstate != DISKDMA_IDLE)` (:972) is a port. DSKLEN is used
//   here as stimulus only; its own rows are graded by the paula_floppy
//   package, and the lot cites nothing for it.

#include "Vpaula.h"
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
// site changes. A single on/off flag reddened all eight assertions at once,
// which cannot show that the mutation recorded for one assertion leaves the
// other seven green.
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
constexpr int A_ADKCONR = 0x010 >> 1;   // paula.v:127
constexpr int A_DMACON  = 0x096 >> 1;   // paula.v:125
constexpr int A_ADKCON  = 0x09E >> 1;   // paula.v:126
constexpr int A_DSKLEN  = 0x024 >> 1;   // paula_floppy.v:124, stimulus only
constexpr int A_IDLE    = 0x000;        // decodes to no register in this module

constexpr uint16_t SETCLR   = 1u << 15; // ADKCON/DMACON bit 15
constexpr uint16_t UARTBRK  = 1u << 11; // ADKCON bit 11
constexpr uint16_t WORDSYNC = 1u << 10; // ADKCON bit 10
constexpr uint16_t DMAEN    = 1u << 9;  // DMACON master enable
constexpr uint16_t DSKEN    = 1u << 4;  // DMACON disk enable

// One fresh Verilated model per trial. paula_floppy's dsktrack has no reset
// term, so state cannot be assumed cleared by a reset pulse alone; the rule is
// followed literally here even though this package never steps the heads.
class Harness {
public:
    Harness() : dut(new Vpaula) { reset(); }
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

    // Advances until n qualifying (clk7_en-active) rising edges have been
    // applied. clk7_en as read here gates the UPCOMING tick's edge.
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
        dut->cck = 0;
        dut->reset = 1;
        dut->reg_address_in = A_IDLE;
        dut->data_in = 0;
        dut->rxd = 1;
        dut->ntsc = 0;
        dut->sof = 0;
        dut->strhor = 0;
        dut->vblint = 0;
        dut->int2 = 0;
        dut->int3 = 0;
        dut->int6 = 0;
        dut->_step = 1;     // inactive (active low): no head stepping anywhere here
        dut->direc = 0;
        dut->_sel = 0xE;    // drive 0 selected; not consulted by dma_active
        dut->side = 0;
        dut->_motor = 1;    // inactive; not consulted by dma_active
        dut->sdc_image_mounted = 0;
        dut->sdc_image_size = 0;
        dut->sdc_busy = 0;
        dut->sdc_done = 0;
        dut->sdc_byte_in_strobe = 0;
        dut->sdc_byte_addr = 0;
        dut->sdc_byte_in_data = 0;
        dut->floppy_drives = 0;
        dut->floppy_wrprot = 0;
        settle(8);
        dut->reset = 0;
        settle(8);
    }

    // Exactly one qualifying-edge write, then the address is withdrawn so no
    // further qualifying edge repeats it.
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

    // paula_floppy.v:735-744: the SECOND write's data_in[15] and the dsklen[15]
    // latched by the FIRST must both be 1 before the floppy's own dmaen rises.
    void arm_dsklen(uint16_t value) {
        write(A_DSKLEN, value);
        write(A_DSKLEN, value);
    }

    bool txd()     { dut->eval(); return dut->txd != 0; }
    bool fdd_led() { dut->eval(); return dut->fdd_led != 0; }

private:
    Vpaula* dut;
    int clk7_phase = 0;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // Each guard below is keyed to its OWN assertion's id: MUTATE=<id> changes
    // ONE thing in that assertion's stimulus, or an expected word the document
    // does not print, and every other assertion runs its real stimulus.
    const int      m_fix     = mutating("FIXTURE-DELIVERS-KNOWN-VALUE") ? A_ADKCON : A_ADKCONR;
    const int      m_adk_w   = mutating("ADKCON-AT-09E-ADKCONR-AT-010") ? A_DMACON : A_ADKCON;
    const uint16_t m_clr     = mutating("ADKCON-BIT-15-SETS-OR-CLEARS") ? SETCLR : 0u;
    const uint16_t m_keep    = mutating("ADKCON-ZERO-BITS-UNCHANGED") ? 0x0F0Fu : 0x00F0u;
    const uint16_t m_top     = mutating("ADKCONR-BIT-15-IS-NOT-STORED") ? 0xFFFFu : 0x7FFFu;
    const uint16_t m_brk     = mutating("ADKCON-BIT-11-CLEARS-TXD") ? WORDSYNC : UARTBRK;
    const int      m_dma_w   = mutating("DMACON-AT-096-GATES-THE-DISK") ? A_ADKCON : A_DMACON;
    const uint16_t m_dma_clr = mutating("DMACON-CLEARS-WHAT-IT-SET") ? SETCLR : 0u;

    std::cout << "aa-audio-disk-control-registers -- AA_CONFORMANCE at paula\n";
    if (g_mutate)
        std::cout << "MUTATE=" << g_mutate
                   << ": exactly the named assertion below must go red, the "
                      "others must keep their baseline verdict\n";

    // --- 1. The fixture delivers a known value ------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red selecting this assertion's own id reads
        // $09E, which the list marks W and the module gives no read path, so
        // the payload cannot come back.
        const uint16_t empty = h.read(A_ADKCONR);
        h.write(A_ADKCON, SETCLR | 0x0155);
        const uint16_t got = h.read(m_fix);
        checks.note("ADKCONR after reset " + hex(empty) +
                    ", after a SET write of 0x0155 " + hex(got));
        checks.expect(empty == 0x0000 && got == 0x0155,
                      "FIXTURE-DELIVERS-KNOWN-VALUE: ADKCONR reads zero out of "
                      "reset and reads back a word written at ADKCON, so every "
                      "assertion below starts from a value this fixture put there");
    }

    // --- 2. ADKCON is at $09E, ADKCONR at $010, and only one of them reads ---
    {
        Harness h;
        // MUTATION ADKCON-AT-09E-ADKCONR-AT-010 stimulus red selecting this assertion's own id writes
        // the payload to $096, which is DMACON and holds five bits, so nothing
        // reaches adkcon and ADKCONR answers zero.
        h.write(m_adk_w, SETCLR | 0x2A41);
        const uint16_t at_010 = h.read(A_ADKCONR);
        const uint16_t at_09e = h.read(A_ADKCON);
        const uint16_t at_idle = h.read(A_IDLE);
        checks.note("$010 reads " + hex(at_010) + ", $09E reads " + hex(at_09e) +
                    ", an undecoded address reads " + hex(at_idle));
        checks.expect(at_010 == 0x2A41 && at_09e == 0x0000 && at_idle == 0x0000,
                      "ADKCON-AT-09E-ADKCONR-AT-010: the word written at $09E "
                      "comes back at $010 and nowhere else, which is the pair "
                      "of addresses and the W/R access letters the AA list "
                      "prints for the two halves of this register [p.6,7,10] "
                      "[claims: opus:AA-adkcon.c1/c2/c4 opus:AA-adkconr.c1/c2 sol:AA-adkcon.c1/c2/c4/c5 sol:AA-adkconr.c1/c2/c6/c7]");
    }

    // --- 3. Bit 15 decides whether a written 1 sets or clears ---------------
    {
        Harness h;
        // MUTATION ADKCON-BIT-15-SETS-OR-CLEARS stimulus red selecting this assertion's own id performs
        // the second write with bit 15 set, so it ORs the same bits in again
        // and nothing is ever cleared.
        h.write(A_ADKCON, SETCLR | 0x7FFF);
        const uint16_t after_set = h.read(A_ADKCONR);
        h.write(A_ADKCON, m_clr | 0x0FF0);
        const uint16_t after_clr = h.read(A_ADKCONR);
        checks.note("after SET 0x7FFF: " + hex(after_set) +
                    ", after CLR 0x0FF0: " + hex(after_clr));
        checks.expect(after_set == 0x7FFF && after_clr == 0x700F,
                      "ADKCON-BIT-15-SETS-OR-CLEARS: the same word written "
                      "twice changes the register in opposite directions "
                      "depending on bit 15 alone, which is the Set/Clear "
                      "control bit the AA bit table describes [p.10] "
                      "[claims: sol:AA-adkcon.c8]");
    }

    // --- 4. A bit written as zero is left alone -----------------------------
    {
        Harness h;
        // MUTATION ADKCON-ZERO-BITS-UNCHANGED stimulus red selecting this assertion's own id clears
        // exactly the bits the first write set, so the register ends empty and
        // "unchanged" cannot be told from "cleared".
        h.write(A_ADKCON, SETCLR | 0x0F0F);
        h.write(A_ADKCON, m_keep);                 // a CLEAR that names other bits
        const uint16_t kept = h.read(A_ADKCONR);
        h.write(A_ADKCON, SETCLR | 0x0000);        // a SET that names no bits
        const uint16_t still = h.read(A_ADKCONR);
        checks.note("after clearing 0x00F0: " + hex(kept) +
                    ", after setting nothing: " + hex(still));
        checks.expect(kept == 0x0F0F && still == 0x0F0F,
                      "ADKCON-ZERO-BITS-UNCHANGED: bits written as zero survive "
                      "both a clear and a set, which is what the AA bit table "
                      "means by bits written with a 1 [p.10] "
                      "[claims: sol:AA-adkcon.c9]");
    }

    // --- 5. Bit 15 is the control, not data ---------------------------------
    {
        Harness h;
        // MUTATION ADKCONR-BIT-15-IS-NOT-STORED oracle red selecting this assertion's own id expects
        // $FFFF, i.e. a bit 15 that the register stored -- which is the reading
        // this assertion exists to rule out.
        h.write(A_ADKCON, 0xFFFF);
        const uint16_t got = h.read(A_ADKCONR);
        checks.note("after writing 0xFFFF at ADKCON, ADKCONR reads " + hex(got));
        checks.expect(got == m_top,
                      "ADKCONR-BIT-15-IS-NOT-STORED: writing every bit leaves "
                      "the top one clear on read back, so bit 15 is the "
                      "Set/Clear control the AA bit table names and not part of "
                      "the register [p.10] [claims: sol:AA-adkcon.c7]");
    }

    // --- 6. UARTBRK clears TXD ----------------------------------------------
    {
        Harness h;
        // MUTATION ADKCON-BIT-11-CLEARS-TXD stimulus red selecting this assertion's own id sets bit 10
        // instead, which is WORDSYNC and reaches the floppy rather than the
        // UART, so the transmit pin never moves.
        const bool idle = h.txd();
        h.write(A_ADKCON, SETCLR | m_brk);
        h.settle(4);
        const bool broken = h.txd();
        h.write(A_ADKCON, m_brk);                  // clear the same bit again
        h.settle(4);
        const bool released = h.txd();
        checks.note(std::string("txd idle ") + (idle ? "1" : "0") +
                    ", with the bit set " + (broken ? "1" : "0") +
                    ", after clearing it " + (released ? "1" : "0"));
        checks.expect(idle && !broken && released,
                      "ADKCON-BIT-11-CLEARS-TXD: setting bit 11 pulls the "
                      "transmit pin low and clearing it lets the pin return, "
                      "which is the UART break the AA bit table says the bit "
                      "forces [p.10] [claims: sol:AA-adkcon.c18/c19]");
    }

    // --- 7. DMACON at $096 gates the disk DMA -------------------------------
    {
        Harness h;
        // MUTATION DMACON-AT-096-GATES-THE-DISK stimulus red selecting this assertion's own id sends the
        // enable word to $09E, which is ADKCON: dmacon[4] stays low, dsken with
        // it, and the disk state machine never leaves idle.
        h.arm_dsklen(0x8001);                      // armed, length 1, read
        const bool before = h.fdd_led();
        h.write(m_dma_w, SETCLR | DMAEN | DSKEN);
        h.settle(4);
        const bool after = h.fdd_led();
        checks.note(std::string("with the DMACON enables clear the disk state "
                                "machine is ") + (before ? "active" : "idle") +
                    ", after setting DMAEN and DSKEN it is " +
                    (after ? "active" : "idle"));
        checks.expect(!before && after,
                      "DMACON-AT-096-GATES-THE-DISK: an armed disk transfer "
                      "stays idle until a word written at $096 enables it, "
                      "which is the address and the W access letter the AA list "
                      "prints for DMACON [p.7] "
                      "[claims: opus:AA-dmacon.c1/c2 sol:AA-dmacon.c1/c2]");
    }

    // --- 8. And the same address clears what it set -------------------------
    {
        Harness h;
        // MUTATION DMACON-CLEARS-WHAT-IT-SET stimulus red selecting this assertion's own id performs the
        // second write as a set, so DSKEN is ORed in again and the transfer
        // stays enabled.
        h.arm_dsklen(0x8001);
        h.write(A_DMACON, SETCLR | DMAEN | DSKEN);
        h.settle(4);
        const bool enabled = h.fdd_led();
        h.write(A_DMACON, m_dma_clr | DSKEN);
        h.settle(4);
        const bool disabled = h.fdd_led();
        checks.note(std::string("after the set the machine is ") +
                    (enabled ? "active" : "idle") + ", after the clear it is " +
                    (disabled ? "active" : "idle"));
        checks.expect(enabled && !disabled,
                      "DMACON-CLEARS-WHAT-IT-SET: the same address turns the "
                      "transfer off again when bit 15 is low, which is what the "
                      "AA list's description column means by write(clear or "
                      "set) [p.7] [claims: opus:AA-dmacon.c4 sol:AA-dmacon.c4]");
    }

    // --- What this boundary cannot grade ------------------------------------
    checks.record(
             "DECLINED at this boundary (absent): MEASURED by grep over every .v "
             "file under src/minimig-aga: `adkcon` appears in paula.v alone, and "
             "its only two consumers are `.uartbrk(adkcon[11])` at paula.v:211 "
             "and `.wordsync(adkcon[10])` at paula.v:269. The bit this claim "
             "describes is stored and read back, and nothing acts on it. "
             "[claims: sol:AA-adkcon.c11/c12/c13/c14/c16/c17/c23]");
    checks.record(
             "DECLINED at this boundary (insufficient): ADKCONR hands back the "
             "stored bit, and a stored bit carries no name. Nothing at this "
             "boundary distinguishes bit 12 called MFMPREC from bit 12 called "
             "anything else. [claims: sol:AA-adkcon.c10/c15/c22]");
    checks.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "access column -- W, R or S. A module answers reads at the "
             "addresses it decodes and zero everywhere else, so a write-only row "
             "reads as the same silence an undecoded address reads as, and a "
             "strobe has no read at all. [claims: opus:AA-aud0len.c2 "
             "opus:AA-aud0per.c2 opus:AA-aud0vol.c2 opus:AA-aud1len.c2 "
             "opus:AA-aud1per.c2 opus:AA-aud1vol.c2 opus:AA-aud2len.c2 "
             "opus:AA-aud2per.c2 opus:AA-aud2vol.c2 opus:AA-aud3len.c2 "
             "opus:AA-aud3per.c2 opus:AA-aud3vol.c2 opus:AA-dmaconr.c2 "
             "sol:AA-aud0len.c2 sol:AA-aud0per.c2 sol:AA-aud0vol.c2 "
             "sol:AA-aud1len.c2 sol:AA-aud1per.c2 sol:AA-aud1vol.c2 "
             "sol:AA-aud2len.c2 sol:AA-aud2per.c2 sol:AA-aud2vol.c2 "
             "sol:AA-aud3len.c2 sol:AA-aud3per.c2 sol:AA-aud3vol.c2 "
             "sol:AA-dmaconr.c2]");
    checks.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "chip column, which says which 1985 silicon a register lives in. No "
             "port of any module in this design carries a chip identity. "
             "[claims: opus:AA-aud0len.c3 opus:AA-aud0per.c3 opus:AA-aud0vol.c3 "
             "opus:AA-aud1len.c3 opus:AA-aud1per.c3 opus:AA-aud1vol.c3 "
             "opus:AA-aud2len.c3 opus:AA-aud2per.c3 opus:AA-aud2vol.c3 "
             "opus:AA-aud3len.c3 opus:AA-aud3per.c3 opus:AA-aud3vol.c3 "
             "opus:AA-dmaconr.c3 sol:AA-aud0len.c3 sol:AA-aud0per.c3 "
             "sol:AA-aud0vol.c3 sol:AA-aud1len.c3 sol:AA-aud1per.c3 "
             "sol:AA-aud1vol.c3 sol:AA-aud2len.c3 sol:AA-aud2per.c3 "
             "sol:AA-aud2vol.c3 sol:AA-aud3len.c3 sol:AA-aud3per.c3 "
             "sol:AA-aud3vol.c3 sol:AA-dmaconr.c3]");
    checks.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. Which silicon a block is fabricated "
             "in is a fact about the source tree and no port of paula carries "
             "it. [claims: opus:AA-adkcon.c3 opus:AA-adkconr.c3 "
             "opus:AA-dmacon.c3 sol:AA-adkcon.c3/c6 sol:AA-adkconr.c3/c8 "
             "sol:AA-dmacon.c3]");
    checks.record(
             "DECLINED at this boundary (insufficient): The register is decoded "
             "at this boundary, but no assertion in this package writes or reads "
             "it. The package grades the rows its own subject names; this row is "
             "another register of the same module. [claims: opus:AA-aud0len.c1 "
             "opus:AA-aud0per.c1 opus:AA-aud0vol.c1 opus:AA-aud1len.c1 "
             "opus:AA-aud1per.c1 opus:AA-aud1vol.c1 opus:AA-aud2len.c1 "
             "opus:AA-aud2per.c1 opus:AA-aud2vol.c1 opus:AA-aud3len.c1 "
             "opus:AA-aud3per.c1 opus:AA-aud3vol.c1 opus:AA-dmaconr.c1/c5 "
             "sol:AA-aud0len.c1 sol:AA-aud0per.c1 sol:AA-aud0vol.c1 "
             "sol:AA-aud1len.c1 sol:AA-aud1per.c1 sol:AA-aud1vol.c1 "
             "sol:AA-aud2len.c1 sol:AA-aud2per.c1 sol:AA-aud2vol.c1 "
             "sol:AA-aud3len.c1 sol:AA-aud3per.c1 sol:AA-aud3vol.c1 "
             "sol:AA-dmaconr.c1/c5]");
    checks.record(
             "DECLINED at this boundary (insufficient): WORDSYNC reaches "
             "paula_floppy's `wordsync` input, so the signal exists. Seeing it "
             "act needs a disk read carrying a word equal to DSKSYNC, which is "
             "this package's first non-goal. [claims: sol:AA-adkcon.c20/c21]");
    checks.record(
             "DECLINED at this boundary (not-hardware): The claim is a legend "
             "statement, a gap between two printed rows, or a reserved line -- "
             "the document describing its own notation and layout. The one "
             "legend mark that IS a hardware restriction, the `~` Coprocessor "
             "limit, is graded by this package's own assertion. [claims: "
             "opus:AA-dmaconr.c4 sol:AA-dmaconr.c4]");
    checks.record(
             "DECLINED at this boundary (not-hardware): The two pages describe "
             "the same register in different words. Which description the "
             "document should print is a fact about the document, not about a "
             "signal. [claims: sol:AA-adkconr.c5]");
    checks.record(
             "DECLINED at this boundary (outside-uut): The `~` mark is a "
             "Coprocessor write restriction enforced in agnus_copper's cdang "
             "path and graded by aa-copper-register-map. [claims: "
             "opus:AA-adkconr.c4 sol:AA-adkconr.c4]");

    std::cout << "\nAssertions: " << checks.passed << " passed, "
              << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
