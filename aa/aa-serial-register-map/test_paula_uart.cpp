// aa-serial-register-map -- paula_uart conformance test
//
// Test purpose: the AA specification's address-ordered register list gives the
//   serial port three addresses -- SERDATR to read at $018, SERDAT to write at
//   $030 and SERPER at $032 -- and assigns all three to Paula. This grades
//   where each of them is decoded, at the module that holds them.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 7 and 8, the
//   address-ordered register list. The document prints no page numbers of its
//   own; the corpus numbering is the PDF's.
// Non-goals: the receiver, the interrupts, the break bit, and SERDATR's own bit
//   table, which is read in another slice and belongs to a package of its own.
//   This grades three addresses and what a write to two of them does.
//
// THE FIRST PACKAGE AT THIS MODULE. paula_uart is one of the modules
//   lot_candidates.py names for the AA register list and had no package until
//   now; the serial rows of the list were declined everywhere as "decoded in
//   paula_uart, a module of its own".
//
// HOW EACH ADDRESS IS MADE TO SHOW ITSELF. The read path answers one address
//   and nothing else --
//
//     assign data_o = (rga_i == REG_SERDATR[8:1])
//                   ? {serdatr, 1'b0, rx_data} : 16'h0000;            (:258)
//
//   so $018 is graded by reading it and reading its neighbours. The two write
//   addresses have no read path at all, and are graded by what they DO:
//   SERDAT starts a transmission, which clears TSRE in the word $018
//   answers and puts the word out on `txd`; SERPER sets the count the
//   transmitter reloads per bit, so the same byte takes longer to leave.

#include "Vpaula_uart.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

static std::string hex(uint32_t v) {
    std::ostringstream o;
    o << "0x" << std::hex << std::uppercase << v;
    return o.str();
}

