// aa-controller-port-registers -- userio conformance test
//
// Test purpose: the AA specification's address-ordered register list gives the
//   controller ports seven addresses -- two joystick/mouse counters, two pot
//   counter pairs, the pot input word, the pot control word and the counter
//   test write. This grades each address against the one thing at this
//   boundary that only that address does.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF pages 6 and 7, the
//   address-ordered register list. The document prints no page numbers of its
//   own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the quadrature decoding, the CD32 pad
//   protocol, the scroll wheel, the analogue joystick comparison, or which pin
//   carries which button. It grades where seven registers are decoded.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the AA specification independently, and ids such as
//   `sol:AA-joy0dat.c3` and `sol:AA-joy1dat.c3` each name one reader's reading
//   of one sentence. They resolve in the claim files that
//   hrm-testpipeline/acceptance/aa-s5.json lists under `inventories`.
//
// EVERY ASSERTION HERE IS A PAIR OF ADDRESSES, because userio has no register
//   it both writes and reads at one address. The two writable ones reach the
//   readable ones:
//
//     JOYTEST $036 -> mouse counters -> JOY0DAT $00A                (:317,341)
//     POTGO  $034 -> potreg -> pot counters -> POT0DAT $012      (:102,130)
//
//   so a write at one address is graded by a read at another, and an address
//   that decoded nothing would leave the read unchanged.
//
// AND THE LIST SAYS WHAT JOYTEST DOES: `Write to all 4 Joystick-mouse
//   counters at once.` Asserting the opposite -- that a load into port 1
//   leaves port 2 alone -- would go red against an RTL that is following
//   the document. Assertion 3 grades the function column instead.
//
// WHICH LEAVES JOY1DAT'S OWN ADDRESS UNGRADED HERE, and the lot says so rather
//   than the package pretending otherwise. After JOYTEST both ports carry the
//   same word BY DESIGN, so the only stimulus that separates $00A from $00C is
//   motion on one port's quadrature pair -- and the quadrature decoding is this
//   package's first non-goal. Using it as stimulus without grading
//   it would produce a change on the wrong port, from the idle state settling
//   rather than motion. The lot records the claim as insufficient at this
//   boundary and names the fixture that would reach it.
//
//   The negative half still matters where it is available: assertion 5 reads
//   the other pot pair, because a decoder that answered every address would
//   pass a test that only ever read one.
//
// HOW THE POT COUNTERS ARE MADE TO COUNT. They advance on `pot_cnt_en` only
//   while their capture bit is low, and the capture bits are driven by the
//   output-enable pairs in POTGO:
//
//     joy1_pin5 = ~(potreg[9] & ~potreg[8])   potcap[0] <= _mthird & joy1_pin5
//     potcap[1] <= _mright & _djoy1[5] & ~(potreg[11] & ~potreg[10])
//
//   so POTGO with bits 11 and 9 set and 10 and 8 clear pulls both port-1
//   captures low and lets the pair run. A fixture that wrote only the START
//   bit would see nothing move and read it as a dead counter.
//
// WHY _joy1 AND _joy2 ARE HELD HIGH. They are active low. Held high the
//   quadrature decoder sees no motion and the counters hold what the test load
//   put in them, so an assertion reading a counter back is measuring the
//   decoder and not a phantom stick.

#include "Vuserio.h"
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

// Word addresses as the AA address list prints them, read from the corpus.
constexpr uint16_t A_JOY0DAT = 0x00A;
constexpr uint16_t A_JOY1DAT = 0x00C;
constexpr uint16_t A_POT0DAT = 0x012;
constexpr uint16_t A_POT1DAT = 0x014;
constexpr uint16_t A_POTINP  = 0x016;
constexpr uint16_t A_POTGO   = 0x034;
constexpr uint16_t A_JOYTEST = 0x036;

// POTGO: port-1 output enables low-active through bits 11/10 and 9/8, plus the
// START bit that clears the counters.
constexpr uint16_t POTGO_RUN_PORT1 = (1u << 11) | (1u << 9) | 1u;

// The test load only reaches bits 7:2 of each counter byte.
constexpr uint16_t COUNTER_MASK = 0xFCFC;

