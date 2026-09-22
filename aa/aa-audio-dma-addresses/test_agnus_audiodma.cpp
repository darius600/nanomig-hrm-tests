// aa-audio-dma-addresses -- agnus_audiodma conformance test
//
// Test purpose: the AA specification's address-ordered register list gives
//   each audio channel a data address and a location-pointer pair, and marks
//   every pointer half `+`. This grades the four data addresses against the
//   address this module drives out per channel, and grades the `+` legend a
//   second time -- at a boundary where the pointer halves are NOT declared as
//   named addresses in the RTL at all.
// Test class: AA_CONFORMANCE
// Primary oracle: Commodore, Functional Specification for the Advanced Amiga
//   Chip Set (AA), 06/07/91, PRELIMINARY. PDF page 7, the address-ordered
//   register list, and the `+` legend on page 6. The document prints no page
//   numbers of its own; the corpus numbering is the PDF's.
// Non-goals: this package does not test the DMA slot allocation, the pointer
//   auto-increment, the restart-from-location behaviour beyond using it as the
//   only route to read a location register back, or anything paula_audio does
//   with the data.
//
// WHY THIS BOUNDARY IS DIFFERENT FROM THE DISK ONE. agnus_diskdma declares
//   DSKPTH and DSKPTL as named parameters. agnus_audiodma declares only the
//   four DATA addresses:
//
//     parameter AUD0DAT_REG = 9'h0AA;   AUD1DAT_REG = 9'h0BA;
//               AUD2DAT_REG = 9'h0CA;   AUD3DAT_REG = 9'h0DA;
//
//   The eight location-pointer halves have no parameter. They are decoded
//   structurally, from the shape of the address:
//
//     audlcena = ~in[8] & in[7] & (in[6]^in[5]) & ~in[3] & ~in[2]      (:71)
//     audlcsel = {~in[5], in[4]}                                       (:74)
//     in[1] picks LCH from LCL                                         (:79,86)
//
//   tools/aa/address_yield.py counts 143 of the document's addresses as having
//   no FULL address in the RTL and says in as many words that this is not the
//   same as the RTL being silent about them. This package is the demonstration:
//   eight addresses with no name in the source, all eight decoded, all eight
//   gradeable.
//
// HOW A LOCATION REGISTER IS READ BACK. There is no read port. The only route
//   is the DMA restart:
//
//     audpt[channel] <= dmas ? audlcout : audptout + 1   when dmal     (:139)
//     address_out    =  audpt[channel]                                 (:132)
//
//   and `dmal`, `dmas` and `channel` are all selected by `hpos` -- one slot per
//   channel, $025, $029, $02D, $031. So reading channel 2's location register
//   means parking the beam in channel 2's slot and asking for a restart. A
//   fixture that holds hpos anywhere else reads a dead module.
//
// WHY EVERY PATTERN IS WRITTEN TO ONE CHANNEL AT A TIME. The pointer bank is
//   `audpt[3:0]` and the location bank is `audlch[3:0]`/`audlcl[3:0]`, indexed
//   by different selectors -- the write index comes from the ADDRESS, the read
//   index from hpos. Writing every channel and then reading one would pass even
//   if the two indices were swapped. Assertion 5 is the one that separates
//   them.

#include "Vagnus_audiodma.h"
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
constexpr uint16_t A_AUDDAT[4] = {0x0AA, 0x0BA, 0x0CA, 0x0DA};
constexpr uint16_t A_AUDLCH[4] = {0x0A0, 0x0B0, 0x0C0, 0x0D0};
constexpr uint16_t A_AUDLCL[4] = {0x0A2, 0x0B2, 0x0C2, 0x0D2};

// The one horizontal position at which each channel owns a DMA slot. Outside
// these four values dmal is hard-wired low and this module does nothing.
constexpr uint16_t SLOT[4] = {0x025, 0x029, 0x02D, 0x031};

