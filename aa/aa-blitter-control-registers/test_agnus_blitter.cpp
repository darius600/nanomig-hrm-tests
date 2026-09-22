// aa-blitter-control-registers -- agnus_blitter conformance test
//
// Test purpose: the AA specification's address-ordered register list gives the
//   blitter's twelve control and data registers an address each, and its
//   function column says which of them START a blit and which only hold a
//   value. This grades the addresses, the two that start, and the ECS
//   condition on the two the document marks as new.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 6 and 7, the
//   address-ordered register list. The document prints no page numbers of its
//   own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the minterm truth table, the fill
//   modes, line mode, the barrel shifters' shift amounts, descending blits, or
//   the DMA slot handshake beyond granting it so a blit can finish.
//
// WHICH ADDRESS STARTS A BLIT IS ITSELF A CLAIM, AND THE MODULE MAKES IT
//   TESTABLE IN ONE LINE:
//
//     busy <= 1 when reg_address_in == BLTSIZE || (reg_address_in == BLTSIZH
//                                                  && ecs)                (:500)
//
//   so `busy`, an output, answers directly. BLTSIZV at $05C writes the height
//   and does NOT start, which is the negative half of the same claim and the
//   reason assertion 3 exists: an address that starts everything would pass a
//   test that only ever wrote the two that should.
//
// AND BLTSIZH IS CONDITIONAL. Its row is marked as new in the HiRes chip set,
//   and the module gates it on `ecs`. With ecs low a write at $05E does
//   nothing at all, which is the same shape as BEAMCON0's VARBEAMEN gating the
//   beam limits in aa-beam-counter-registers: the register and its condition
//   grade each other, and one reading of either alone proves nothing.
//
// HOW A BLIT IS RUN WITHOUT MEMORY. With USEA clear the A data comes from
//   BLTADAT as the processor wrote it rather than from a fetch (:326, :252),
//   and with USED set the result leaves through `data_out` while `we` is high
//   (:490). So a four-word blit with minterm $F0 -- D = A -- carries BLTADAT
//   out unchanged, and that is the path assertions 4 to 6 measure. Nothing
//   here needs a memory model.

#include "Vagnus_blitter.h"
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
constexpr uint16_t A_BLTDDAT  = 0x000;
constexpr uint16_t A_BLTCON0  = 0x040;
constexpr uint16_t A_BLTCON1  = 0x042;
constexpr uint16_t A_BLTAFWM  = 0x044;
constexpr uint16_t A_BLTALWM  = 0x046;
constexpr uint16_t A_BLTSIZE  = 0x058;
constexpr uint16_t A_BLTCON0L = 0x05A;
constexpr uint16_t A_BLTSIZV  = 0x05C;
constexpr uint16_t A_BLTSIZH  = 0x05E;
constexpr uint16_t A_BLTCDAT  = 0x070;
constexpr uint16_t A_BLTBDAT  = 0x072;
constexpr uint16_t A_BLTADAT  = 0x074;

// USED only, minterm $F0: D takes A unchanged.
constexpr uint16_t CON0_D_TAKES_A = 0x0100 | 0x00F0;
// Four words, one line, in the OCS packing BLTSIZE uses: height in bits 15:6,
// width in bits 5:0.
//
// WHY NOT ONE WORD. The D channel asks for its cycle on `used & pipeline_full`
// (:735) and the pipeline is not full until an A cycle has been through it, so
// a one-word blit starts, runs four cycles and finishes having written
// nothing. The first version of this package used one word and read -1 from
// every assertion that watched the D channel -- which looked like a dead
// decoder and was a dead fixture.
constexpr uint16_t SIZE_FOUR_WORDS = (1u << 6) | 4u;

