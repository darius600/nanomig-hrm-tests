// aa-copper-register-map -- agnus_copper conformance test
//
// Test purpose: the AA specification's address-ordered register list gives an
//   address, an access type and a chip for each of the Copper's eight
//   registers. This grades the ADDRESSES against the decoder: for each, a
//   write at the printed address must reach the thing that address names, and
//   a write one register away must not.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 6 and 7, the
//   address-ordered register list. The document prints no page numbers of its
//   own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the Copper's instruction set, its
//   WAIT/SKIP comparison, its DMA priority, or the CDANG access ranges beyond
//   the one bit that makes COPCON observable. It grades where eight registers
//   are decoded and nothing else.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the AA specification independently, and ids such as
//   `opus:AA-cop1lch.c1` and `opus:AA-cop1lcl.c1` each name one reader's
//   reading of one sentence. They resolve in the claim files that
//   hrm-testpipeline/acceptance/aa-s7-rest.json lists under `inventories`.
//
// WHY THESE EIGHT AND NOT THE OTHER EIGHTY-FIVE. Of the 236 registers the AA
//   address list gives an address for, 93 are also declared as an address in
//   NanoMig's RTL -- the union of all 38 independent readings. These eight
//   are one module, one register family, one boundary.
//   tools/aa/lot_candidates.py reports where the rest sit, by the module
//   that declares each address.
//
// HOW THE ADDRESSES ARE OBSERVED. agnus_copper exposes no register read path.
//   Each address is reached through the one effect that makes it visible at
//   this boundary:
//
//     COP1LCH/COP1LCL  -> address_out, after a COPJMP1 strobe loads the
//                         pointer:  address_out[20:1] <= {cop1lch[20:16],
//                         cop1lcl[15:1]}   (agnus_copper.v:196)
//     COP2LCH/COP2LCL  -> the same through COPJMP2                    (:198)
//     COPJMP1/COPJMP2  -> which of the two pointers arrives. A strobe that
//                         does not decode loads neither.
//     COPCON           -> cdang <= data_in[1] (:178), which decides whether a
//                         MOVE to a low register is illegal (:225)
//     COPINS           -> ir2 <= data_in (:186), and ir2[8:1] is what
//                         reg_address_out carries during a MOVE       (:216)
//
// THE MODULE IS BEAM-DRIVEN AND THE FIXTURE HAS TO BE TOO. `clk_ena = hpos[0]`
//   (:131), `bus_ena` toggles on every other cck, and `enable = ~bus_blk &
//   bus_ena & clk_ena` (:360). A pointer load needs `dma_ack && strobe1 &&
//   copper_state==RESET`, and `dma_ack = ackdma & enable` (:363). So hpos must
//   advance like a real horizontal counter or nothing in this module ever
//   happens, and a fixture that holds hpos still reads as a dead decoder.

#include "Vagnus_copper.h"
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

// Printing a value with std::to_string and an "0x" in front of it prints the
// DECIMAL digits under a hexadecimal label. The first run of this package
// reported "address_out as 0x54898" for a value that was 0xD672 -- decimal
// 54898 -- and sent me looking for a decoder fault that was not there. The
// fixture had been right the whole time; the note was the lie.
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
constexpr uint16_t A_COPCON  = 0x02E;
constexpr uint16_t A_COP1LCH = 0x080;
constexpr uint16_t A_COP1LCL = 0x082;
constexpr uint16_t A_COP2LCH = 0x084;
constexpr uint16_t A_COP2LCL = 0x086;
constexpr uint16_t A_COPJMP1 = 0x088;
constexpr uint16_t A_COPJMP2 = 0x08A;
constexpr uint16_t A_COPINS  = 0x08C;

class Harness {
public:
    explicit Harness(bool ecs) : dut(new Vagnus_copper) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->ecs = ecs ? 1 : 0;
        dut->ackdma = 0; dut->enadma = 1; dut->sof = 0; dut->blit_busy = 0;
        dut->vpos = 0x20; dut->data_in = 0; dut->reg_address_in = 0;
        hpos = 0x40;
        for (int i = 0; i < 16; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 8; ++i) step();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // One colour clock. hpos advances because the module's own enable chain
    // hangs off it -- see the header note.
    void step() {
        dut->hpos = hpos & 0x1FF;
        dut->clk = 1; dut->eval();
        dut->clk = 0; dut->eval();
        hpos = (hpos + 1) & 0x1FF;
        if ((hpos & 0x1FF) == 0) hpos = 0x40;   // stay clear of $E1/$E2
    }