class Harness {
public:
    Harness() : dut(new Vagnus_audiodma) {
        dut->clk = 0; dut->clk7_en = 1;
        dut->audio_dmal = 0; dut->audio_dmas = 0;
        dut->hpos = 0; dut->reg_address_in = 0; dut->data_in = 0;
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

    // Park the beam in one channel's slot. `channel` follows hpos[3:2] and the
    // dma request follows hpos exactly, so this one call selects both.
    void park(int ch) {
        dut->hpos = SLOT[ch];
        dut->eval();
    }

    // Ask that channel to restart from its location register, which is the
    // only way this module lets a location register be observed.
    uint32_t restart(int ch) {
        park(ch);
        dut->audio_dmal = 1u << ch;
        dut->audio_dmas = 1u << ch;
        step();
        dut->audio_dmal = 0;
        dut->audio_dmas = 0;
        dut->eval();
        return dut->address_out & 0xFFFFF;
    }

    uint8_t addr_out_for(int ch) {
        park(ch);
        return dut->reg_address_out;
    }

    // address_out is [20:1]: the C value's bit 0 IS address_out[1].
    static uint32_t high5(uint32_t p) { return (p >> 15) & 0x1F; }
    static uint32_t low15(uint32_t p) { return p & 0x7FFF; }

    Vagnus_audiodma* dut;
};

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks c;
    const bool mutate = getenv("MUTATE") != nullptr;

    // MUTATION. Every expect() is guarded so MUTATE=1 forces it red, and each
    // guard moves ONE thing: which address is written, which data pattern, or
    // which channel is parked. None touches a comparison.
    //
    // NO GUARD SWAPS ALL FOUR CHANNELS. Rotating every channel by one writes
    // every register and reads every register, and a bank indexed consistently
    // wrong still returns what was put in it -- the shape that left four of the
    // Copper package's six assertions green. Each guard breaks ONE channel.
    const int      m_park   = mutate ? 1 : 0;           // 2: reads the wrong channel's address
    const uint16_t m_hi_pat = mutate ? 0x001F : 0xFFE0; // 3: the ignored bits stop being ignored
    const uint16_t m_lo_pat = mutate ? 0xFFFF : 0x0001; // 4: likewise for DB0
    const uint16_t m_lch2   = mutate ? A_AUDLCH[0] : A_AUDLCH[2]; // 5: channel 2 never written
    const uint16_t m_live   = mutate ? A_AUDLCH[1] : A_AUDLCH[0]; // liveness

    c.record("oracle: AA specification, PDF p.7, the address-ordered register "
             "list. AUD0DAT $0AA, AUD1DAT $0BA, AUD2DAT $0CA, AUD3DAT $0DA, and "
             "the location pairs AUDxLCH $0A0/$0B0/$0C0/$0D0 with AUDxLCL "
             "$0A2/$0B2/$0C2/$0D2, every pointer half marked + "
             "[no page -- the package's own oracle line]");

    c.record("DECLINED at this boundary: the list's chip column gives the audio "
             "registers to Paula, and this module is agnus_audiodma -- Agnus "
             "fetches the samples Paula plays. Which silicon a block sits in is "
             "a fact about the source tree that no port carries [claims: "
             "sol:AA-aud0dat.c3 sol:AA-aud1dat.c3 sol:AA-aud2dat.c3 "
             "sol:AA-aud3dat.c3 sol:AA-aud0lch.c3 sol:AA-aud0lcl.c3]");
    c.record("DECLINED at this boundary: the list marks the audio registers `&` "
             "-- used by a DMA channel only -- and the location halves `~` as "
             "well. Whether a processor access is refused is decided by the bus "
             "and by agnus_copper's cdang, not here [claims: "
             "sol:AA-aud0dat.c4 sol:AA-aud1dat.c4 sol:AA-aud2dat.c4 "
             "sol:AA-aud3dat.c4 sol:AA-aud0lch.c5 sol:AA-aud0lcl.c5]");

