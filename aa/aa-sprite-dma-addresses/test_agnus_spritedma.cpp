// aa-sprite-dma-addresses -- agnus_spritedma conformance test
//
// Test purpose: the AA specification's address-ordered register list gives the
//   eight sprites a pointer pair each and four data registers each, and marks
//   every pointer half `+`. This grades those addresses against a module that
//   names almost none of them, and grades FMODE, whose low bits decide how far
//   a sprite pointer advances per fetch.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 8 and 9, the
//   address-ordered register list, and the `+` legend on page 6. The document
//   prints no page numbers of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the vertical start/stop comparison,
//   the sprite scan-double bit, the ECS extended position bits, or the DMA
//   priority handshake beyond using it to select a sprite.
//
// FORTY-NINE ADDRESSES, THREE NAMES. The module declares:
//
//     parameter SPRPTBASE_REG     = 9'h120;
//     parameter SPRPOSCTLBASE_REG = 9'h140;
//     parameter FMODE_REG         = 9'h1fc;
//
//   and decodes everything else from the shape of the address:
//
//     ptsel = reg_address_in[4:2]           which sprite          (:171)
//     reg_address_in[1]                     PTH from PTL       (:183,195)
//     reg_address_in[8:5] == base[8:5]      that it is a pointer  (:183)
//
//   so SPR0PTH $120, SPR0PTL $122, SPR1PTH $124 ... SPR7PTL $13E are sixteen
//   addresses with one name between them, and SPRxPOS/CTL/DATA/DATB are
//   thirty-two more driven OUT rather than decoded in. This is the third
//   package to grade addresses the RTL never names, after the audio one, and
//   the largest.
//
// HOW A SPRITE IS SELECTED, AND WHY THE FIXTURE COMPUTES hpos BACKWARDS.
//   `address_out` carries the pointer of whichever sprite the DMA engine has
//   currently selected, and that selection is a function of the beam:
//
//     sprite[2:0] <= {hpos[5]^hpos[4], ~hpos[4], hpos[3]}  when hpos[2:0]==1
//
//   which inverts: hpos[3] = s[0], hpos[4] = ~s[1], hpos[5] = s[2] ^ ~s[1].
//   The fixture solves that for the sprite it wants and leaves the high bits
//   free to place hpos inside the slot window, because `enable` is only set
//   between $18x and $38x and reg_address_out is $FF outside it. A fixture that
//   picks a plausible hpos instead of solving for one reads sprite 0 every time
//   and calls it a pass.
//
// WHY NOTHING HERE USES ackdma. With it raised the pointer bank latches
//   `newptr` instead of `data_in`, and the sprite index comes from the engine
//   rather than the address -- both of which would overwrite the thing each
//   assertion is about. The auto-increment is real and it is the package's
//   first non-goal; FMODE is graded through the increment it selects, which is
//   the one place ackdma has to be raised, and that assertion says so.

#include "Vagnus_spritedma.h"
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
constexpr uint16_t A_SPRPTBASE = 0x120;   // SPR0PTH; +4 per sprite, +2 for PTL
constexpr uint16_t A_SPRPOSCTL = 0x140;   // SPR0POS; +8 per sprite, +2 per field
constexpr uint16_t A_FMODE     = 0x1FC;

static uint16_t sprpth_addr(int s) { return A_SPRPTBASE + 4 * s; }
static uint16_t sprptl_addr(int s) { return A_SPRPTBASE + 4 * s + 2; }
static uint16_t sprposctl_addr(int s, int field) {
    return A_SPRPOSCTL + 8 * s + 2 * field;   // 0 POS, 1 CTL, 2 DATA, 3 DATB
}

class Harness {
public:
    Harness() : dut(new Vagnus_spritedma) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->aga = 1; dut->ecs = 1;
        dut->ackdma = 0; dut->vbl = 0; dut->vblend = 0;
        dut->hpos = 0; dut->vpos = 0x40;
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

    // Solve sprite[2:0] = {hpos[5]^hpos[4], ~hpos[4], hpos[3]} for hpos, with
    // hpos[2:0]=1 so the latch fires, and high bits chosen to sit inside the
    // sprite slot window ($18x..$38x) so `enable` is set.
    static uint16_t hpos_for(int s) {
        const int b3 = s & 1;
        const int b4 = ((s >> 1) & 1) ? 0 : 1;
        const int b5 = (((s >> 2) & 1) ^ b4) & 1;
        uint16_t h = 0x001 | (b3 << 3) | (b4 << 4) | (b5 << 5);
        // Lift it into the window without disturbing bits 5..0.
        while ((h >> 1) < 0x18 || (h >> 1) > 0x38) h += 0x40;
        return h & 0x1FF;
    }

