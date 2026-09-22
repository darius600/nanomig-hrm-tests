// aa-beam-counter-registers -- agnus_beamcounter conformance test
//
// Test purpose: the AA specification's address-ordered register list gives the
//   beam counters an address each -- the readable pair VPOSR and VHPOSR, the
//   writable pair VPOSW and VHPOSW, BEAMCON0, and the eleven limit registers
//   that the `VARBEAMEN=1` note in the list's own function column makes
//   conditional. This grades the addresses, the two-field packing the readable
//   pair carries, and the condition itself.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 6, 9 and 10, the
//   address-ordered register list. The document prints no page numbers of its
//   own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the sync pulse shapes, blanking
//   windows, interlace field alternation, light-pen capture, or the NTSC long
//   line. It grades where seventeen registers are decoded and what two of them
//   pack.
//
// SEVEN OF SEVENTEEN, AND THE OTHER TEN ARE NOT IN THIS PACKAGE'S LOT AT ALL.
//   Graded here: VPOSR, VPOSW, VHPOSR, VHPOSW, BEAMCON0, HTOTAL, VTOTAL.
//   Not graded: HSSTRT, HSSTOP, HBSTRT, HBSTOP, HCENTER, VSSTRT, VSSTOP,
//   VBSTRT, VBSTOP and BPLCON0. They are observable in principle -- they drive
//   `_hsync`, `hblank`, `_vsync` and `vblank`, which are ports of this module
//   -- and grading them means running whole sync windows rather than watching
//   a counter wrap. They are left OUT of the lot rather than classified as
//   not-asserted, because "this package did not get to it" is not one of the
//   five reasons the vocabulary admits, and recording it as one would put a
//   scope decision where a measurement belongs. lot_candidates.py still
//   reports agnus_beamcounter as holding seventeen, so the gap stays visible.
//
// WHY THE LIMIT REGISTERS NEED BEAMCON0 TO BE GRADED AT ALL. The module makes
//   every one of them conditional on one bit:
//
//     varbeamen = beamcon0_reg[7]                                    (:135)
//     htotal    = varbeamen ? htotal_reg : HTOTAL_VAL << 1           (:223)
//     vtotal    = varbeamen ? vtotal_reg : pal ? ... : ...           (:229)
//
//   so a write to HTOTAL with varbeamen clear changes nothing observable, and
//   an assertion that wrote HTOTAL and watched the beam would pass on a module
//   that ignored the register entirely. BEAMCON0 and HTOTAL grade each other:
//   the same write is invisible with the bit clear and decisive with it set,
//   and the pair of readings is the measurement. The AA list says as much in
//   its own function column, which prints `(VARBEAMEN=1)` against these rows.
//
// HOW THE BEAM IS DRIVEN. `hpos[8:1]` advances on `cck` and resets when
//   `hpos[8:0] == {htotal[8:1], 1'b0}` (:250); `vpos` advances at end of line
//   and resets on `last_line`. So the fixture toggles cck and counts, and a
//   wrap is observed by reading VHPOSR rather than by trusting a cycle count.
//
// WHY long_frame IS CLEARED BEFORE THE VERTICAL WRAP IS MEASURED.
//   `last_line = long_frame ? extra_line : vpos_equ_vtotal` (:369), and
//   long_frame resets to 1, which puts the wrap one line later than VTOTAL
//   names. Clearing it through VPOSW bit 15 is itself one of the claims this
//   package grades, so the clearing is not a convenience -- it is the
//   assertion before the one that needs it.

#include "Vagnus_beamcounter.h"
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
constexpr uint16_t A_VPOSR    = 0x004;
constexpr uint16_t A_VHPOSR   = 0x006;
constexpr uint16_t A_VPOSW    = 0x02A;
constexpr uint16_t A_VHPOSW   = 0x02C;
constexpr uint16_t A_BEAMCON0 = 0x1DC;
constexpr uint16_t A_HTOTAL   = 0x1C0;
constexpr uint16_t A_VTOTAL   = 0x1C8;