    // --- FIXTURE LIVENESS ---------------------------------------------------
    {
        Harness h;
        // MUTATION FIXTURE-DELIVERS-KNOWN-VALUE stimulus red MUTATE=1 writes
        // channel 0's high word into channel 1, so the restart reads zero.
        h.write(m_live, 0x0013);
        h.write(A_AUDLCL[0], 0x579A);
        const uint32_t got = h.restart(0);
        c.note("channel 0 location $0013/$579A, restarted; address_out = "
               + hex(got) + ", high5 = " + hex(Harness::high5(got))
               + ", low15 = " + hex(Harness::low15(got)));
        c.expect(Harness::high5(got) == 0x13
                 && Harness::low15(got) == (0x579A >> 1),
                 "FIXTURE-DELIVERS-KNOWN-VALUE: a location pair written by "
                 "address and read back through a channel's own DMA restart "
                 "carries a distinct pattern, so a later assertion that reads "
                 "nothing means the decoder and not the fixture [no page -- "
                 "infrastructure]");
    }

    // --- 2. each channel drives its own AUDxDAT address out -----------------
    {
        Harness h;
        bool all = true;
        std::string seen;
        for (int ch = 0; ch < 4; ++ch) {
            // MUTATION AUDDAT-ADDRESS-PER-CHANNEL stimulus red MUTATE=1 parks
            // channel 0's read one slot along, so it answers with AUD1DAT.
            const int look = (ch == 0) ? m_park : ch;
            const uint8_t got = h.addr_out_for(look);
            seen += (ch ? ", " : "") + hex(got);
            all = all && got == ((A_AUDDAT[ch] >> 1) & 0xFF);
        }
        c.note("reg_address_out per channel: " + seen + "; expected "
               + hex(A_AUDDAT[0] >> 1) + ", " + hex(A_AUDDAT[1] >> 1) + ", "
               + hex(A_AUDDAT[2] >> 1) + ", " + hex(A_AUDDAT[3] >> 1));
        c.expect(all,
                 "AUDDAT-ADDRESS-PER-CHANNEL: the address this module drives "
                 "out for a channel's sample fetch is that channel's AUDxDAT, "
                 "$0AA $0BA $0CA $0DA in order, which is what the AA list "
                 "prints [p.7] [claims: opus:AA-aud0dat.c1 opus:AA-aud1dat.c1 opus:AA-aud2dat.c1 opus:AA-aud3dat.c1 sol:AA-aud0dat.c1 sol:AA-aud1dat.c1 sol:AA-aud2dat.c1 sol:AA-aud3dat.c1]");
    }

    // --- 3. the HIGH half of an audio location pair uses DB0-DB4 ------------
    {
        Harness h;
        // MUTATION AUDLC-HIGH-TAKES-DB0-DB4 oracle red MUTATE=1 writes a word
        // whose low five bits are set where the assertion needs them clear.
        h.write(A_AUDLCH[1], m_hi_pat);
        const uint32_t ignored = Harness::high5(h.restart(1));
        h.write(A_AUDLCH[1], 0x001F);
        const uint32_t taken = Harness::high5(h.restart(1));
        c.note("channel 1 high word: $FFE0 gives " + hex(ignored)
               + ", $001F gives " + hex(taken));
        c.expect(ignored == 0x00 && taken == 0x1F,
                 "AUDLC-HIGH-TAKES-DB0-DB4: the high half of an audio location "
                 "pair takes data bits 0 to 4 and no bit above them, which is "
                 "what the p.6 legend states for the high word of a `+` address "
                 "register pair [p.6] [claims: opus:AA-aud1lch.c4 opus:AA-legend6-plus.c3 sol:AA-aud1lch.c4 sol:AA-legends.c5]");
    }

