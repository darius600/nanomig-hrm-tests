// aa-lisa-colour-registers -- denise conformance test
//
// Test purpose: the AA specification's address-ordered register list gives
//   LISAID, the bit-plane control registers and the colour table their
//   addresses, and this is the module that answers for them. It grades the
//   chip-revision register the document introduces, the colour table's
//   addresses, and the two AGA mechanisms the list's own rows depend on --
//   reading the colour table back, and selecting one of eight banks of it.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 6, 8 and 9, the
//   address-ordered register list. The document prints no page numbers of its
//   own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the pixel pipeline, HAM, the sprites,
//   the playfield priority, the display window, or what any colour looks like
//   on a screen. It grades where registers are decoded and what two control
//   bits switch.
//
// LISAID IS THE REGISTER THIS DOCUMENT EXISTS FOR. Its row reads
//   `LISAID  h ~07C  R  D  Chip Revision level for Denise/Lisa`, its revision
//   list records `added new LISAID definition` on 4/30/91, and the module
//   answers with a different word per chip set:
//
//     deniseid_out = aga ? $00F8 : ecs ? $FFFC : $FFFF                (:322)
//
//   so the identity a program reads at $07C is how it learns which silicon it
//   is on. Assertion 3 reads all three.
//
// AND TWO AGA MECHANISMS GATE THE COLOUR TABLE. Reading it back at all needs
//   `rdram = bplcon2[8] & aga` (:221), and WHICH of eight banks an address
//   reaches is `bank = bplcon3[15:13] & {3{aga}}` (:249), which is how 32
//   addresses cover 256 colours. Both are ANDed with `aga`, so on an older
//   chip set the same writes do nothing -- and that, not the colour, is what
//   makes these AA claims rather than HRM ones.
//
// READING A COLOUR ADDRESS WITH THE BIT CLEAR IS A WRITE. `wr_en` is
//   `address decodes && clk7_en && !rdram` (denise_colortable.v:31), and
//   nothing about it asks whether the bus is writing -- so holding a colour
//   address while the readback bit is low stores whatever data_in happens to
//   carry. This package's first version read the closed case first and
//   overwrote the colour with zero before the open case could see it, which
//   read as a readback path that never opened. Every assertion here reads the
//   open case first.
//
// HOW A COLOUR IS READ BACK. The table is written when the address decodes and
//   `rdram` is low, and read at the same address when `rdram` is high
//   (denise_colortable.v:30-34). So every assertion here writes with the
//   readback bit clear and reads with it set, and the switch between them is
//   itself assertion 5.