    // Park the beam so the engine selects sprite s, and let the latch fire.
    void select(int s) {
        dut->hpos = hpos_for(s);
        for (int i = 0; i < 4; ++i) step();
        dut->eval();
    }

    uint32_t pointer(int s) { select(s); return dut->address_out & 0xFFFFF; }
    static uint32_t high5(uint32_t p) { return (p >> 15) & 0x1F; }
    static uint32_t low15(uint32_t p) { return p & 0x7FFF; }

    Vagnus_spritedma* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    const bool mutate = getenv("MUTATE") != nullptr;

    // MUTATION. Every expect() is guarded so MUTATE=1 forces it red, and each
    // guard moves ONE thing: which address is written, which data pattern, or
    // which sprite is selected.
    //
    // NO GUARD ROTATES ALL EIGHT SPRITES. A bank indexed consistently wrong
    // returns what was put in it, which is how a symmetric mutation leaves a
    // symmetric decoder green. Each guard breaks one sprite.
    const int      m_sel    = mutate ? 1 : 0;              // 2: reads sprite 1's pointer
    const uint16_t m_hi_pat = mutate ? 0x001F : 0xFFE0;    // 3: ignored bits stop being ignored
    const uint16_t m_lo_pat = mutate ? 0xFFFF : 0x0001;    // 4: likewise for DB0
    const int      m_spr5   = mutate ? 2 : 5;              // 5: sprite 5 never written
    const uint16_t m_fmode  = mutate ? 0x0000 : 0x000C;    // 6: increment stays 1
    const int      m_live   = mutate ? 1 : 0;              // liveness

    c.record("oracle: AA specification, PDF pp. 8-9, the address-ordered "
             "register list. SPR0PTH $120 through SPR7PTL $13E, every half "
             "marked +; SPR0POS $140 through SPR7DATB $17E; FMODE $1FC "
             "[no page -- the package's own oracle line]");

    c.record("DECLINED at this boundary: the list's chip column gives the "
             "sprite registers to Agnus/Alice and Denise/Lisa in different "
             "rows. Which silicon a block sits in is a fact about the source "
             "tree that no port of this module carries [claims: "
             "sol:AA-spr0pth.c3 sol:AA-spr0ptl.c3 sol:AA-fmode.c3]");
    c.record("DECLINED at this boundary: the list marks the sprite pointers "
             "`~`, a Coprocessor write restriction enforced in agnus_copper "
             "and graded by aa-copper-register-map [claims: sol:AA-spr0pth.c5 "
             "sol:AA-spr0ptl.c5]");