constexpr uint16_t VARBEAMEN = 1u << 7;   // BEAMCON0 bit 7

class Harness {
public:
    Harness() : dut(new Vagnus_beamcounter) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->cck = 0; dut->ntsc = 0; dut->aga = 1; dut->ecs = 1; dut->a1k = 0;
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

    // One colour clock: the horizontal counter advances on cck's high half.
    void cck_tick() {
        dut->cck = 1; step();
        dut->cck = 0; step();
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
        const uint16_t got = dut->data_out;
        dut->reg_address_in = 0;
        dut->eval();
        return got;
    }

    // Where the horizontal counter wrapped, observed rather than counted:
    // run cck until the high byte of VHPOSR comes back smaller than it was.
    int hpos_wraps_after(int limit) {
        uint16_t prev = read(A_VHPOSR) & 0xFF;
        for (int i = 1; i <= limit; ++i) {
            cck_tick();
            const uint16_t now = read(A_VHPOSR) & 0xFF;
            if (now < prev) return (int)prev;
            prev = now;
        }
        return -1;
    }

    Vagnus_beamcounter* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    const bool mutate = getenv("MUTATE") != nullptr;

    // MUTATION. Every expect() is guarded so MUTATE=1 forces it red, and each
    // guard moves ONE thing: the address written, the address read, or the one
    // bit that decides whether a limit register is consulted.
    const uint16_t m_vhposw = mutate ? A_VPOSW  : A_VHPOSW;   // 2: hpos never set
    const uint16_t m_vposr  = mutate ? A_VHPOSR : A_VPOSR;    // 3: reads the other register
    const uint16_t m_lf     = mutate ? 0x8000u  : 0x0000u;    // 4: long_frame stays set
    const uint16_t m_varb   = mutate ? 0u       : VARBEAMEN;  // 5,6: limits never consulted
    const uint16_t m_htotal = mutate ? A_VTOTAL : A_HTOTAL;   // 6: htotal_reg never written
    const uint16_t m_vtotal = mutate ? A_HTOTAL : A_VTOTAL;   // 7: vtotal_reg never written
    const uint16_t m_live   = mutate ? A_VPOSW  : A_VHPOSW;   // liveness

    c.record("oracle: AA specification, PDF pp. 6, 9 and 10, the "
             "address-ordered register list. VPOSR $004, VHPOSR $006, VPOSW "
             "$02A, VHPOSW $02C, HTOTAL $1C0, VTOTAL $1C8, BEAMCON0 $1DC, and "
             "the limit registers' function column prints (VARBEAMEN=1) "
             "[no page -- the package's own oracle line]");

    c.record("DECLINED at this boundary: the list's chip column gives these "
             "registers to Agnus/Alice, and the `~` mark on VPOSR, VPOSW, "
             "VHPOSR and VHPOSW is a Coprocessor write restriction enforced in "
             "agnus_copper. Neither is visible at any port of this module "
             "[claims: sol:AA-vposr.c3 sol:AA-vposw.c3 sol:AA-vhposr.c3 "
             "sol:AA-vhposw.c3 sol:AA-vposr.c4 sol:AA-vposw.c4 "
             "sol:AA-vhposr.c4 sol:AA-vhposw.c4]");