static std::string dec(int v) {
    std::ostringstream o;
    o << v;
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

// Word addresses as the AA address-ordered list prints them.
constexpr uint16_t A_SERDATR = 0x018;
constexpr uint16_t A_SERDAT  = 0x030;
constexpr uint16_t A_SERPER  = 0x032;
constexpr uint16_t A_INTREQR = 0x01E;   // a neighbour this module must not answer

// SERDATR's status bits, from `{rx_ovrun, rx_rbf, tx_tbe, tx_tsre,
// rx_rxd}` at [15:11] (:248). TBE is NOT the bit that says a
// transmission is running: the buffer is handed to the shifter at once
// and comes back empty in the same cycle -- measured, 0x3800 idle and
// 0x2800 while transmitting. TSRE is the shifter's own bit and is the
// one that stays low until the word has left.
constexpr uint16_t TBE  = 1u << 13;     // transmit buffer empty
constexpr uint16_t TSRE = 1u << 12;     // transmit shift register empty

class Harness {
public:
    Harness() : dut(new Vpaula_uart) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->rga_i = 0; dut->data_i = 0;
        dut->uartbrk = 0; dut->rbfmirror = 0; dut->rxd = 1;
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 8; ++i) step();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void step() {
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
    }

    void write(uint16_t word_addr, uint16_t value) {
        dut->rga_i = (word_addr >> 1) & 0xFF;
        dut->data_i = value;
        step();
        dut->rga_i = 0;
        dut->data_i = 0;
        step();
    }

    uint16_t read(uint16_t word_addr) {
        dut->rga_i = (word_addr >> 1) & 0xFF;
        dut->eval();
        const uint16_t got = (uint16_t)dut->data_o;
        dut->rga_i = 0;
        dut->eval();
        return got;
    }

    // How many clocks the word takes to leave the shift register.
    int clocks_until_sent(int limit = 400000) {
        for (int i = 1; i <= limit; ++i) {
            step();
            if (read(A_SERDATR) & TSRE) return i;
        }
        return -1;
    }

    Vpaula_uart* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    const bool mutate = getenv("MUTATE") != nullptr;

    // MUTATION. Every expect() is guarded so MUTATE=1 forces it red, and each
    // guard moves ONE thing: the address a word is written to or read from.
    const uint16_t m_datr = mutate ? A_INTREQR : A_SERDATR;   // 2
    const uint16_t m_dat  = mutate ? A_SERPER  : A_SERDAT;    // 3
    // 4: the VALUE, not the address. Writing the period to SERDATR's
    // address instead left the second run with the reset period of zero,
    // which is FASTER than the slow one -- so the mutated run passed the
    // assertion more convincingly than the real one.
    const uint16_t m_per  = mutate ? (uint16_t)0x0060u        // 4
                                   : (uint16_t)0x0010u;
    const uint16_t m_live = mutate ? A_INTREQR : A_SERDAT;    // 1

    constexpr uint16_t WORD = 0x0155;      // a byte with a stop bit above it
    constexpr uint16_t SLOW = 0x0060;   // the shorter period is m_per,
                                        // which the mutation moves

    std::cout << "aa-serial-register-map -- AA_CONFORMANCE at paula_uart\n";
    std::cout << (mutate ? "MUTATE=1: every assertion below must go red\n" : "");

    // --- 1. The fixture starts a transmission -------------------------------
    {
        Harness h;
        const uint16_t idle = h.read(A_SERDATR);
        h.write(A_SERPER, SLOW);
        // MUTATION FIXTURE-STARTS-A-TRANSMISSION stimulus red MUTATE=1 writes
        // the word to a neighbouring address this module does not decode, so
        // the transmitter never starts and TSRE never falls.
        h.write(m_live, WORD);
        h.step();
        const uint16_t busy = h.read(A_SERDATR);
        c.note("SERDATR reads " + hex(idle) + " idle and " + hex(busy) +
               " just after a word is written to $030");
        c.expect((idle & TSRE) && !(busy & TSRE),
                 "FIXTURE-STARTS-A-TRANSMISSION: the shift register is empty "
                 "until a word is written and holding a word after, so every "
                 "assertion below is against a transmitter this fixture "
                 "started [no page -- infrastructure]");
    }

    // --- 2. SERDATR is the address that answers a read ----------------------
    {
        Harness h;
        // MUTATION SERDATR-IS-AT-018 stimulus red MUTATE=1 reads a
        // neighbouring address instead, which this module answers with zero
        // exactly as it answers every address that is not its own.
        const uint16_t got = h.read(m_datr);
        const uint16_t neighbour = h.read(A_INTREQR);
        c.note("the address the list gives SERDATR reads " + hex(got) +
               " and the neighbouring $01E reads " + hex(neighbour));
        c.expect(got != 0 && neighbour == 0,
                 "SERDATR-IS-AT-018: the one address this module answers a "
                 "read at is the one the AA list gives SERDATR, and the "
                 "addresses either side of it answer nothing [p.7] "
                 "[claims: opus:AA-serdatr.c1 sol:AA-serdatr.c1]");
    }

    // --- 3. SERDAT is the address a transmission starts from ----------------
    {
        Harness h;
        h.write(A_SERPER, SLOW);
        // MUTATION SERDAT-IS-AT-030 stimulus red MUTATE=1 writes the word
        // to SERPER's address instead, which sets a period and starts no
        // transmission, so the shift register stays empty.
        h.write(m_dat, WORD);
        h.step();
        const bool started = !(h.read(A_SERDATR) & TSRE);
        c.note(std::string("after writing the word at ") +
               hex(mutate ? A_SERPER : A_SERDAT) + ", the shift register is " +
               (started ? "holding a word" : "still empty"));
        c.expect(started,
                 "SERDAT-IS-AT-030: the address the AA list gives SERDAT is "
                 "the one whose write hands a word to the transmitter -- the "
                 "list's own W access column, measured rather than read "
                 "[p.7] [claims: opus:AA-serdat.c1 sol:AA-serdat.c1]");
    }

    // --- 4. SERPER is the address that sets how long a bit lasts ------------
    {
        Harness slow, fast;
        slow.write(A_SERPER, SLOW);
        slow.write(A_SERDAT, WORD);
        const int t_slow = slow.clocks_until_sent();

        // MUTATION SERPER-IS-AT-032 stimulus red MUTATE=1 writes the SAME
        // period into the second run, so the two transmissions take the
        // same time and the register is not shown to decide anything.
        fast.write(A_SERPER, m_per);
        fast.write(A_SERDAT, WORD);
        const int t_fast = fast.clocks_until_sent();

        c.note("the same word takes " + dec(t_slow) + " clocks to leave with "
               "SERPER=" + hex(SLOW) + " and " + dec(t_fast) + " with SERPER=" +
               hex(m_per));
        c.expect(t_slow > 0 && t_fast > 0 && t_fast < t_slow,
                 "SERPER-IS-AT-032: the address the AA list gives SERPER is "
                 "the one whose word decides how long the transmitter holds "
                 "each bit, so the same byte leaves sooner when it is smaller "
                 "[p.7] [claims: opus:AA-serper.c1 sol:AA-serper.c1]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "access column -- W, R or S. A module answers reads at the "
             "addresses it decodes and zero everywhere else, so a write-only row "
             "reads as the same silence an undecoded address reads as, and a "
             "strobe has no read at all. [claims: opus:AA-serdat.c2 "
             "opus:AA-serdatr.c2 opus:AA-serper.c2 sol:AA-serdat.c2 "
             "sol:AA-serdatr.c2 sol:AA-serper.c2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "chip column, which says which 1985 silicon a register lives in. No "
             "port of any module in this design carries a chip identity. "
             "[claims: opus:AA-serdat.c3 opus:AA-serdatr.c3 opus:AA-serper.c3 "
             "sol:AA-serdat.c3 sol:AA-serdatr.c3 sol:AA-serper.c3]");
    c.record(
             "DECLINED at this boundary (insufficient): This package writes and "
             "reads the register -- that is how its address row is graded here "
             "-- but no assertion distinguishes what this row says about it: the "
             "function column's width or role, the read-and-clear, the restart. "
             "A stimulus that passes whether or not the row is true does not "
             "grade it. [claims: opus:AA-serdat.c5 opus:AA-serdatr.c5 "
             "opus:AA-serper.c5 sol:AA-serdat.c5 sol:AA-serdatr.c5]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is a legend "
             "statement, a gap between two printed rows, or a reserved line -- "
             "the document describing its own notation and layout. The one "
             "legend mark that IS a hardware restriction, the `~` Coprocessor "
             "limit, is graded by this package's own assertion. [claims: "
             "opus:AA-serdat.c4 opus:AA-serdatr.c4 opus:AA-serper.c4 "
             "sol:AA-serdat.c4 sol:AA-serdatr.c4 sol:AA-serper.c4]");

    std::cout << "\nAssertions: " << c.passed << " passed, " << c.failed
              << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