    // --- FIXTURE LIVENESS ---------------------------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red MUTATE=1 reads
        // sprite 1's pointer, which was never written.
        h.write(sprpth_addr(0), 0x0011);
        h.write(sprptl_addr(0), 0x2468);
        const uint32_t got = h.pointer(m_live);
        c.note("sprite 0 pointer written $0011/$2468; hpos "
               + hex(Harness::hpos_for(0)) + " selects it; address_out = "
               + hex(got) + ", high5 = " + hex(Harness::high5(got))
               + ", low15 = " + hex(Harness::low15(got)));
        c.expect(Harness::high5(got) == 0x11
                 && Harness::low15(got) == (0x2468 >> 1),
                 "FIXTURE-DELIVERS-KNOWN-VALUE: a sprite pointer written by "
                 "address and read back through the beam position that selects "
                 "that sprite carries a distinct pattern, so a later assertion "
                 "that reads nothing means the decoder and not the fixture "
                 "[no page -- infrastructure]");
    }

    // --- 2. every sprite's pointer pair is at its own address ---------------
    {
        Harness h;
        for (int s = 0; s < 8; ++s) {
            h.write(sprpth_addr(s), 0x0008 | s);
            h.write(sprptl_addr(s), (uint16_t)((s + 1) << 8));
        }
        bool all = true;
        std::string seen;
        for (int s = 0; s < 8; ++s) {
            // MUTATION SPRPT-ADDRESSES stimulus red MUTATE=1 reads sprite 0
            // through sprite 1's beam position.
            const int look = (s == 0) ? m_sel : s;
            const uint32_t p = h.pointer(look);
            seen += (s ? " " : "") + hex(Harness::high5(p));
            all = all && Harness::high5(p) == (uint32_t)(0x8 | s)
                      && Harness::low15(p) == (uint32_t)(((s + 1) << 8) >> 1);
        }
        c.note("high halves read back per sprite: " + seen
               + "; expected 0x8 0x9 0xA 0xB 0xC 0xD 0xE 0xF");
        c.expect(all,
                 "SPRPT-ADDRESSES: each of the eight sprite pointer pairs is "
                 "written at its own address, $120 and $122 for sprite 0 and "
                 "four apart thereafter, which is what the AA list prints for "
                 "SPR0PTH through SPR7PTL [p.8] "
                 "[claims: sol:AA-spr0pth.c1/c4 sol:AA-spr0ptl.c1/c4 sol:AA-spr1pth.c1/c4 "
                 "sol:AA-spr1ptl.c1/c4 sol:AA-spr2pth.c1/c4 sol:AA-spr2ptl.c1/c4 sol:AA-spr3pth.c1 "
                 "sol:AA-spr3ptl.c1 sol:AA-spr4pth.c1/c4 sol:AA-spr4ptl.c1/c4 sol:AA-spr5pth.c1/c4 "
                 "sol:AA-spr5ptl.c1/c4 sol:AA-spr6pth.c1/c4 sol:AA-spr6ptl.c1/c4 sol:AA-spr7pth.c1/c4 "
                 "sol:AA-spr7ptl.c1/c4 opus:AA-spr0pth.c1/c4 opus:AA-spr0ptl.c1/c4 opus:AA-spr1pth.c1/c4 "
                 "opus:AA-spr1ptl.c1/c4 opus:AA-spr2pth.c1/c4 opus:AA-spr2ptl.c1/c4 opus:AA-spr3pth.c1 "
                 "opus:AA-spr3ptl.c1 opus:AA-spr4pth.c1/c4 opus:AA-spr4ptl.c1/c4 opus:AA-spr5pth.c1/c4 "
                 "opus:AA-spr5ptl.c1/c4 opus:AA-spr6pth.c1/c4 opus:AA-spr6ptl.c1/c4 opus:AA-spr7pth.c1/c4 "
                 "opus:AA-spr7ptl.c1/c4]");
    }

    // --- 3. the HIGH half of a sprite pointer takes DB0-DB4 -----------------
    {
        Harness h;
        // MUTATION SPRPT-HIGH-TAKES-DB0-DB4 oracle red MUTATE=1 writes a word
        // whose low five bits are set where the assertion needs them clear.
        h.write(sprpth_addr(3), m_hi_pat);
        const uint32_t ignored = Harness::high5(h.pointer(3));
        h.write(sprpth_addr(3), 0x001F);
        const uint32_t taken = Harness::high5(h.pointer(3));
        c.note("sprite 3 high half: $FFE0 gives " + hex(ignored)
               + ", $001F gives " + hex(taken));
        c.expect(ignored == 0x00 && taken == 0x1F,
                 "SPRPT-HIGH-TAKES-DB0-DB4: the high half of a sprite pointer "
                 "takes data bits 0 to 4 and no bit above them, which is what "
                 "the p.6 legend states for the high word of a `+` address "
                 "register pair [p.6] [claims: sol:AA-legends.c5 "
                 "opus:AA-legend6-plus.c3 sol:AA-spr3pth.c4 opus:AA-spr3pth.c4]");
    }

    // --- 4. the LOW half takes DB1-DB15, so DB0 is ignored ------------------
    {
        Harness h;
        // MUTATION SPRPT-LOW-TAKES-DB1-DB15 oracle red MUTATE=1 writes $FFFF
        // where the assertion needs a word with only DB0 set.
        h.write(sprptl_addr(3), m_lo_pat);
        const uint32_t ignored = Harness::low15(h.pointer(3));
        h.write(sprptl_addr(3), 0xFFFF);
        const uint32_t taken = Harness::low15(h.pointer(3));
        c.note("sprite 3 low half: $0001 gives " + hex(ignored)
               + ", $FFFF gives " + hex(taken));
        c.expect(ignored == 0x0000 && taken == 0x7FFF,
                 "SPRPT-LOW-TAKES-DB1-DB15: the low half of a sprite pointer "
                 "takes data bits 1 to 15 and ignores bit 0, which is what the "
                 "p.6 legend states for the low word of a `+` address register "
                 "pair [p.6] [claims: sol:AA-legends.c4 opus:AA-legend6-plus.c2 "
                 "sol:AA-spr3ptl.c4 opus:AA-spr3ptl.c4]");
    }

    // --- 5. writing one sprite leaves the others alone ----------------------
    {
        Harness h;
        h.write(sprpth_addr(2), 0x0003);  h.write(sprptl_addr(2), 0x0006);
        // MUTATION SPRPT-IS-PER-SPRITE stimulus red MUTATE=1 writes sprite 5's
        // pattern into sprite 2, so sprite 2 holds the wrong one and sprite 5
        // holds nothing.
        h.write(sprpth_addr(m_spr5), 0x001C); h.write(sprptl_addr(5), 0x7FFE);
        const uint32_t p2 = h.pointer(2);
        const uint32_t p5 = h.pointer(5);
        c.note("sprite 2 reads " + hex(p2) + " (expected "
               + hex((0x03u << 15) | (0x0006 >> 1)) + "), sprite 5 reads "
               + hex(p5) + " (expected " + hex((0x1Cu << 15) | (0x7FFE >> 1)) + ")");
        c.expect(p2 == ((0x03u << 15) | (0x0006 >> 1))
                 && p5 == ((0x1Cu << 15) | (0x7FFE >> 1)),
                 "SPRPT-IS-PER-SPRITE: each sprite's pointer pair holds its own "
                 "value, so the sixteen addresses the AA list prints name "
                 "sixteen registers and not one bank read eight ways [p.8] "
                 "[claims: sol:AA-spr0pth.c2 sol:AA-spr0ptl.c2 sol:AA-spr1pth.c2 sol:AA-spr1ptl.c2 "
                 "sol:AA-spr2pth.c2 sol:AA-spr2ptl.c2 sol:AA-spr3pth.c2 sol:AA-spr3ptl.c2 "
                 "sol:AA-spr4pth.c2 sol:AA-spr4ptl.c2 sol:AA-spr5pth.c2 sol:AA-spr5ptl.c2 "
                 "sol:AA-spr6pth.c2 sol:AA-spr6ptl.c2 sol:AA-spr7pth.c2 sol:AA-spr7ptl.c2 "
                 "opus:AA-spr0pth.c2 opus:AA-spr0ptl.c2 opus:AA-spr1pth.c2 opus:AA-spr1ptl.c2 "
                 "opus:AA-spr2pth.c2 opus:AA-spr2ptl.c2 opus:AA-spr3pth.c2 opus:AA-spr3ptl.c2 "
                 "opus:AA-spr4pth.c2 opus:AA-spr4ptl.c2 opus:AA-spr5pth.c2 opus:AA-spr5ptl.c2 "
                 "opus:AA-spr6pth.c2 opus:AA-spr6ptl.c2 opus:AA-spr7pth.c2 opus:AA-spr7ptl.c2]");
    }

    // --- 6. FMODE decides how far a sprite pointer advances -----------------
    {
        Harness h;
        h.write(sprpth_addr(0), 0x0000);
        h.write(sprptl_addr(0), 0x0100);
        // MUTATION FMODE-AT-1FC stimulus red MUTATE=1 writes zero to $1FC, so
        // fmode[3:2] stays 00 and the pointer advances by one, not four.
        h.write(A_FMODE, m_fmode);           // fmode[3:2] = 11 -> add 4
        h.select(0);
        const uint32_t before = h.dut->address_out & 0xFFFFF;
        h.dut->ackdma = 1;
        h.step();
        h.dut->ackdma = 0;
        h.dut->eval();
        const uint32_t after = h.dut->address_out & 0xFFFFF;
        c.note("with fmode[3:2]=11 the pointer went from " + hex(before)
               + " to " + hex(after) + ", a step of "
               + std::to_string((int)(after - before)));
        c.expect(after - before == 4,
                 "FMODE-AT-1FC: a write at $1FC reaches the fetch-mode "
                 "register, whose low bits set how far a sprite pointer "
                 "advances per fetch, which is the address the AA list prints "
                 "for FMODE [p.9] [claims: sol:AA-fmode.c1 opus:AA-fmode.c1 "
                 "sol:AA-fmode.c2 opus:AA-fmode.c2]");
    }

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