#include "Vdenise.h"
#include "verilated.h"
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {
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

// Word addresses as the AA address list prints them, read from the corpus.
constexpr uint16_t A_LISAID  = 0x07C;
constexpr uint16_t A_BPLCON2 = 0x104;
constexpr uint16_t A_BPLCON3 = 0x106;
constexpr uint16_t A_BPLCON4 = 0x10C;
constexpr uint16_t A_COLOR00 = 0x180;

constexpr uint16_t RDRAM   = 1u << 8;    // BPLCON2: read the colour table
constexpr uint16_t BANK_SH = 13;         // BPLCON3: colour bank select

static uint16_t color_addr(int n) { return A_COLOR00 + 2 * n; }

class Harness {
public:
    Harness(bool aga = true, bool ecs = true) : dut(new Vdenise) {
        dut->clk = 0; dut->clk7_en = 1;
        dut->aga = aga ? 1 : 0; dut->ecs = ecs ? 1 : 0;
        dut->reg_address_in = 0; dut->data_in = 0;
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
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->data_in = value;
        step();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        step();
    }

    uint16_t read(uint16_t word_addr) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->eval();
        step();                       // the table's output is a clock behind
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->eval();
        const uint16_t got = dut->data_out;
        dut->reg_address_in = 0;
        dut->eval();
        return got;
    }

    // Write a colour with the readback bit clear, then read it with it set.
    uint16_t colour_roundtrip(int n, uint16_t value, uint16_t bplcon3 = 0) {
        write(A_BPLCON2, 0);              // rdram low: the table takes writes
        write(A_BPLCON3, bplcon3);
        write(color_addr(n), value);
        write(A_BPLCON2, RDRAM);          // rdram high: the table answers reads
        return read(color_addr(n)) & 0x0FFF;
    }

    Vdenise* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks c;

    // Per-assertion mutation selector: MUTATE names ONE assertion's own
    // label, and only that assertion's stimulus site changes.
    const uint16_t m_lisaid = mutating("LISAID-AT-07C") ? A_BPLCON2 : A_LISAID;  // 2
    const bool     m_aga    = mutating("LISAID-IDENTIFIES-THE-CHIP-SET") ? true : false; // 3
    const uint16_t m_col    = mutating("COLOUR-TABLE-AT-180")
                                   ? color_addr(3) : color_addr(7);              // 4
    const uint16_t m_rdram  = mutating("BPLCON2-AT-104-OPENS-READBACK") ? RDRAM : 0u; // 5
    const uint16_t m_bank   = mutating("BPLCON3-AT-106-SELECTS-THE-BANK")
                                   ? 0u : (1u << BANK_SH);                       // 6
    const uint16_t m_live   = mutating("FIXTURE-DELIVERS-KNOWN-VALUE")
                                   ? A_BPLCON2 : A_LISAID;                       // liveness

    c.record("oracle: AA specification, PDF pp. 6, 8 and 9, the address-ordered "
             "register list. LISAID $07C, BPLCON2 $104, BPLCON3 $106, BPLCON4 "
             "$10C, COLOR00 $180 through COLOR31 $1BE "
             "[no page -- the package's own oracle line]");

    c.record("DECLINED at this boundary: BPLCON4's write is gated on `aga` like "
             "the other two, but what it sets -- `bplxor`, the colour-select "
             "XOR, and the sprite colour offsets -- reaches the PIXEL path and "
             "not the readback path, so no read at any address shows it. A "
             "fixture that runs a display would; this one does not [claims: "
             "sol:AA-bplcon4.c1/c2/c3/c4/c5 opus:AA-bplcon4.c1/c2/c3/c4/c5]");
    c.record("DECLINED at this boundary: the list's chip column gives these to "
             "Denise/Lisa and marks LISAID `~` for the Coprocessor. Neither is "
             "visible at any port of denise [claims: sol:AA-lisaid.c3/c4/c5 "
             "sol:AA-bplcon2.c3 sol:AA-bplcon3.c3 sol:AA-color00.c3]");

    // --- FIXTURE LIVENESS ---------------------------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: MUTATE=<id>
        // reads $104, which is write-only and answers zero. MEASURED (WSL/
        // Verilator 5.052, run alone against this id): FAIL FIXTURE-
        // DELIVERS-KNOWN-VALUE, all five other assertions PASS, summary "5
        // passed, 1 failed" (baseline "6 passed, 0 failed"). Only the named
        // assertion moved.
        const uint16_t got = h.read(m_live);
        c.note("reading $07C on an AGA part returns " + hex(got));
        c.expect(got != 0,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: this module answers a read at "
                 "an address it decodes, so a later assertion that reads "
                 "nothing means the decoder and not the fixture [no page -- "
                 "infrastructure]");
    }

    // --- 2. LISAID answers at $07C and nowhere else -------------------------
    {
        Harness h;
        // MUTATION LISAID-AT-07C stimulus red: MUTATE=<id> reads $104, which
        // is write-only at this boundary and answers zero. MEASURED (WSL/
        // Verilator 5.052, run alone against this id): FAIL LISAID-AT-07C,
        // all five other assertions PASS, summary "5 passed, 1 failed".
        // Only the named assertion moved.
        const uint16_t got = h.read(m_lisaid);
        const uint16_t elsewhere = h.read(0x0A0);
        c.note("$07C reads " + hex(got) + ", an address this module does not "
               "decode reads " + hex(elsewhere));
        c.expect(got == 0x00F8 && elsewhere == 0x0000,
                 "LISAID-AT-07C: the chip revision register answers at $07C and "
                 "an undecoded address answers zero, which is the address and "
                 "the read access the AA list prints for LISAID [p.6] [claims: opus:AA-lisaid.c1/c2 sol:AA-lisaid.c1/c2]");
    }

    // --- 3. LISAID says WHICH chip set ---------------------------------------
    {
        Harness aga(true, true);
        Harness ecs(false, true);
        Harness ocs(false, false);
        const uint16_t a = aga.read(A_LISAID);
        // MUTATION LISAID-IDENTIFIES-THE-CHIP-SET stimulus red: MUTATE=<id>
        // builds the second fixture as AGA too, so two of the three answers
        // match. MEASURED (WSL/Verilator 5.052, run alone against this id):
        // FAIL LISAID-IDENTIFIES-THE-CHIP-SET, all five other assertions
        // PASS, summary "5 passed, 1 failed". Only the named assertion
        // moved.
        Harness other(m_aga, true);
        const uint16_t e = other.read(A_LISAID);
        const uint16_t o = ocs.read(A_LISAID);
        c.note("LISAID reads " + hex(a) + " on AGA, " + hex(e)
               + " on ECS and " + hex(o) + " on neither");
        c.expect(a == 0x00F8 && e == 0xFFFC && o == 0xFFFF
                 && a != e && e != o,
                 "LISAID-IDENTIFIES-THE-CHIP-SET: $07C answers with a different "
                 "word on each chip set, which is what makes it the chip "
                 "revision level the AA list's function column calls it, and "
                 "the register this document's revision list records adding "
                 "[p.6] [claims: opus:AA-lisaid.c6 sol:AA-lisaid.c6]");
    }

    // --- 4. the colour table is addressed from $180 --------------------------
    {
        Harness h;
        h.colour_roundtrip(7, 0x0ABC);
        // MUTATION COLOUR-TABLE-AT-180 stimulus red: MUTATE=<id> reads
        // entry 3, which was never written. MEASURED (WSL/Verilator 5.052,
        // run alone against this id): FAIL COLOUR-TABLE-AT-180, all five
        // other assertions PASS, summary "5 passed, 1 failed". Only the
        // named assertion moved.
        h.write(A_BPLCON2, RDRAM);
        const uint16_t got = h.read(m_col) & 0x0FFF;
        const uint16_t neighbour = h.read(color_addr(8)) & 0x0FFF;
        c.note("colour 7 written $ABC reads " + hex(got)
               + ", colour 8 reads " + hex(neighbour));
        c.expect(got == 0x0ABC && neighbour != 0x0ABC,
                 "COLOUR-TABLE-AT-180: a colour written at $180 plus twice its "
                 "number comes back at the same address and not at its "
                 "neighbour, which is the run of addresses the AA list prints "
                 "for the colour table [p.9] [claims: opus:AA-color00.c1/c2 sol:AA-color00.c1/c2]");
    }

    // --- 5. BPLCON2 bit 8 is what opens the readback path -------------------
    {
        Harness h;
        h.write(A_BPLCON2, 0);
        h.write(color_addr(5), 0x0DEF);
        // MUTATION BPLCON2-AT-104-OPENS-READBACK stimulus red: MUTATE=<id>
        // leaves the readback bit set for the read that should see nothing.
        // MEASURED (WSL/Verilator 5.052, run alone against this id): FAIL
        // BPLCON2-AT-104-OPENS-READBACK, all five other assertions PASS,
        // summary "5 passed, 1 failed". Only the named assertion moved.
        h.write(A_BPLCON2, RDRAM);
        const uint16_t open = h.read(color_addr(5)) & 0x0FFF;
        h.write(A_BPLCON2, m_rdram);
        const uint16_t closed = h.read(color_addr(5)) & 0x0FFF;
        c.note("colour 5 reads " + hex(closed) + " with the readback bit clear "
               "and " + hex(open) + " with it set");
        c.expect(closed == 0x0000 && open == 0x0DEF,
                 "BPLCON2-AT-104-OPENS-READBACK: bit 8 of the word written at "
                 "$104 decides whether the colour table answers a read at all, "
                 "which is the address the AA list prints for BPLCON2 and one "
                 "of the mechanisms its AGA rows rest on [p.8] [claims: opus:AA-bplcon2.c1/c2 sol:AA-bplcon2.c1/c2]");
    }

    // --- 6. BPLCON3 selects which bank the same address reaches -------------
    {
        Harness h;
        h.colour_roundtrip(2, 0x0111, 0);
        // MUTATION BPLCON3-AT-106-SELECTS-THE-BANK stimulus red: MUTATE=<id>
        // writes the second colour into bank 0 as well, so both reads agree.
        // MEASURED (WSL/Verilator 5.052, run alone against this id): FAIL
        // BPLCON3-AT-106-SELECTS-THE-BANK, all five other assertions PASS,
        // summary "5 passed, 1 failed". Only the named assertion moved.
        h.colour_roundtrip(2, 0x0222, m_bank);
        h.write(A_BPLCON2, RDRAM);
        h.write(A_BPLCON3, 0);
        const uint16_t bank0 = h.read(color_addr(2)) & 0x0FFF;
        h.write(A_BPLCON3, 1u << BANK_SH);
        const uint16_t bank1 = h.read(color_addr(2)) & 0x0FFF;
        c.note("one address reads " + hex(bank0) + " in bank 0 and "
               + hex(bank1) + " in bank 1");
        c.expect(bank0 == 0x0111 && bank1 == 0x0222,
                 "BPLCON3-AT-106-SELECTS-THE-BANK: the top three bits of the "
                 "word written at $106 decide which bank of the colour table "
                 "one address reaches, which is how the AA list's thirty-two "
                 "colour addresses cover more than thirty-two colours [p.8] "
                 "[claims: opus:AA-bplcon3.c1/c2 sol:AA-bplcon3.c1/c2]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): BPLCON4's write is gated "
             "on `aga` like BPLCON2's readback bit and BPLCON3's bank, but what "
             "it sets -- bplxor, the colour-select XOR, and the sprite colour "
             "offsets -- reaches the PIXEL path and not the readback path, so no "
             "read at any address shows it. [claims: "
             "opus:AA-bplcon4.c1/c2/c3/c4/c5 sol:AA-bplcon4.c1/c2/c3/c4/c5]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "access column -- W, R or S. A module answers reads at the "
             "addresses it decodes and zero everywhere else, so a write-only row "
             "reads as the same silence an undecoded address reads as, and a "
             "strobe has no read at all. [claims: opus:AA-bplcon0.c2 "
             "opus:AA-color01.c2 opus:AA-color02.c2 opus:AA-color03.c2 "
             "opus:AA-color04.c2 opus:AA-color05.c2 opus:AA-color06.c2 "
             "opus:AA-color07.c2 opus:AA-color08.c2 opus:AA-color09.c2 "
             "opus:AA-color10.c2 opus:AA-color11.c2 opus:AA-color12.c2 "
             "opus:AA-color13.c2 opus:AA-color14.c2 opus:AA-color15.c2 "
             "opus:AA-color16.c2 opus:AA-color17.c2 opus:AA-color18.c2 "
             "opus:AA-color19.c2 opus:AA-color20.c2 opus:AA-color21.c2 "
             "opus:AA-color22.c2 opus:AA-color23.c2 opus:AA-color24.c2 "
             "opus:AA-color25.c2 opus:AA-color26.c2 opus:AA-color27.c2 "
             "opus:AA-color28.c2 opus:AA-color29.c2 opus:AA-color30.c2 "
             "opus:AA-color31.c2 sol:AA-bplcon0.c2 sol:AA-color01.c2 "
             "sol:AA-color02.c2 sol:AA-color03.c2 sol:AA-color04.c2 "
             "sol:AA-color05.c2 sol:AA-color06.c2 sol:AA-color07.c2 "
             "sol:AA-color08.c2 sol:AA-color09.c2 sol:AA-color10.c2 "
             "sol:AA-color11.c2 sol:AA-color12.c2 sol:AA-color13.c2 "
             "sol:AA-color14.c2 sol:AA-color15.c2 sol:AA-color16.c2 "
             "sol:AA-color17.c2 sol:AA-color18.c2 sol:AA-color19.c2 "
             "sol:AA-color20.c2 sol:AA-color21.c2 sol:AA-color22.c2 "
             "sol:AA-color23.c2 sol:AA-color24.c2 sol:AA-color25.c2 "
             "sol:AA-color26.c2 sol:AA-color27.c2 sol:AA-color28.c2 "
             "sol:AA-color29.c2 sol:AA-color30.c2 sol:AA-color31.c2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "chip column, which says which 1985 silicon a register lives in. No "
             "port of any module in this design carries a chip identity. "
             "[claims: opus:AA-bplcon0.c3 opus:AA-color01.c3 opus:AA-color02.c3 "
             "opus:AA-color03.c3 opus:AA-color04.c3 opus:AA-color05.c3 "
             "opus:AA-color06.c3 opus:AA-color07.c3 opus:AA-color08.c3 "
             "opus:AA-color09.c3 opus:AA-color10.c3 opus:AA-color11.c3 "
             "opus:AA-color12.c3 opus:AA-color13.c3 opus:AA-color14.c3 "
             "opus:AA-color15.c3 opus:AA-color16.c3 opus:AA-color17.c3 "
             "opus:AA-color18.c3 opus:AA-color19.c3 opus:AA-color20.c3 "
             "opus:AA-color21.c3 opus:AA-color22.c3 opus:AA-color23.c3 "
             "opus:AA-color24.c3 opus:AA-color25.c3 opus:AA-color26.c3 "
             "opus:AA-color27.c3 opus:AA-color28.c3 opus:AA-color29.c3 "
             "opus:AA-color30.c3 opus:AA-color31.c3 sol:AA-bplcon0.c3 "
             "sol:AA-color01.c3 sol:AA-color02.c3 sol:AA-color03.c3 "
             "sol:AA-color04.c3 sol:AA-color05.c3 sol:AA-color06.c3 "
             "sol:AA-color07.c3 sol:AA-color08.c3 sol:AA-color09.c3 "
             "sol:AA-color10.c3 sol:AA-color11.c3 sol:AA-color12.c3 "
             "sol:AA-color13.c3 sol:AA-color14.c3 sol:AA-color15.c3 "
             "sol:AA-color16.c3 sol:AA-color17.c3 sol:AA-color18.c3 "
             "sol:AA-color19.c3 sol:AA-color20.c3 sol:AA-color21.c3 "
             "sol:AA-color22.c3 sol:AA-color23.c3 sol:AA-color24.c3 "
             "sol:AA-color25.c3 sol:AA-color26.c3 sol:AA-color27.c3 "
             "sol:AA-color28.c3 sol:AA-color29.c3 sol:AA-color30.c3 "
             "sol:AA-color31.c3]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. Which silicon a block is fabricated "
             "in is a fact about the source tree and no port of denise carries "
             "it. [claims: opus:AA-bplcon2.c3/c4 opus:AA-bplcon3.c3 "
             "opus:AA-color00.c3 opus:AA-lisaid.c3 sol:AA-bplcon2.c3 "
             "sol:AA-bplcon3.c3 sol:AA-color00.c3 sol:AA-lisaid.c3]");
    c.record(
             "DECLINED at this boundary (insufficient): The register is decoded "
             "at this boundary, but no assertion in this package writes or reads "
             "it. The package grades the rows its own subject names; this row is "
             "another register of the same module. [claims: opus:AA-bplcon0.c1 "
             "opus:AA-color01.c1 opus:AA-color02.c1 opus:AA-color03.c1 "
             "opus:AA-color04.c1 opus:AA-color05.c1 opus:AA-color06.c1 "
             "opus:AA-color07.c1 opus:AA-color08.c1 opus:AA-color09.c1 "
             "opus:AA-color10.c1 opus:AA-color11.c1 opus:AA-color12.c1 "
             "opus:AA-color13.c1 opus:AA-color14.c1 opus:AA-color15.c1 "
             "opus:AA-color16.c1 opus:AA-color17.c1 opus:AA-color18.c1 "
             "opus:AA-color19.c1 opus:AA-color20.c1 opus:AA-color21.c1 "
             "opus:AA-color22.c1 opus:AA-color23.c1 opus:AA-color24.c1 "
             "opus:AA-color25.c1 opus:AA-color26.c1 opus:AA-color27.c1 "
             "opus:AA-color28.c1 opus:AA-color29.c1 opus:AA-color30.c1 "
             "opus:AA-color31.c1 sol:AA-bplcon0.c1 sol:AA-color01.c1 "
             "sol:AA-color02.c1 sol:AA-color03.c1 sol:AA-color04.c1 "
             "sol:AA-color05.c1 sol:AA-color06.c1 sol:AA-color07.c1 "
             "sol:AA-color08.c1 sol:AA-color09.c1 sol:AA-color10.c1 "
             "sol:AA-color11.c1 sol:AA-color12.c1 sol:AA-color13.c1 "
             "sol:AA-color14.c1 sol:AA-color15.c1 sol:AA-color16.c1 "
             "sol:AA-color17.c1 sol:AA-color18.c1 sol:AA-color19.c1 "
             "sol:AA-color20.c1 sol:AA-color21.c1 sol:AA-color22.c1 "
             "sol:AA-color23.c1 sol:AA-color24.c1 sol:AA-color25.c1 "
             "sol:AA-color26.c1 sol:AA-color27.c1 sol:AA-color28.c1 "
             "sol:AA-color29.c1 sol:AA-color30.c1 sol:AA-color31.c1]");
    c.record(
             "DECLINED at this boundary (not-hardware): A lower-case `h` or `p` "
             "in the revision column says which chip set introduced the "
             "register. That is a statement about the document's history of the "
             "silicon, not about behaviour. [claims: opus:AA-lisaid.c4 "
             "sol:AA-lisaid.c4]");
    c.record(
             "DECLINED at this boundary (outside-uut): The `~` mark is a "
             "Coprocessor write restriction enforced in agnus_copper's cdang "
             "path and graded by aa-copper-register-map. [claims: "
             "opus:AA-lisaid.c5 sol:AA-lisaid.c5]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