    // --- 4. the LOW half uses DB1-DB15, so DB0 is ignored -------------------
    {
        Harness h;
        // MUTATION AUDLC-LOW-TAKES-DB1-DB15 oracle red MUTATE=1 writes $FFFF
        // where the assertion needs a word with only DB0 set.
        h.write(A_AUDLCL[1], m_lo_pat);
        const uint32_t ignored = Harness::low15(h.restart(1));
        h.write(A_AUDLCL[1], 0xFFFF);
        const uint32_t taken = Harness::low15(h.restart(1));
        c.note("channel 1 low word: $0001 gives " + hex(ignored)
               + ", $FFFF gives " + hex(taken));
        c.expect(ignored == 0x0000 && taken == 0x7FFF,
                 "AUDLC-LOW-TAKES-DB1-DB15: the low half of an audio location "
                 "pair takes data bits 1 to 15 and ignores bit 0, which is what "
                 "the p.6 legend states for the low word of a `+` address "
                 "register pair [p.6] [claims: opus:AA-aud1lcl.c4 opus:AA-legend6-plus.c2 sol:AA-aud1lcl.c4 sol:AA-legends.c4]");
    }

    // --- 5. the four location pairs are four registers, not one -------------
    {
        Harness h;
        h.write(A_AUDLCH[0], 0x0001);  h.write(A_AUDLCL[0], 0x0002);
        // MUTATION AUDLC-IS-PER-CHANNEL stimulus red MUTATE=1 writes channel
        // 2's pattern into channel 0, so channel 2 reads channel 0's value and
        // channel 0 reads the wrong one.
        h.write(m_lch2, 0x0018);       h.write(A_AUDLCL[2], 0x7FFE);
        const uint32_t ch0 = h.restart(0);
        const uint32_t ch2 = h.restart(2);
        c.note("channel 0 reads " + hex(ch0) + " (expected "
               + hex((0x01u << 15) | (0x0002 >> 1)) + "), channel 2 reads "
               + hex(ch2) + " (expected " + hex((0x18u << 15) | (0x7FFE >> 1)) + ")");
        c.expect(ch0 == ((0x01u << 15) | (0x0002 >> 1))
                 && ch2 == ((0x18u << 15) | (0x7FFE >> 1)),
                 "AUDLC-IS-PER-CHANNEL: each channel's location pair holds its "
                 "own value, so the four addresses the AA list prints for the "
                 "four channels name four registers and not one; the write at "
                 "each address is what put the value there, which is the W "
                 "access the list gives them, and the expected value encodes "
                 "the `+` split for channel 0 -- $0001 into the high half "
                 "arrives as 1, $0002 into the low half arrives as 1 [p.7] "
                 "[claims: opus:AA-aud0lch.c1/c2/c4 opus:AA-aud0lcl.c1/c2/c4 opus:AA-aud1lch.c1/c2 opus:AA-aud1lcl.c1/c2 sol:AA-aud0lch.c1/c2/c4 sol:AA-aud0lcl.c1/c2/c4 sol:AA-aud1lch.c1/c2 sol:AA-aud1lcl.c1/c2]");
    }