    void write(uint16_t word_addr, uint16_t value) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->data_in = value;
        step();
        dut->reg_address_in = 0;
        dut->data_in = 0;
        step();
    }

    // Grant DMA until the copper has taken a slot and the pointer has had a
    // chance to load. 64 ccks is far more than the handshake needs; the loop
    // stops early so a failure is a real one rather than a timeout.
    uint32_t run_until_pointer_loads(uint32_t before) {
        dut->ackdma = 1;
        for (int i = 0; i < 64; ++i) {
            step();
            if (address_out() != before) break;
        }
        dut->ackdma = 0;
        for (int i = 0; i < 4; ++i) step();
        return address_out();
    }

    uint32_t address_out() const { return dut->address_out & 0xFFFFF; }
    uint8_t  reg_address_out() const { return dut->reg_address_out & 0xFF; }

    Vagnus_copper* dut;
    uint32_t hpos;
};

// Drive a pointer pair, strobe, and report the chip address that arrives.
static uint32_t load_via(Harness& h, uint16_t hi_addr, uint16_t lo_addr,
                         uint16_t strobe_addr, uint32_t pattern) {
    const uint32_t before = h.address_out();
    h.write(hi_addr, static_cast<uint16_t>((pattern >> 16) & 0x1F));
    h.write(lo_addr, static_cast<uint16_t>(pattern & 0xFFFE));
    h.write(strobe_addr, 0);
    return h.run_until_pointer_loads(before);
}