class Harness {
public:
    Harness() : dut(new Vuserio) {
        dut->clk = 0; dut->clk7_en = 1; dut->reset = 1;
        dut->pot_cnt_en = 0;
        dut->_fire0_dat = 1; dut->_fire1_dat = 1;
        dut->_joy1 = 0xFFFF; dut->_joy2 = 0xFFFF;     // active low: nothing pressed
        dut->joy_ana1 = 0; dut->joy_ana2 = 0;
        // mouse_btn is active HIGH -- `_mthird = ~mouse_btn[2]` (:363) -- so
        // zero is nothing pressed. Seven meant every button held, which pulled
        // the capture bits low and let the pot counters run whatever POTGO
        // said. The mutation gate caught it: the assertion that claimed to
        // measure POTGO's enables stayed green with POTGO set to one.
        dut->mouse_btn = 0; dut->kms_level = 0;
        dut->kbd_mouse_type = 0; dut->kbd_mouse_data = 0;
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

    uint16_t read(uint16_t word_addr) {
        dut->reg_address_in = (word_addr >> 1) & 0xFF;
        dut->eval();
        const uint16_t got = dut->data_out;
        dut->reg_address_in = 0;
        dut->eval();
        return got;
    }

    // One scanline's worth of pot counting.
    void pot_ticks(int n) {
        for (int i = 0; i < n; ++i) {
            dut->pot_cnt_en = 1; step();
            dut->pot_cnt_en = 0; step();
        }
    }

    Vuserio* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    g_mutate = std::getenv("MUTATE");

    // MUTATION. Each guard below tests its own assertion's label and moves
    // ONE thing: the address written, the address read, or the POTGO word
    // that decides whether a counter runs at all. MUTATE=<label> selects
    // exactly one.
    const uint16_t m_clear   = mutating("POTGO-AT-034-CLEARS")
                                    ? (uint16_t)(POTGO_RUN_PORT1 & ~1u)
                                    : POTGO_RUN_PORT1;             // 4: never clears
    const uint16_t m_potgo   = mutating("POT0DAT-AT-012-COUNTS")
                                    ? 1u : POTGO_RUN_PORT1; // 5: never runs
    const uint16_t m_pot0    = mutating("POT1DAT-AT-014-IS-THE-OTHER-PAIR")
                                    ? A_POT1DAT : A_POT0DAT;  // 6: reads the other pair
    const uint16_t m_potinp  = mutating("POTINP-AT-016-IS-ITS-OWN-WORD")
                                    ? A_POT0DAT : A_POTINP;   // 7: reads a counter
    const uint16_t m_live    = mutating("FIXTURE-DELIVERS-KNOWN-VALUE")
                                    ? A_POTGO : A_JOYTEST;  // liveness

    c.record("oracle: AA specification, PDF pp. 6-7, the address-ordered "
             "register list. JOY0DAT $00A, JOY1DAT $00C, POT0DAT $012, POT1DAT "
             "$014, POTINP $016, POTGO $034, JOYTEST $036 "
             "[no page -- the package's own oracle line]");

    c.record("DECLINED at this boundary: the list's chip column gives all seven "
             "to Paula, and marks the four readable ones `~` for the "
             "Coprocessor. Neither is visible at any port of userio, which is "
             "the module Minimig puts the port logic in [claims: "
             "sol:AA-joy0dat.c3 sol:AA-joy1dat.c3 sol:AA-pot0dat.c3 "
             "sol:AA-pot1dat.c3 sol:AA-potinp.c3 sol:AA-potgo.c3 "
             "sol:AA-joytest.c3]");

    // --- FIXTURE LIVENESS ---------------------------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red: selecting this
        // assertion's own id writes the pattern at $034, which loads no
        // counter.
        h.write(m_live, 0xAB48);
        const uint16_t got = h.read(A_JOY0DAT);
        c.note("wrote $AB48 at $036, JOY0DAT reads " + hex(got)
               + ", masked to the loadable bits " + hex(got & COUNTER_MASK));
        c.expect((got & COUNTER_MASK) == (0xAB48 & COUNTER_MASK),
                 "FIXTURE-DELIVERS-KNOWN-VALUE: a word written at one address "
                 "reaches a counter read at another, so a later assertion that "
                 "reads nothing means the decoder and not the fixture "
                 "[no page -- infrastructure]");
    }

    // --- 2. JOYTEST loads the port 1 counter, JOY0DAT reads it --------------
    {
        Harness h;
        // MUTATION JOYTEST-AT-036-AND-JOY0DAT-AT-00A stimulus red: selecting
        // this assertion's own id writes at $034, which is the pot control
        // word and loads no counter.
        const uint16_t joytest2 = mutating("JOYTEST-AT-036-AND-JOY0DAT-AT-00A")
                                       ? A_POTGO : A_JOYTEST;
        h.write(joytest2, 0x5C34);
        const uint16_t got = h.read(A_JOY0DAT);
        c.note("wrote $5C34 at $036, JOY0DAT reads " + hex(got & COUNTER_MASK)
               + ", expected " + hex(0x5C34 & COUNTER_MASK));
        c.expect((got & COUNTER_MASK) == (0x5C34 & COUNTER_MASK),
                 "JOYTEST-AT-036-AND-JOY0DAT-AT-00A: a write at $036 loads the "
                 "port 1 counter and a read at $00A returns it, which is the "
                 "pair of addresses the AA list prints for JOYTEST and JOY0DAT "
                 "[p.6] [claims: sol:AA-joytest.c1/c2 opus:AA-joytest.c1/c2 "
                 "sol:AA-joy0dat.c1/c2 opus:AA-joy0dat.c1/c2]");
    }