class Harness {
public:
    explicit Harness(bool ecs = true) : dut(new Vagnus_blitter) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->ecs = ecs ? 1 : 0;
        dut->clkena = 1; dut->enadma = 1; dut->ackdma = 0;
        dut->reg_address_in = 0; dut->data_in = 0;
        for (int i = 0; i < 8; ++i) step();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) step();
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

    bool busy() const { return dut->busy != 0; }

    // Grant every cycle the blitter asks for and capture the FIRST D word as it
    // leaves. First, not last, because BLTAFWM masks the first word of channel
    // A and the last-word mask is a different register; a fixture that kept
    // the last write would grade BLTALWM while claiming to grade BLTAFWM.
    // Returns -1 if the blit never wrote, which is a failure the assertion
    // should see rather than a hang.
    int run_blit(int limit = 400) {
        int got = -1;
        for (int i = 0; i < limit; ++i) {
            dut->ackdma = dut->reqdma;
            dut->eval();
            if (dut->we && got < 0) got = dut->data_out;
            step();
            if (!dut->busy) break;
        }
        dut->ackdma = 0;
        dut->eval();
        return got;
    }

    Vagnus_blitter* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    const bool mutate = getenv("MUTATE") != nullptr;

    // MUTATION. Every expect() is guarded so MUTATE=1 forces it red, and each
    // guard moves ONE thing: the address written, the data pattern, or the ecs
    // input that decides whether an address is decoded at all.
    const uint16_t m_size  = mutate ? A_BLTSIZV : A_BLTSIZE;   // 2: never starts
    const bool     m_ecs   = mutate ? true : false;            // 3: the ecs-less half
    const uint16_t m_sizv  = mutate ? A_BLTSIZE : A_BLTSIZV;   // 4: starts after all
    const uint16_t m_adat  = mutate ? A_BLTBDAT : A_BLTADAT;   // 5: A never loaded
    const uint16_t m_con0  = mutate ? A_BLTCON1 : A_BLTCON0;   // 6: minterm never set
    const uint16_t m_afwm  = mutate ? A_BLTALWM : A_BLTAFWM;   // 7: the mask never set
    const uint16_t m_live  = mutate ? A_BLTSIZV : A_BLTSIZE;   // liveness

    c.record("oracle: AA specification, PDF pp. 6-7, the address-ordered "
             "register list. BLTDDAT $000, BLTCON0 $040, BLTCON1 $042, BLTAFWM "
             "$044, BLTALWM $046, BLTSIZE $058, BLTCON0L $05A, BLTSIZV $05C, "
             "BLTSIZH $05E, BLTCDAT $070, BLTBDAT $072, BLTADAT $074 "
             "[no page -- the package's own oracle line]");

    c.record("DECLINED at this boundary: the list's chip column gives all "
             "twelve to Agnus/Alice, and marks them `~` for the Coprocessor "
             "and `&` or `%` for DMA use. None of the three is visible at any "
             "port of this module [claims: sol:AA-bltcon0.c3 sol:AA-bltadat.c3 "
             "sol:AA-bltsize.c3 sol:AA-bltcon0.c4 sol:AA-bltadat.c4 "
             "sol:AA-bltsize.c4]");

    // --- FIXTURE LIVENESS ---------------------------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red MUTATE=1 writes
        // the size at $05C, which sets the height and starts nothing.
        h.write(A_BLTCON0, CON0_D_TAKES_A);
        h.write(A_BLTADAT, 0xBEEF);
        h.write(A_BLTAFWM, 0xFFFF);
        h.write(A_BLTALWM, 0xFFFF);
        h.write(m_live, SIZE_FOUR_WORDS);
        const int got = h.run_blit();
        c.note("a four-word D=A blit with BLTADAT $BEEF wrote " + hex((uint32_t)got));
        c.expect(got == 0xBEEF,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: a blit started by address "
                 "carries a distinct pattern from BLTADAT out through the D "
                 "channel, so a later assertion that reads nothing means the "
                 "decoder and not the fixture [no page -- infrastructure]");
    }

    // --- 2. BLTSIZE starts a blit -------------------------------------------
    {
        Harness h;
        const bool before = h.busy();
        // MUTATION BLTSIZE-AT-058-STARTS stimulus red MUTATE=1 writes at $05C,
        // which is the height register and starts nothing.
        h.write(m_size, SIZE_FOUR_WORDS);
        const bool after = h.busy();
        c.note("busy was " + std::to_string(before) + " before the write at "
               "$058 and " + std::to_string(after) + " after");
        c.expect(!before && after,
                 "BLTSIZE-AT-058-STARTS: a write at $058 sets the blitter busy, "
                 "which is the address and the starting function the AA list "
                 "prints for BLTSIZE [p.6] [claims: sol:AA-bltsize.c1/c2 "
                 "opus:AA-bltsize.c1/c2]");
    }

    // --- 3. BLTSIZH starts a blit, but only with ECS ------------------------
    {
        Harness h(true);
        h.write(A_BLTSIZH, 1);
        const bool with_ecs = h.busy();
        // MUTATION BLTSIZH-AT-05E-NEEDS-ECS stimulus red MUTATE=1 raises ecs
        // for the half that is supposed to do nothing.
        Harness g(m_ecs);
        g.write(A_BLTSIZH, 1);
        const bool without_ecs = g.busy();
        c.note("a write at $05E set busy to " + std::to_string(with_ecs)
               + " with ECS and " + std::to_string(without_ecs) + " without");
        c.expect(with_ecs && !without_ecs,
                 "BLTSIZH-AT-05E-NEEDS-ECS: a write at $05E starts a blit only "
                 "when the ECS extensions are enabled, which is the address the "
                 "AA list prints for BLTSIZH and the chip-set condition its row "
                 "carries [p.6] [claims: sol:AA-bltsizh.c1/c2 "
                 "opus:AA-bltsizh.c1/c2]");
    }

    // --- 4. BLTSIZV holds the height and does NOT start ---------------------
    {
        Harness h;
        // MUTATION BLTSIZV-AT-05C-DOES-NOT-START stimulus red MUTATE=1 writes
        // at $058 instead, which does start one.
        h.write(m_sizv, 4);
        const bool started = h.busy();
        c.note("after a write at $05C, busy reads " + std::to_string(started));
        c.expect(!started,
                 "BLTSIZV-AT-05C-DOES-NOT-START: a write at $05C leaves the "
                 "blitter idle, so the two addresses the AA list gives a "
                 "starting function are the only two that have one [p.6] "
                 "[claims: sol:AA-bltsizv.c1/c2 opus:AA-bltsizv.c1/c2]");
    }

    // --- 5. BLTADAT is the A source when no channel fetches it --------------
    {
        Harness h;
        h.write(A_BLTCON0, CON0_D_TAKES_A);
        // MUTATION BLTADAT-AT-074 stimulus red MUTATE=1 writes the pattern at
        // $072, so channel A keeps its reset value of zero.
        h.write(m_adat, 0x1234);
        h.write(A_BLTAFWM, 0xFFFF);
        h.write(A_BLTALWM, 0xFFFF);
        h.write(A_BLTSIZE, SIZE_FOUR_WORDS);
        const int got = h.run_blit();
        c.note("with BLTADAT $1234 and minterm $F0 the D channel wrote "
               + hex((uint32_t)got));
        c.expect(got == 0x1234,
                 "BLTADAT-AT-074: the word written at $074 is what channel A "
                 "contributes when no DMA channel fetches it, which is the "
                 "address the AA list prints for BLTADAT [p.7] [claims: "
                 "sol:AA-bltadat.c1/c2 opus:AA-bltadat.c1/c2]");
    }

    // --- 6. BLTCON0 carries the minterm -------------------------------------
    {
        Harness h;
        // MUTATION BLTCON0-AT-040 stimulus red MUTATE=1 writes the control
        // word at $042, so the minterm and the D enable are never set.
        h.write(m_con0, 0x0100 | 0x0000);   // USED, minterm $00 -> D is zero
        h.write(A_BLTADAT, 0x5555);
        h.write(A_BLTAFWM, 0xFFFF);
        h.write(A_BLTALWM, 0xFFFF);
        h.write(A_BLTSIZE, SIZE_FOUR_WORDS);
        const int zeroed = h.run_blit();

        Harness g;
        g.write(A_BLTCON0, CON0_D_TAKES_A); // minterm $F0 -> D takes A
        g.write(A_BLTADAT, 0x5555);
        g.write(A_BLTAFWM, 0xFFFF);
        g.write(A_BLTALWM, 0xFFFF);
        g.write(A_BLTSIZE, SIZE_FOUR_WORDS);
        const int passed_through = g.run_blit();

        c.note("minterm $00 wrote " + hex((uint32_t)zeroed)
               + ", minterm $F0 wrote " + hex((uint32_t)passed_through)
               + " from the same BLTADAT $5555");
        c.expect(zeroed == 0x0000 && passed_through == 0x5555,
                 "BLTCON0-AT-040: the word written at $040 decides what the "
                 "blitter computes -- the same source produces zero under one "
                 "minterm and passes through under another -- which is the "
                 "address and the control function the AA list prints for "
                 "BLTCON0 [p.6] [claims: sol:AA-bltcon0.c1/c2 "
                 "opus:AA-bltcon0.c1/c2]");
    }

    // --- 7. BLTAFWM masks the first word of channel A -----------------------
    {
        Harness h;
        h.write(A_BLTCON0, CON0_D_TAKES_A);
        h.write(A_BLTADAT, 0xFFFF);
        // MUTATION BLTAFWM-AT-044 stimulus red MUTATE=1 writes the mask at
        // $046, so the first-word mask keeps its reset value.
        h.write(m_afwm, 0x0F0F);
        h.write(A_BLTALWM, 0xFFFF);
        h.write(A_BLTSIZE, SIZE_FOUR_WORDS);
        const int got = h.run_blit();
        c.note("with BLTADAT $FFFF and a first-word mask of $0F0F the D "
               "channel wrote " + hex((uint32_t)got));
        c.expect(got == 0x0F0F,
                 "BLTAFWM-AT-044: the word written at $044 masks the first word "
                 "channel A contributes, which is the address and the "
                 "first-word-mask function the AA list prints for BLTAFWM "
                 "[p.6] [claims: sol:AA-bltafwm.c1/c2 opus:AA-bltafwm.c1/c2]");
    }

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