    // --- What this boundary cannot grade ------------------------------------
    c.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "access column -- W, R or S. A module answers reads at the "
             "addresses it decodes and zero everywhere else, so a write-only row "
             "reads as the same silence an undecoded address reads as, and a "
             "strobe has no read at all. [claims: opus:AA-aud2lch.c2 "
             "opus:AA-aud2lcl.c2 opus:AA-aud3lch.c2 opus:AA-aud3lcl.c2 "
             "sol:AA-aud2lch.c2 sol:AA-aud2lcl.c2 sol:AA-aud3lch.c2 "
             "sol:AA-aud3lcl.c2]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim is the list's "
             "chip column, which says which 1985 silicon a register lives in. No "
             "port of any module in this design carries a chip identity. "
             "[claims: opus:AA-aud2lch.c3 opus:AA-aud2lcl.c3 opus:AA-aud3lch.c3 "
             "opus:AA-aud3lcl.c3 sol:AA-aud2lch.c3 sol:AA-aud2lcl.c3 "
             "sol:AA-aud3lch.c3 sol:AA-aud3lcl.c3]");
    c.record(
             "DECLINED at this boundary (insufficient): The claim names the chip "
             "that implements the register. This module is agnus_audiodma -- "
             "Agnus fetches the samples Paula plays -- and which silicon a block "
             "sits in is a fact about the source tree that no port carries. "
             "[claims: opus:AA-aud0dat.c3 opus:AA-aud0lch.c3 opus:AA-aud0lcl.c3 "
             "opus:AA-aud1dat.c3 opus:AA-aud1lch.c3 opus:AA-aud1lcl.c3 "
             "opus:AA-aud2dat.c3 opus:AA-aud3dat.c3 sol:AA-aud0dat.c3 "
             "sol:AA-aud0lch.c3 sol:AA-aud0lcl.c3 sol:AA-aud1dat.c3 "
             "sol:AA-aud1lch.c3 sol:AA-aud1lcl.c3 sol:AA-aud2dat.c3 "
             "sol:AA-aud3dat.c3]");
    c.record(
             "DECLINED at this boundary (insufficient): The register is decoded "
             "at this boundary, but no assertion in this package writes or reads "
             "it. The package grades the rows its own subject names; this row is "
             "another register of the same module. [claims: "
             "opus:AA-aud2lch.c1/c5 opus:AA-aud2lcl.c1/c5 opus:AA-aud3lch.c1/c5 "
             "opus:AA-aud3lcl.c1/c5 sol:AA-aud2lch.c1/c5 sol:AA-aud2lcl.c1/c5 "
             "sol:AA-aud3lch.c1/c5 sol:AA-aud3lcl.c1/c5]");
    c.record(
             "DECLINED at this boundary (not-hardware): The claim is a legend "
             "statement, a gap between two printed rows, or a reserved line -- "
             "the document describing its own notation and layout. The one "
             "legend mark that IS a hardware restriction, the `~` Coprocessor "
             "limit, is graded by this package's own assertion. [claims: "
             "opus:AA-aud2lch.c4 opus:AA-aud2lcl.c4 opus:AA-aud3lch.c4 "
             "opus:AA-aud3lcl.c4 sol:AA-aud2lch.c4 sol:AA-aud2lcl.c4 "
             "sol:AA-aud3lch.c4 sol:AA-aud3lcl.c4]");
    c.record(
             "DECLINED at this boundary (outside-uut): The `&` mark says the "
             "register is used by a DMA channel only. Whether a processor access "
             "to it is refused is decided by the bus, not at this boundary. "
             "[claims: opus:AA-aud0dat.c4 opus:AA-aud1dat.c4 opus:AA-aud2dat.c4 "
             "opus:AA-aud3dat.c4 sol:AA-aud0dat.c4 sol:AA-aud1dat.c4 "
             "sol:AA-aud2dat.c4 sol:AA-aud3dat.c4]");
    c.record(
             "DECLINED at this boundary (outside-uut): The `~` mark is a "
             "Coprocessor write restriction enforced in agnus_copper's cdang "
             "path and graded by aa-copper-register-map. [claims: "
             "opus:AA-aud0lch.c5 opus:AA-aud0lcl.c5 opus:AA-aud1lch.c5 "
             "opus:AA-aud1lcl.c5 sol:AA-aud0lch.c5 sol:AA-aud0lcl.c5 "
             "sol:AA-aud1lch.c5 sol:AA-aud1lcl.c5]");
    c.record(
             "DECLINED at this boundary (outside-uut): `W` says a write at this "
             "address changes the register. agnus_audiodma never writes AUD0DAT: "
             "it drives that address out so the fetched word lands there, in "
             "Paula. The address is gradeable here and the write is not. "
             "[claims: opus:AA-aud0dat.c2 opus:AA-aud1dat.c2 opus:AA-aud2dat.c2 "
             "opus:AA-aud3dat.c2 sol:AA-aud0dat.c2 sol:AA-aud1dat.c2 "
             "sol:AA-aud2dat.c2 sol:AA-aud3dat.c2]");

    std::cout << "\nAssertions: " << c.passed << " passed, "
              << c.failed << " failed\n";
    return c.failed == 0 ? 0 : 1;
}