    // --- 3. JOYTEST writes ALL FOUR counters, which is what the list says ---
    {
        Harness h;
        // MUTATION JOYTEST-WRITES-ALL-FOUR stimulus red: selecting this
        // assertion's own id writes at $034, so neither port is loaded and
        // the two read alike at zero.
        const uint16_t joytest3 = mutating("JOYTEST-WRITES-ALL-FOUR")
                                       ? A_POTGO : A_JOYTEST;
        h.write(joytest3, 0x5C34);
        const uint16_t port1 = h.read(A_JOY0DAT);
        const uint16_t port2 = h.read(A_JOY1DAT);
        c.note("one write at $036 left $00A reading " + hex(port1)
               + " and $00C reading " + hex(port2));
        c.expect((port1 & COUNTER_MASK) == (0x5C34 & COUNTER_MASK)
                 && (port2 & COUNTER_MASK) == (0x5C34 & COUNTER_MASK),
                 "JOYTEST-WRITES-ALL-FOUR: one write at $036 reaches both port "
                 "counters, which is what the AA list's function column states "
                 "for JOYTEST in as many words [p.6] [claims: "
                 "sol:AA-joytest.c5 opus:AA-joytest.c5]");
    }

    // --- 4. POTGO's START bit clears the pot counters -----------------------
    {
        Harness h;
        h.write(A_POTGO, POTGO_RUN_PORT1);
        h.pot_ticks(12);
        const uint16_t ran = h.read(A_POT0DAT);
        // MUTATION POTGO-AT-034-CLEARS stimulus red: selecting this
        // assertion's own id writes the same word without its START bit, so
        // the counters keep what they had.
        h.write(A_POTGO, m_clear);           // START again: clears
        const uint16_t cleared = h.read(A_POT0DAT);
        c.note("the port 1 pot pair read " + hex(ran)
               + " after twelve counts and " + hex(cleared)
               + " immediately after the next START");
        c.expect(ran != 0 && cleared == 0,
                 "POTGO-AT-034-CLEARS: the word written at $034 zeroes the pot "
                 "counters, which is the start function the AA list prints for "
                 "POTGO [p.7] [claims: sol:AA-potgo.c1/c2 opus:AA-potgo.c1/c2]");
    }

    // --- 5. POT0DAT counts only while POTGO lets it -------------------------
    {
        Harness h;
        // MUTATION POT0DAT-AT-012-COUNTS stimulus red: selecting this
        // assertion's own id writes only the START bit, leaving both capture
        // bits high so nothing counts.
        h.write(A_POTGO, m_potgo);
        h.pot_ticks(12);
        const uint16_t got = h.read(A_POT0DAT);
        c.note("after twelve counts with the port 1 enables set, POT0DAT reads "
               + hex(got));
        c.expect(got != 0,
                 "POT0DAT-AT-012-COUNTS: the pot counter pair read at $012 "
                 "advances only while POTGO's port 1 output enables hold its "
                 "capture bits low, which is the address and the counter the "
                 "AA list prints for POT0DAT [p.6] [claims: sol:AA-pot0dat.c1/c2 "
                 "opus:AA-pot0dat.c1/c2]");
    }

    // --- 6. POT1DAT is the other pair ---------------------------------------
    {
        Harness h;
        h.write(A_POTGO, POTGO_RUN_PORT1);   // port 1 enables only
        h.pot_ticks(12);
        const uint16_t port1 = h.read(A_POT0DAT);
        // MUTATION POT1DAT-AT-014-IS-THE-OTHER-PAIR stimulus red: selecting
        // this assertion's own id reads $012, which is the pair that did
        // count.
        const uint16_t port2 = h.read(m_pot0 == A_POT0DAT ? A_POT1DAT : A_POT0DAT);
        c.note("with only the port 1 enables set, $012 reads " + hex(port1)
               + " and $014 reads " + hex(port2));
        c.expect(port1 != 0 && port2 == 0,
                 "POT1DAT-AT-014-IS-THE-OTHER-PAIR: enabling port 1's counters "
                 "leaves port 2's at zero, so the two addresses the AA list "
                 "prints for POT0DAT and POT1DAT name two pairs and not one "
                 "[p.6] [claims: sol:AA-pot1dat.c1/c2 opus:AA-pot1dat.c1/c2]");
    }

    // --- 7. POTINP is a different word again, and an unrelated address is 0 --
    {
        Harness h;
        h.write(A_POTGO, POTGO_RUN_PORT1);
        h.pot_ticks(12);
        // MUTATION POTINP-AT-016-IS-ITS-OWN-WORD stimulus red: selecting this
        // assertion's own id reads $012, which carries the counters rather
        // than the capture bits.
        const uint16_t inp = h.read(m_potinp);
        const uint16_t counters = h.read(A_POT0DAT);
        const uint16_t elsewhere = h.read(0x0A0);
        c.note("POTINP reads " + hex(inp) + ", POT0DAT reads " + hex(counters)
               + ", an unrelated address reads " + hex(elsewhere));
        c.expect((inp & 0x00FF) == 0 && inp != counters && elsewhere == 0,
                 "POTINP-AT-016-IS-ITS-OWN-WORD: $016 returns the capture bits "
                 "in the high byte with the low byte clear, which is neither "
                 "what $012 returns nor what an address this module does not "
                 "decode returns, and is the address the AA list prints for "
                 "POTINP [p.6] [claims: sol:AA-potinp.c1/c2 "
                 "opus:AA-potinp.c1/c2]");
    }

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