// Does the Copper ever put `want` on reg_address_out within `ccks`?
//
// The bus alternates: COPINS[8:1] on the instruction fetch, then ir2[8:1] on
// the MOVE it executes, then COPINS again, forever. A scan that stops at the
// first value other than 0xFF stops on the FETCH every time and never sees the
// MOVE -- which is how this package first read a correct decoder as broken.
static bool emits(Harness& h, uint8_t want, int ccks = 60) {
    bool seen = false;
    h.dut->ackdma = 1;
    for (int i = 0; i < ccks && !seen; ++i) {
        h.step();
        if (h.reg_address_out() == want) seen = true;
    }
    h.dut->ackdma = 0;
    for (int i = 0; i < 4; ++i) h.step();
    return seen;
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    g_mutate = std::getenv("MUTATE");

    // MUTATION. Each guard below tests its own assertion's label and moves
    // ONE address written -- no guard touches a comparison. MUTATE=<label>
    // selects exactly one.
    //
    // THE FIRST VERSION OF THIS SWAPPED BOTH SIDES AND FOUR OF SIX ASSERTIONS
    // STAYED GREEN. Exchanging COP1LCH with COP2LCH *and* COP1LCL with COP2LCL
    // *and* COPJMP1 with COPJMP2 writes both pairs and fires both strobes, so
    // the same value arrives by the mirrored route and nothing goes red. A
    // symmetric mutation of a symmetric pair is not a mutation. Each assertion
    // now breaks its own pairing on ONE side, so the register that should have
    // received the pattern keeps its reset value instead.
    // EVERY PATTERN MUST HAVE NON-ZERO HIGH BITS, or the mutation writes a
    // zero into the wrong register and nothing moves. 0x0AAAA has none:
    // (0x0AAAA >> 16) == 0, so assertion 1 stayed green under mutation until
    // the pattern became 0x1AAAA. A mutation the test data cannot express is
    // not a mutation.
    const uint16_t m_cop1lch = mutating("COP1LC-AT-080-082") ? A_COP2LCH : A_COP1LCH;   // 1: high word lost
    const uint16_t m_cop2lch = mutating("COP2LC-AT-084-086") ? A_COP1LCH : A_COP2LCH;   // 2: high word lost
    const uint16_t m_cop2lcl = mutating("COP1-AND-COP2-ARE-DISTINCT") ? A_COP1LCL : A_COP2LCL;   // 3: pointer 2 never written
    const uint16_t m_copins  = mutating("COPINS-AT-08C") ? A_COPCON  : A_COPINS;    // 4: ir2 never written
    const uint16_t m_copcon  = mutating("COPCON-AT-02E") ? A_COPINS  : A_COPCON;    // 5: cdang never written
    const uint16_t m_live    = mutating("FIXTURE-DELIVERS-KNOWN-VALUE") ? A_COP2LCH : A_COP1LCH;   // liveness, same shape
    const uint16_t m_sprhdat_con = mutating("SPRHDAT-078-IS-RESTRICTED") ? A_COPINS : A_COPCON; // 6: cdang never set

    c.record("oracle: AA specification, PDF pp. 6-7, the address-ordered "
             "register list. COPCON $02E, COP1LCH $080, COP1LCL $082, "
             "COP2LCH $084, COP2LCL $086, COPJMP1 $088, COPJMP2 $08A, "
             "COPINS $08C [AA-copper-map.c1]");

    // --- FIXTURE LIVENESS, before any address assertion leans on it --------
    {
        Harness h(true);
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: selecting this
        // assertion's own id sends the high word to $084, so the pattern
        // arrives without its top five bits.
        const uint32_t got = load_via(h, m_live, A_COP1LCL, A_COPJMP1, 0x1ACE4);
        // address_out is declared [20:1]: it carries bits 20 down to 1 of a
        // byte address, so a byte-address pattern arrives shifted right once.
        c.note("a $1ACE4 pattern written through COP1LCH/COP1LCL and strobed "
               "arrives at address_out as " + hex(got) + ", expected "
               + hex(0x1ACE4 >> 1));
        c.expect(got == (0x1ACE4 >> 1),
                 "FIXTURE-DELIVERS-KNOWN-VALUE: the beam-driven handshake this "
                 "fixture builds does reach address_out with a distinct known "
                 "pattern, so a later address assertion that reads no change "
                 "means the decoder and not the fixture [no page -- "
                 "infrastructure]");
    }

    // --- 1. COP1LCH / COP1LCL / COPJMP1 -----------------------------------
    {
        Harness h(true);
        // MUTATION COP1LC-AT-080-082 stimulus red: selecting this assertion's
        // own id writes the high word at $084 instead of $080, so cop1lch
        // keeps its reset value of zero.
        const uint32_t got = load_via(h, m_cop1lch, A_COP1LCL, A_COPJMP1, 0x1AAAA);
        c.note("through $080/$082 strobed at $088, address_out = " + hex(got)
               + ", expected " + hex(0x1AAAA >> 1));
        c.expect(got == (0x1AAAA >> 1),
                 "COP1LC-AT-080-082: a 20-bit pattern written at $080 and $082 "
                 "and strobed at $088 arrives at address_out, so COP1LCH, "
                 "COP1LCL and COPJMP1 are decoded where the address list puts "
                 "them [claims: opus:AA-cop1lch.c1 opus:AA-cop1lcl.c1 opus:AA-copjmp1.c1 sol:AA-cop1lch.c1 sol:AA-cop1lcl.c1 sol:AA-copjmp1.c1]");
    }

    // --- 2. COP2LCH / COP2LCL / COPJMP2 -----------------------------------
    {
        Harness h(true);
        // MUTATION COP2LC-AT-084-086 stimulus red: selecting this assertion's
        // own id writes the high word at $080 instead of $084, so cop2lch
        // keeps its reset value of zero.
        const uint32_t got = load_via(h, m_cop2lch, A_COP2LCL, A_COPJMP2, 0x15554);
        c.note("through $084/$086 strobed at $08A, address_out = " + hex(got)
               + ", expected " + hex(0x15554 >> 1));
        c.expect(got == (0x15554 >> 1),
                 "COP2LC-AT-084-086: a 20-bit pattern written at $084 and $086 "
                 "and strobed at $08A arrives at address_out, so COP2LCH, "
                 "COP2LCL and COPJMP2 are decoded where the address list puts "
                 "them [claims: opus:AA-cop2lch.c1 opus:AA-cop2lcl.c1 opus:AA-copjmp2.c1 sol:AA-cop2lch.c1 sol:AA-cop2lcl.c1 sol:AA-copjmp2.c1]");
    }

    // --- 3. The two pointers are separate, which is what makes 1 and 2 an
    //        address measurement rather than one register answering twice.
    {
        Harness h(true);
        load_via(h, A_COP1LCH, A_COP1LCL, A_COPJMP1, 0x1AAAA);
        // MUTATION COP1-AND-COP2-ARE-DISTINCT stimulus red: selecting this
        // assertion's own id writes the low word at $082, so pointer 2's low
        // half is never written at all.
        const uint32_t second = load_via(h, A_COP2LCH, m_cop2lcl, A_COPJMP2,
                                         0x15554);
        c.expect(second == (0x15554 >> 1),
                 "COP1-AND-COP2-ARE-DISTINCT: loading pointer 2 after pointer 1 "
                 "yields pointer 2's value, so $084/$086 are not aliases of "
                 "$080/$082 -- a CONTROL for the two address assertions above, "
                 "grading no claim of its own");
        // NO CLAIMS BRACKET ON THIS LINE, ON PURPOSE. It cited
        // opus:AA-cop2lch.c2, which in slice 7's union is "the address-ordered
        // list gives COP2LCH the access letters W" -- the access column, which
        // this assertion does not read and which the residue lot declines. What
        // this assertion establishes is that assertions 1 and 2 measure two
        // registers rather than one register answering twice; the address
        // claims it supports are cited on those two lines.
    }

    // --- 4. COPINS at $08C, seen through ir2 on reg_address_out -----------
    {
        Harness h(true);
        // A MOVE whose destination is a high register is always legal, so what
        // reg_address_out carries on the execute cycle is ir2[8:1] and nothing
        // else decides it. Two writes: ir1 <= ir2, ir2 <= data.
        // MUTATION COPINS-AT-08C stimulus red: selecting this assertion's
        // own id writes the instruction word at $02E instead of $08C, so
        // ir2 is never loaded.
        h.write(m_copins, 0x0180);
        h.write(m_copins, 0x0180);
        const uint8_t want = (0x180 >> 1) & 0xFF;
        const bool seen = emits(h, want);
        c.note(std::string("with $0180 written at $08C, reg_address_out ")
               + (seen ? "does" : "does NOT") + " carry " + hex(want)
               + " on a MOVE cycle");
        c.expect(seen,
                 "COPINS-AT-08C: a word written at $08C becomes the register "
                 "address the Copper puts out for a MOVE, so COPINS is decoded "
                 "at $08C [claims: opus:AA-copins.c1 sol:AA-copins.c1]");
    }

    // --- 5. COPCON at $02E, seen through cdang's effect on a low MOVE -----
    {
        // ir2[8:7]==00 is a destination below $080. With cdang clear that is an
        // illegal access (agnus_copper.v:225) and the MOVE is not emitted; with
        // cdang set on ECS it is allowed. COPCON bit 1 is the only thing that
        // moves between the two runs, so the difference is the measurement.
        const uint8_t want = (0x040 >> 1) & 0xFF;

        // MUTATION COPCON-AT-02E stimulus red: selecting this assertion's
        // own id writes the danger bit at $08C instead of $02E, so cdang
        // never changes and both runs agree.
        Harness dark(true);
        dark.write(m_copcon, 0x0000);            // cdang = 0
        dark.write(A_COPINS, 0x0040);
        dark.write(A_COPINS, 0x0040);
        const bool blocked = emits(dark, want);

        Harness lit(true);
        lit.write(m_copcon, 0x0002);             // cdang = 1, via data_in[1]
        lit.write(A_COPINS, 0x0040);
        lit.write(A_COPINS, 0x0040);
        const bool allowed = emits(lit, want);

        c.note(std::string("a MOVE to $040 reaches reg_address_out with "
                           "cdang=0: ") + (blocked ? "yes" : "no")
               + ", with cdang=1: " + (allowed ? "yes" : "no"));
        c.expect(!blocked && allowed,
                 "COPCON-AT-02E: writing bit 1 at $02E decides whether a MOVE "
                 "to a register below $080 reaches reg_address_out, so COPCON "
                 "is decoded at $02E [claims: opus:AA-copcon.c1 sol:AA-copcon.c1]");
    }

    // --- 6. The one address in the restricted range the list does not mark
    {
        // The list marks a register `~` to say the Coprocessor cannot write it
        // unless CDANG is set. agnus_copper implements that as a RANGE --
        // ir2[8:7]==00, every address below $080 (agnus_copper.v:225) -- and
        // over 237 address-list rows, 58 marked `~`, the range covers 59: 58 agree. SPRHDAT at
        // $078 is the single exception: it sits between BLTADAT `& ~074` and
        // LISAID `h ~07C`, both marked, and carries `&h` with no tilde at all.
        //
        // This grades what the RTL does there. The document's omission is
        // NM-AA-002 and is not decided here.
        const uint8_t want = (0x078 >> 1) & 0xFF;

        Harness dark(true);
        dark.write(A_COPCON, 0x0000);            // cdang = 0
        dark.write(A_COPINS, 0x0078);
        dark.write(A_COPINS, 0x0078);
        const bool blocked = emits(dark, want);

        Harness lit(true);
        lit.write(m_sprhdat_con, 0x0002);        // cdang = 1
        lit.write(A_COPINS, 0x0078);
        lit.write(A_COPINS, 0x0078);
        const bool allowed = emits(lit, want);

        c.note(std::string("a MOVE to $078 reaches reg_address_out with "
                           "cdang=0: ") + (blocked ? "yes" : "no")
               + ", with cdang=1: " + (allowed ? "yes" : "no"));
        // MUTATION SPRHDAT-078-IS-RESTRICTED stimulus red: selecting this
        // assertion's own id writes the danger bit at $08C instead of $02E
        // in the SECOND run, so cdang is never SET and both halves come
        // back blocked. Mutating the first run
        // instead achieves nothing: cdang is already zero out of reset, so
        // removing the write that clears it changes no behaviour at all.
        c.expect(!blocked && allowed,
                 "SPRHDAT-078-IS-RESTRICTED: $078 is blocked from Coprocessor "
                 "access with CDANG clear and allowed with it set, exactly like "
                 "every neighbouring address the list marks `~` -- and $078 is "
                 "the one address in $000-$07E the list leaves unmarked "
                 "[claims: opus:AA-sprhdat-tilde.c1]");
    }

    // --- 6. The document says S for COPJMP1/2 and W for the rest. Measured,
    //        not graded: this boundary has no read path to falsify W with.
    {
        Harness h(true);
        c.record("the AA list marks COPJMP1 and COPJMP2 access `S` (strobe) and "
                 "the other six `W`. agnus_copper exposes no read path for any "
                 "of the eight, so the W/R half of each row is not falsifiable "
                 "at this boundary and is recorded rather than graded "
                 "[AA-copper-map.n1]");
        c.record("the list marks COP1LCH, COP1LCL, COP2LCH and COP2LCL with "
                 "`+` -- an address register pair, low word DB1-DB15, high word "
                 "DB0-DB4. Assertions 1 and 2 write 5 bits high and 15 bits low "
                 "and read 20 bits back, which is that shape, but the package "
                 "does not grade the bit-lane statement itself "
                 "[AA-copper-map.n2]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "access column -- W, R or S. A module answers reads at the "
             "addresses it decodes and zero everywhere else, so a write-only row "
             "reads as the same silence an undecoded address reads as, and a "
             "strobe has no read at all. [claims: opus:AA-cop1lch.c2 "
             "opus:AA-cop1lcl.c2 opus:AA-cop2lch.c2 opus:AA-cop2lcl.c2 "
             "opus:AA-copcon.c2 opus:AA-copins.c2 opus:AA-copjmp1.c2 "
             "opus:AA-copjmp2.c2 sol:AA-cop1lch.c2 sol:AA-cop1lcl.c2 "
             "sol:AA-cop2lch.c2 sol:AA-cop2lcl.c2 sol:AA-copcon.c2 "
             "sol:AA-copins.c2 sol:AA-copjmp1.c2 sol:AA-copjmp2.c2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "chip column, which says which 1985 silicon a register lives in. No "
             "port of any module in this design carries a chip identity. "
             "[claims: opus:AA-cop1lch.c3 opus:AA-cop1lcl.c3 opus:AA-cop2lch.c3 "
             "opus:AA-cop2lcl.c3 opus:AA-copcon.c3 opus:AA-copins.c3 "
             "opus:AA-copjmp1.c3 opus:AA-copjmp2.c3 sol:AA-cop1lch.c3 "
             "sol:AA-cop1lcl.c3 sol:AA-cop2lch.c3 sol:AA-cop2lcl.c3 "
             "sol:AA-copcon.c3 sol:AA-copins.c3 sol:AA-copjmp1.c3 "
             "sol:AA-copjmp2.c3]");
    c.record(
             "DECLINED at this boundary (insufficient): This package writes and "
             "reads the register -- that is how its address row is graded here "
             "-- but no assertion distinguishes what this row says about it: the "
             "function column's width or role, the read-and-clear, the restart. "
             "A stimulus that passes whether or not the row is true does not "
             "grade it. [claims: opus:AA-cop1lch.c5 opus:AA-cop1lcl.c5 "
             "opus:AA-cop2lch.c5 opus:AA-cop2lcl.c5 opus:AA-copcon.c5 "
             "opus:AA-copins.c4 opus:AA-copjmp1.c4 opus:AA-copjmp2.c4 "
             "sol:AA-cop1lch.c5 sol:AA-cop1lcl.c5 sol:AA-cop2lch.c5 "
             "sol:AA-cop2lcl.c5 sol:AA-copins.c4 sol:AA-copjmp1.c4 "
             "sol:AA-copjmp2.c4]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is a legend "
             "statement, a gap between two printed rows, or a reserved line -- "
             "the document describing its own notation and layout. The one "
             "legend mark that IS a hardware restriction, the `~` Coprocessor "
             "limit, is graded by this package's own assertion. [claims: "
             "opus:AA-alpha-header.c1 opus:AA-alpha-order.c1 "
             "opus:AA-audfamily-spelling.c1 opus:AA-cop1lch.c4 "
             "opus:AA-cop1lcl.c4 opus:AA-cop2lch.c4 opus:AA-cop2lcl.c4 "
             "opus:AA-copcon.c4 opus:AA-gap068.c1/c2 opus:AA-gap06A.c1/c2 "
             "opus:AA-gap06C.c1/c2 opus:AA-gap06E.c1/c2 opus:AA-gap076.c1/c2 "
             "opus:AA-gap0AC.c1 opus:AA-gap0AE.c1 opus:AA-gap0BC.c1 "
             "opus:AA-gap0BE.c1 opus:AA-gap0CC.c1 opus:AA-gap0CE.c1 "
             "opus:AA-gap0DC.c1 opus:AA-gap0DE.c1 opus:AA-legend10-H.c1/c2 "
             "opus:AA-legend10-P.c1/c2 opus:AA-legend10-chip.c1 "
             "opus:AA-legend10-er.c1/c2/c3 opus:AA-legend10-h.c1 "
             "opus:AA-legend10-missing.c1 opus:AA-legend10-p.c1 "
             "opus:AA-legend10-ph.c1 opus:AA-legend10-wr.c1 "
             "opus:AA-legend6-amp.c1 opus:AA-legend6-chip.c1/c2/c3 "
             "opus:AA-legend6-er.c1/c2/c3 opus:AA-legend6-lc.c1/c2/c3/c4/c5 "
             "opus:AA-legend6-mod.c1/c2/c3/c4 opus:AA-legend6-pct.c1 "
             "opus:AA-legend6-plus.c1 opus:AA-legend6-pt.c1/c2/c3/c4/c5 "
             "opus:AA-legend6-s.c1 opus:AA-legend6-tilde.c1 "
             "opus:AA-legend6-wr.c1/c2 opus:AA-list-header.c1 "
             "opus:AA-list-order.c1 opus:AA-noop.c1/c2/c3 opus:AA-reserved.c1 "
             "sol:AA-cop1lch.c4 sol:AA-cop1lcl.c4 sol:AA-cop2lch.c4 "
             "sol:AA-cop2lcl.c4 sol:AA-copcon.c4 "
             "sol:AA-legends.c1/c2/c3/c6/c7/c8/c9/c10/c11/c12/c13/c14/c15/c16/c17/c18/c19/c20/c21/c22/c23/c24/c25/c26/c27/c28/c29/c30/c31/c32/c33/c34/c35/c36/c37/c38 "
             "sol:AA-no-op-null.c1/c2 sol:AA-reserved.c1]");

    std::cout << "\naa-copper-register-map: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed ? 1 : 0;
}