    // --- FIXTURE LIVENESS ---------------------------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red MUTATE=1 writes
        // the word at $02A, which sets neither hpos nor vpos[7:0].
        h.write(m_live, 0x5A3C);
        const uint16_t got = h.read(A_VHPOSR);
        c.note("wrote $5A3C at $02C, VHPOSR reads " + hex(got));
        c.expect(got == 0x5A3C,
                 "FIXTURE-DELIVERS-KNOWN-VALUE: a word written at one address "
                 "and read at another carries a distinct pattern through this "
                 "module, so a later assertion that reads nothing means the "
                 "decoder and not the fixture [no page -- infrastructure]");
    }

    // --- 2. VHPOSW writes both fields, VHPOSR reads them back ---------------
    {
        Harness h;
        // MUTATION VHPOS-PACKS-VPOS-AND-HPOS stimulus red MUTATE=1 writes at
        // $02A, which takes only three bits and leaves hpos untouched.
        h.write(m_vhposw, 0x8F44);
        const uint16_t got = h.read(A_VHPOSR);
        c.note("wrote $8F44 at $02C; VHPOSR reads " + hex(got)
               + " -- vertical high byte " + hex(got >> 8)
               + ", horizontal low byte " + hex(got & 0xFF));
        c.expect(got == 0x8F44,
                 "VHPOS-PACKS-VPOS-AND-HPOS: one word at $02C sets the low "
                 "eight bits of the vertical counter and the horizontal "
                 "counter, and $006 reads the same two fields back in the same "
                 "order, which is the pair of addresses the AA list prints for "
                 "VHPOSW and VHPOSR [p.6] [claims: sol:AA-vhposw.c1/c2/c5 "
                 "opus:AA-vhposw.c1/c2 sol:AA-vhposr.c1/c2/c5 opus:AA-vhposr.c1/c2]");
    }

    // --- 3. VPOSW writes the vertical high bits, VPOSR reads them -----------
    {
        Harness h;
        h.write(A_VPOSW, 0x0005);
        // MUTATION VPOS-HIGH-BITS-AT-02A-AND-004 stimulus red MUTATE=1 reads
        // $006, whose low byte is the horizontal counter and not vpos[10:8].
        const uint16_t got = h.read(m_vposr);
        c.note("wrote $0005 at $02A; VPOSR reads " + hex(got)
               + ", its low three bits " + hex(got & 7));
        c.expect((got & 7) == 5,
                 "VPOS-HIGH-BITS-AT-02A-AND-004: the top three bits of the "
                 "vertical counter are written at $02A and read at $004, which "
                 "is the pair of addresses the AA list prints for VPOSW and "
                 "VPOSR [p.6] [claims: sol:AA-vposw.c1/c2 opus:AA-vposw.c1/c2 "
                 "sol:AA-vposr.c1/c2 opus:AA-vposr.c1/c2]");
    }

    // --- 4. VPOSW bit 15 is the long-frame flag, and VPOSR bit 15 shows it --
    {
        Harness h;
        h.write(A_VPOSW, 0x8000);
        const uint16_t set = h.read(A_VPOSR) >> 15;
        // MUTATION VPOSW-BIT15-IS-LONG-FRAME oracle red MUTATE=1 writes $8000
        // again where the assertion needs the bit cleared.
        h.write(A_VPOSW, m_lf);
        const uint16_t cleared = h.read(A_VPOSR) >> 15;
        c.note("VPOSR bit 15 reads " + std::to_string(set)
               + " after writing it set and " + std::to_string(cleared)
               + " after writing it clear");
        c.expect(set == 1 && cleared == 0,
                 "VPOSW-BIT15-IS-LONG-FRAME: the top bit written at $02A is "
                 "held and read back at $004, so VPOSR carries more than the "
                 "counter and VPOSW writes more than it [p.6] [claims: "
                 "sol:AA-vposr.c5 opus:AA-vposr.c5 sol:AA-vposw.c5 opus:AA-vposw.c5]");
    }

    // --- 5. BEAMCON0 decides whether the limit registers are consulted ------
    {
        Harness h;
        // Park the beam just below a small limit, then see whether the limit
        // is reached at all. With varbeamen clear the line is the fixed 227
        // CCKs and the small limit is ignored.
        h.write(A_HTOTAL, 0x10);          // the counter wraps when hpos[8:1] == $10
        // MUTATION BEAMCON0-AT-1DC stimulus red MUTATE=1 writes zero to $1DC,
        // so varbeamen stays clear and the short line never happens.
        h.write(A_BEAMCON0, m_varb);
        h.write(A_VHPOSW, 0x0008);        // hpos[8:1] = $08, below the limit
        const int wrapped_with = h.hpos_wraps_after(64);

        Harness g;
        g.write(A_HTOTAL, 0x10);
        g.write(A_BEAMCON0, 0);           // varbeamen clear
        g.write(A_VHPOSW, 0x0008);
        const int wrapped_without = g.hpos_wraps_after(64);

        c.note("with VARBEAMEN the counter wrapped after " + std::to_string(wrapped_with)
               + "; with it clear, " + std::to_string(wrapped_without)
               + " (-1 means it did not wrap within 64 colour clocks)");
        c.expect(wrapped_with >= 0 && wrapped_without < 0,
                 "BEAMCON0-AT-1DC: bit 7 of the word written at $1DC decides "
                 "whether the limit registers are consulted at all, which is "
                 "what the AA list's function column means by (VARBEAMEN=1) "
                 "against those rows [p.10] [claims: sol:AA-beamcon0.c1/c2 "
                 "opus:AA-beamcon0.c1/c2]");
    }

    // --- 6. HTOTAL sets where the horizontal counter wraps ------------------
    {
        Harness h;
        h.write(A_BEAMCON0, VARBEAMEN);
        // MUTATION HTOTAL-AT-1C0 stimulus red MUTATE=1 writes the limit to
        // $1C8, so htotal_reg keeps its reset value and the wrap moves.
        h.write(m_htotal, 0x14);          // the counter wraps when hpos[8:1] == $14
        h.write(A_VHPOSW, 0x0008);
        const int at = h.hpos_wraps_after(64);
        c.note("with the limit written as $14 the counter last read "
               + hex((uint32_t)at) + " before wrapping");
        c.expect(at == 0x14,
                 "HTOTAL-AT-1C0: the word written at $1C0 sets the horizontal "
                 "count the beam wraps at -- the module stores it doubled and "
                 "compares the doubled halves, so the written byte IS the wrap "
                 "point of the counter VHPOSR reports, "
                 "which is the address the AA list prints for HTOTAL [p.10] "
                 "[claims: sol:AA-htotal.c1/c2/c5 opus:AA-htotal.c1/c2/c5]");
    }

    // --- 7. VTOTAL sets where the vertical counter wraps -------------------
    {
        Harness h;
        h.write(A_BEAMCON0, VARBEAMEN);
        h.write(A_HTOTAL, 0x04);          // a four-count line, so frames are short
        // long_frame resets to 1 and puts the wrap one line late; clearing it
        // is what assertion 4 grades, and this is the assertion that needs it.
        h.write(A_VPOSW, 0x0000);
        // MUTATION VTOTAL-AT-1C8 stimulus red MUTATE=1 writes the limit to
        // $1C0, so vtotal_reg keeps its reset value and the frame never ends.
        h.write(m_vtotal, 0x0006);
        h.write(A_VHPOSW, 0x0000);
        int last = -1, seen = -1;
        for (int i = 0; i < 400; ++i) {
            h.cck_tick();
            const int v = (h.read(A_VHPOSR) >> 8) & 0xFF;
            if (last >= 0 && v < last) { seen = last; break; }
            last = v;
        }
        c.note("with vtotal_reg = 6 the vertical counter last read "
               + std::to_string(seen) + " before wrapping");
        c.expect(seen == 6,
                 "VTOTAL-AT-1C8: the word written at $1C8 sets the line the "
                 "vertical counter wraps at, which is the address the AA list "
                 "prints for VTOTAL and one of the rows whose function column "
                 "carries (VARBEAMEN=1) [p.10] [claims: sol:AA-vtotal.c1/c2/c5 "
                 "opus:AA-vtotal.c1/c2/c5]");
    }

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
