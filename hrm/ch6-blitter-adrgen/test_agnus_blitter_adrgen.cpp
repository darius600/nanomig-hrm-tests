// Test purpose: Verify agnus_blitter_adrgen's pointer/modulo register banks
//   and the arithmetic unit that services them - four independent BLTxPT
//   pointer registers and four independent BLTxMOD modulo registers, each
//   written as a byte address/offset with its least-significant bit ignored;
//   incptr/decptr moving a pointer by one word-address unit (two bytes);
//   addmod/submod adding or subtracting a sign-extended modulo; the register
//   offsets BLTxPTH/BLTxPTL/ BLTxMOD are written at; and the pointer's value
//   after a simulated blit row.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 170, 172, 173, 184, 200, 270, 287, 289 and 309.
// Non-goals: line-mode use of sign_out (Bresenham accumulator sign) - no
//   claim in this package's inventory exercises it; PKG-BLT-LINE's
//   territory. The "pointing to chip memory" half of Appendix B's "+" mark
//   (p.287, p.289) - address_out carries no memory-region attribute, so only
//   the "pair, even address" half is asserted (each covering claim's
//   bundled_observable_half field says the same). The original chip set's
//   3-bit/18-bit BLTxPTH form (p.270 "high 3 bits", 6-appa-registers.c110,
//   ch6-appa-bltxpt.c2, .c6) - this module has no `ecs` input at all (see
//   its port list, agnus_blitter_adrgen.v:4-20), so the OCS alternative
//   cannot be separately driven here; ECS-FIVE-BIT-PTH below measures the
//   behaviour this design actually implements (unconditionally 5 bits) and,
//   by that same measurement, shows the 3-bit alternative is not what is
//   present - the same reporting pattern hrm/ch6-blitter-status used for
//   the pre-Fat-Agnus busy timing. Register "write-only" wording
//   (ch6-appa-bltxmod.c2, 6-appa-registers.c105, .c108) - this module
//   exposes no host-read datapath for any register at all (no
//   reg_address_out, no data-out-to-bus port), so nothing here could show a
//   register behaving otherwise; not a distinguishing behaviour at this
//   boundary.
// Claim ids: ids such as `6-dma-channels.c14` and `6-dma-channels.c15` name
//   one sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch6.json lists under `inventories`.
//
// Supersedes hrm/agnus_blitter_modulo and hrm/agnus_blitter_ptr, per the build
// register (work/2026-09-08_fpga-090_build_register.md). Both compile the same
// agnus_blitter_adrgen.v this package does, but neither carries a page number,
// a Test class:, or a Primary oracle: (docs/evidence/blitter-prior-art.md lists both as
// "nowhere" cited) - so neither counts as HRM_CONFORMANCE evidence even though
// both currently pass. Neither is deleted here: both stay in the tree, unchanged
// and still green, until this package is confirmed green in their place.
//
// docs/evidence/blitter-prior-art.md names hrm/agnus_blitter_modulo's uncited claim by
// name: a modulo of $000A advances the word pointer by 5, and $FFF6 moves it
// back by 5, with "where the manual says so is not recorded." MOD-ARITH-000A
// below is that same case, now cited: p.172 "The modulo values are in bytes,
// not words" and "the least significant bit is ignored" and "value is
// sign-extended to the full width of the address pointer", corroborated by
// p.200's "The least significant bit of all pointers and modulos is ... ignored"
// and by Appendix A p.270's BLTxMOD entry.
//
// 6-dma-channels.c58 (p.172, "incremented by two (two bytes, or one word)") was
// discarded by Prompt 4's classification and overturned by Prompt 5's attack
// stage back into this package's pool (see its record's `evidence` field in the
// classified inventory). PTR-INC-ONE-WORD below asserts it: address_out is a
// WORD address (bits [20:1] of agnus_blitter_adrgen.v:19 - one unit is two
// bytes, because bit 0 of the byte address is never represented at all), and
// `t_newptr = bltptr_out + 20'h1` (agnus_blitter_adrgen.v:106) adds exactly one
// such unit. "+1 word-address unit" and "+2 bytes" are the same physical
// increment stated in two different units, not two different claims - the
// assertion below measures the +1 and the assertion message spells out why
// that agrees with the manual's "+2 bytes" wording.
//
// 6-appa-registers.c109 (p.270, "BLTxPTL    052      W       A     Blitter
// pointer to x (low 15 bits)") and ch6-appa-bltxpt.c5 (p.270, the same
// register-table row, restated as "BLTxPTL holds the low 15 bits of the
// blitter pointer") were routed to this package in the 2026-09-25 unit
// backlog: BLTxPTL, the low word of a channel's pointer register, is a
// 15-bit field, not a 16-bit one - the register table's parenthetical beside
// BLTxPTL's own entry, the same table whose BLTxPTH row carries the "(high 5
// bits)"/"(high 3 bits)" wording ECS-FIVE-BIT-PTH already measures for the
// high word above. Not yet asserted at this point in the file.
//
// CLAIM COVERAGE, for claims of the 92 with no assertion of their own:
//   6-dma-channels.c56 (p.172, "16-bit modulo register called BLTxMOD") is a
//     width description, not independently falsifiable on its own - its
//     testable content (that of the 16 bits a bus write carries, 15 are
//     retained and the 16th, bit 0, is ignored) is exactly what
//     MOD-BYTE-LSB-IGNORED measures.
//   6-appa-registers.c112, .c113, ch6-appa-bltxpt.c7 (p.270, "the pointer
//     addresses blitter DMA data" / "must be preloaded with the starting
//     address") describe what a pointer register is FOR (software preloads it
//     before a blit) rather than a distinct RTL behaviour - PTR-TWO-WORD-
//     COMPOSITION already demonstrates that whatever byte address is written
//     is exactly what the pointer bank holds and address_out reports.
//
// FIXTURE NOTE, learned from the agnus_blitter package built immediately
// before this one (hrm/ch6-blitter-status): this module is sequential (the
// pointer and modulo banks are `reg`, updated `@(posedge clk)`), so a value
// this fixture sets and a value it samples are never in the same instant
// unless the read is a genuinely combinational probe. address_out IS such a
// probe here (agnus_blitter_adrgen.v:72,80,82,84 are all continuous
// assignments off the register arrays), so Harness::read_ptr never clocks -
// but PTR-WRITE-READBACK-BASELINE below still asserts the simple case (write a
// pointer, read it back unchanged) as its own check, before anything that
// depends on arithmetic, exactly per that package's warning: without it, an
// arithmetic assertion could be green because nothing moved at all.
//
// Every mutation below was actually applied (edit -> rebuild -> run -> revert
// -> rebuild-clean) and the FAIL lines quoted are what the run printed, not
// predictions.
//
// MUTATION PTR-WRITE-READBACK-BASELINE stimulus red: read_ptr(CHA) changed to read_ptr(CHB) (a
//   channel that was never written) -> PTR-WRITE-READBACK-BASELINE FAIL, got 0
//   instead of 0x1000; every other assertion PASS.
// MUTATION PTR-TWO-WORD-COMPOSITION stimulus red: the second bus_write in the
//   block changed from BLTAPTL to BLTAPTH (so the "low word alone" step
//   re-writes the high word instead) -> PTR-TWO-WORD-COMPOSITION FAIL; every
//   other assertion PASS.
// MUTATION PTR-BYTE-LSB-IGNORED stimulus red: the odd stimulus BLTAPTL=0x2001
//   changed to the even 0x2002 (a genuinely different word address) ->
//   PTR-BYTE-LSB-IGNORED FAIL, the two writes no longer produce the same
//   address_out; every other assertion PASS.
// MUTATION MOD-BYTE-LSB-IGNORED stimulus red: the odd stimulus BLTAMOD=0x0009
//   changed to the even 0x000A (a genuinely different modulo) ->
//   MOD-BYTE-LSB-IGNORED FAIL; every other assertion PASS.
// MUTATION PTR-INC-ONE-WORD stimulus red: apply()'s incptr argument changed to
//   decptr in the PTR-INC-ONE-WORD block (asks the module to decrement instead
//   of increment) -> PTR-INC-ONE-WORD FAIL, got 0x0FFF instead of 0x1001;
//   every other assertion PASS.
// MUTATION PTR-DEC-ONE-WORD stimulus red: apply()'s decptr argument changed to
//   incptr in the PTR-DEC-ONE-WORD block -> PTR-DEC-ONE-WORD FAIL, got 0x1001
//   instead of 0x0FFF; every other assertion PASS.
// MUTATION MOD-ADD-SIGN-EXTENDED stimulus red: apply()'s addmod argument
//   changed to submod in the MOD-ADD-SIGN-EXTENDED block (asks the module to
//   subtract the modulo instead of add it) -> MOD-ADD-SIGN-EXTENDED FAIL, got
//   0x1003 instead of 0x0FFD; every other assertion PASS.
// MUTATION MOD-SUB-SIGN-EXTENDED stimulus red: apply()'s submod argument
//   changed to addmod in the MOD-SUB-SIGN-EXTENDED block -> MOD-SUB-SIGN-
//   EXTENDED FAIL, got 0x1005 instead of 0x0FFB; every other assertion PASS.
// MUTATION MOD-ARITH-000A-PLUS5-WORDS stimulus red: the second stimulus in the
//   block, BLTAMOD=0xFFF6, changed to 0xFFF7 (still negative, but now odd - an
//   LSB-ignored value equal to 0xFFF6 after the drop) -> stayed PASS, which is
//   the CORRECT shape (0xFFF7's dropped-bit-0 form is identical to 0xFFF6's),
//   so this was replaced with 0xFFF4 (a genuinely different negative modulo,
//   -6 words) -> MOD-ARITH-000A-PLUS5-WORDS FAIL, got 0x0FFA instead of the
//   expected 0x0FFB; every other assertion PASS.
// MUTATION MOD-NEGATIVE-REPEATS-ROW stimulus red: the three incptr applies
//   (simulating a 3-word row) reduced to two, while the modulo stimulus stayed
//   the negative equivalent of 3 words (-6 bytes) -> MOD-NEGATIVE-REPEATS-ROW
//   FAIL, the pointer landed one word short of the row start (0x0FFF instead
//   of 0x1000); every other assertion PASS.
// MUTATION FOUR-INDEPENDENT-PTR-BANKS stimulus red: channel D's bus writes
//   (BLTDPTH/BLTDPTL) changed to reuse channel C's offsets (BLTCPTH/BLTCPTL),
//   so what was meant as channel D's pointer silently overwrites channel C's
//   -> FOUR-INDEPENDENT-PTR-BANKS FAIL, channel C's read-back no longer
//   matched its own stimulus; every other assertion PASS.
// MUTATION FOUR-INDEPENDENT-MOD-BANKS stimulus red: channel D's modulo bus
//   write (BLTDMOD) changed to BLTBMOD, so channel D's addmod silently applies
//   channel B's modulo instead of its own -> FOUR-INDEPENDENT-MOD-BANKS FAIL,
//   channel D's result no longer matched its own stimulus; every other
//   assertion PASS.
// MUTATION POINTER-VALUE-AFTER-BLIT stimulus red: the row's word-transfer
//   count changed from three incptr applies to two -> POINTER-VALUE-AFTER-BLIT
//   FAIL, got 0x1005 instead of the expected 0x1006; every other assertion
//   PASS.
// MUTATION FIGURE-6-2-PTR-STIMULUS stimulus red: the figure's BLTxPTR
//   stimulus, byte address 0x0214 (532), changed to 0x0216 (534, a value the
//   figure does not state) -> FIGURE-6-2-PTR-STIMULUS FAIL, got 0x010B instead
//   of the expected 0x010A; every other assertion PASS.
// MUTATION FIGURE-6-2-MOD-STIMULUS stimulus red: the figure's BLTxMOD
//   stimulus, 0x001E (30 bytes), changed to 0x0020 (32 bytes, a value the
//   figure does not state) -> FIGURE-6-2-MOD-STIMULUS FAIL, got 0x0010 instead
//   of the expected 0x000F; every other assertion PASS.
// MUTATION ECS-FIVE-BIT-PTH stimulus red: the stimulus BLTAPTH=0x001F (all
//   five bits set) changed to 0x000F (only four bits set, well short of the
//   ECS width) -> ECS-FIVE-BIT-PTH FAIL, got 0x78000 instead of the expected
//   0xF8000; every
//   other assertion PASS.

#include <cstdint>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vagnus_blitter_adrgen.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

// Register offsets, taken from agnus_blitter_adrgen.v's own parameters
// (lines 23-34, 9-bit byte-address form) and converted to the 8-bit
// word-register-index form reg_address_in actually carries (bit 0 of a
// register's byte address is always 0, so >>1 loses nothing). BLTAPTH/
// BLTAPTL/BLTAMOD's numeric values ($050/$052/$064) are exactly what
// Appendix A prints for the generic "x=A" register-table entry (p.270).
constexpr uint16_t BLTCPTH = to_reg(0x048);
constexpr uint16_t BLTCPTL = to_reg(0x04a);
constexpr uint16_t BLTBPTH = to_reg(0x04c);
constexpr uint16_t BLTBPTL = to_reg(0x04e);
constexpr uint16_t BLTAPTH = to_reg(0x050);
constexpr uint16_t BLTAPTL = to_reg(0x052);
constexpr uint16_t BLTDPTH = to_reg(0x054);
constexpr uint16_t BLTDPTL = to_reg(0x056);
constexpr uint16_t BLTCMOD = to_reg(0x060);
constexpr uint16_t BLTBMOD = to_reg(0x062);
constexpr uint16_t BLTAMOD = to_reg(0x064);
constexpr uint16_t BLTDMOD = to_reg(0x066);
constexpr uint16_t IDLE_REG = 0xFF;  // no register above matches this

// Channel select codes, per agnus_blitter_adrgen.v:37-40.
constexpr uint8_t CHA = 0x2;  // 2'b10
constexpr uint8_t CHB = 0x1;  // 2'b01
constexpr uint8_t CHC = 0x0;  // 2'b00
constexpr uint8_t CHD = 0x3;  // 2'b11

class Harness {
public:
    Harness() : dut(new Vagnus_blitter_adrgen) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 0;  // reset is a port but nothing in the module body reads
                          // it (no reset branch on bltpth/bltptl/bltmod) - held
                          // low throughout, matching what the RTL actually uses.
        dut->ptrsel = 0;
        dut->modsel = 0;
        dut->enaptr = 0;
        dut->incptr = 0;
        dut->decptr = 0;
        dut->addmod = 0;
        dut->submod = 0;
        dut->data_in = 0;
        dut->reg_address_in = IDLE_REG;
        dut->eval();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // Host bus write: data lands on data_in at reg_address_in==reg_addr, for
    // one clock. enaptr stays 0, so the write is latched through the
    // reg_address_in comparison path (agnus_blitter_adrgen.v:66-70, 74-78,
    // 92-96), the same path the real Agnus register bus uses - not through the
    // arithmetic (enaptr) path.
    void bus_write(uint16_t reg_addr, uint16_t data) {
        dut->enaptr = 0;
        dut->reg_address_in = reg_addr;
        dut->data_in = data;
        tick();
        dut->reg_address_in = IDLE_REG;
        dut->data_in = 0;
    }

    // Applies one arithmetic cycle to the pointer bank `sel` (also used to
    // select the modulo bank when addmod/submod is asserted, matching how the
    // real blitter always pairs a channel's own pointer with its own modulo):
    // asserts enaptr/ptrsel/modsel plus whichever of incptr/decptr/addmod/
    // submod is requested, clocks it once, then de-asserts every control bit
    // again. The bank is a `reg` updated `@(posedge clk)` - the new value
    // exists only from the tick after the one that requested it.
    void apply(uint8_t sel, bool incptr, bool decptr, bool addmod, bool submod) {
        dut->enaptr = 1;
        dut->ptrsel = sel;
        dut->modsel = sel;
        dut->incptr = incptr ? 1 : 0;
        dut->decptr = decptr ? 1 : 0;
        dut->addmod = addmod ? 1 : 0;
        dut->submod = submod ? 1 : 0;
        tick();
        dut->incptr = 0;
        dut->decptr = 0;
        dut->addmod = 0;
        dut->submod = 0;
    }

    // Reads the pointer bank selected by `sel` WITHOUT touching the clock:
    // address_out is a continuous assignment off bltptr_out, itself a
    // continuous read of the register arrays at bltptr_sel
    // (agnus_blitter_adrgen.v:72,80,82,84) - a pure combinational probe. Needs
    // enaptr=1 (so bltptr_sel==ptrsel rather than being derived from
    // reg_address_in) and every arithmetic control bit held at 0, so the probe
    // itself can never mutate what it reads.
    uint32_t read_ptr(uint8_t sel) {
        dut->enaptr = 1;
        dut->ptrsel = sel;
        dut->incptr = 0;
        dut->decptr = 0;
        dut->addmod = 0;
        dut->submod = 0;
        dut->eval();
        return dut->address_out;
    }

private:
    Vagnus_blitter_adrgen* dut;

    void tick() {
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    int passed = 0;
    int failed = 0;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- PTR-WRITE-READBACK-BASELINE [fixture check] ----------------
    //
    // Before any assertion about pointer/modulo ARITHMETIC can mean anything,
    // the fixture has to prove it can do the simple thing: write a pointer and
    // read it back unchanged. Without this, an arithmetic assertion could be
    // green because nothing moved at all - the exact trap the agnus_blitter_
    // status package's header warns the next fixture about.
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x2000);
        const uint32_t got = h.read_ptr(CHA);
        checks.expect(got == 0x1000,
            "[fixture check] PTR-WRITE-READBACK-BASELINE: writing BLTAPTH=$0000, "
            "BLTAPTL=$2000 and reading channel A's pointer back (no inc/dec/mod "
            "applied) gives word address $1000 unchanged - proves the fixture can "
            "write a pointer and observe it without disturbing it, which every "
            "arithmetic assertion below depends on");
    }

    // ---------------- PTR-TWO-WORD-COMPOSITION (p.170, p.270) ----------------
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x2000);  // word addr $1000
        h.bus_write(BLTAPTH, 0x0007);  // touch ONLY the high word
        const uint32_t after_high = h.read_ptr(CHA);
        h.bus_write(BLTAPTL, 0x4000);  // touch ONLY the low word
        const uint32_t after_low = h.read_ptr(CHA);

        checks.expect(after_high == 0x39000 && after_low == 0x3A000,
            "PTR-TWO-WORD-COMPOSITION: writing BLTAPTH alone changes only "
            "address_out's high bits (low bits from the earlier BLTAPTL write "
            "survive unchanged: $39000), and a later BLTAPTL write alone then "
            "changes only the low bits (high bits from the BLTAPTH write survive: "
            "$3A000) - the pointer is composed of two independently-writable "
            "words, BLTxPTH and BLTxPTL, named and ordered exactly so - which is "
            "also the 'Address register pair' half of Appendix B's + mark, one "
            "proposition carried by ten claims, one per register per reader "
            "(ch6-appb-bltapth.dma, ch6-appb-bltaptl.dma, ch6-appb-bltbpth.dma, "
            "ch6-appb-bltbptl.dma, ch6-appb-bltcpth.dma, ch6-appb-bltcptl.dma, "
            "ch6-appb-bltdpth.dma, ch6-appb-bltdptl.dma, ch6-appb-legend.plus, "
            "pp.287 and 289; their quote field is a reader's scan reference, not "
            "manual text, so the wording above is taken from the restatement and "
            "the page image, and the 'even address' half is PTR-BYTE-LSB-IGNORED) "
            "(6-dma-channels.c14, 6-dma-channels.c15, 6-dma-channels.c17, "
            "ch6-dma.c11, ch6-dma.c12, ch6-dma.c15, p.170; the offsets exercised "
            "here, $050 and $052 for channel A, match Appendix A's generic "
            "BLTxPTH/BLTxPTL entries: 6-appa-registers.c104, "
            "6-appa-registers.c105, 6-appa-registers.c107, "
            "6-appa-registers.c108, p.270; what a pointer register is preloaded "
            "with and addresses is the same byte address held and reported "
            "here: 6-appa-registers.c112, 6-appa-registers.c113, "
            "ch6-appa-bltxpt.c7, p.270)");
    }

    // ---------------- PTR-BYTE-LSB-IGNORED (p.170, p.200) ----------------
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x2000);  // even byte address
        const uint32_t even = h.read_ptr(CHA);
        h.bus_write(BLTAPTL, 0x2001);  // odd byte address, one more than above
        const uint32_t odd = h.read_ptr(CHA);

        checks.expect(even == 0x1000 && odd == 0x1000,
            "PTR-BYTE-LSB-IGNORED: byte addresses $2000 and $2001 (differing "
            "only in the bit the blitter cannot address, since it works only on "
            "words) both read back as the identical word address $1000 - the "
            "pointer register is written as a byte address with its "
            "least-significant bit ignored (6-dma-channels.c19, "
            "6-dma-channels.c20, ch6-dma.c17, ch6-dma.c18, p.170; "
            "6-blitter-key-points.c3, 6-blitter-key-points.c6, ch6-keypoints.c4, "
            "ch6-keypoints.c7, p.200)");
    }

    // ---------------- MOD-BYTE-LSB-IGNORED (p.172, p.200) ----------------
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x2000);  // ptr = $1000
        h.bus_write(BLTAMOD, 0x0008);  // even, 8 bytes = 4 words
        h.apply(CHA, false, false, /*addmod=*/true, false);
        const uint32_t even = h.read_ptr(CHA);

        h.bus_write(BLTAPTL, 0x2000);  // ptr back to $1000
        h.bus_write(BLTAMOD, 0x0009);  // odd: the low bit is the one the blitter ignores
        h.apply(CHA, false, false, /*addmod=*/true, false);
        const uint32_t odd = h.read_ptr(CHA);

        checks.expect(even == 0x1004 && odd == 0x1004,
            "MOD-BYTE-LSB-IGNORED: modulo values $0008 and $0009 (differing only "
            "in the bit the blitter cannot address) both advance the pointer by "
            "the identical 4 words ($1004) - the modulo register is read as a "
            "byte offset with its least-significant bit ignored "
            "(6-dma-channels.c61, 6-dma-channels.c62, ch6-dma.c63, ch6-dma.c64, "
            "p.172; 6-blitter-key-points.c2, 6-blitter-key-points.c7, "
            "ch6-keypoints.c3, ch6-keypoints.c8, p.200; also covers "
            "6-dma-channels.c56's '16-bit modulo register' - of the 16 bits a "
            "bus write carries, this measures that 15 are retained and the 16th "
            "is ignored)");
    }

    // ---------------- PTR-INC-ONE-WORD (p.172; overturned c58) ----------------
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x2000);  // ptr = $1000
        h.apply(CHA, /*incptr=*/true, false, false, false);
        const uint32_t got = h.read_ptr(CHA);

        checks.expect(got == 0x1001,
            "PTR-INC-ONE-WORD: incptr advances channel A's pointer from word "
            "address $1000 to $1001, i.e. by one word-address unit - "
            "address_out is a WORD address (agnus_blitter_adrgen.v:19, bits "
            "[20:1]; bit 0 of the byte address is never represented at all), so "
            "one unit here IS two bytes, and 'address pointer register is "
            "incremented by two (two bytes, or one word)' (6-dma-channels.c58, "
            "overturned from a discard by the attack stage - see header) and "
            "'advanced ... for every fetched or written word' "
            "(6-dma-channels.c57, ch6-dma.c60, p.172) describe this exact "
            "physical increment, just in two different units, not two "
            "different claims");
    }

    // ---------------- PTR-DEC-ONE-WORD (entailed, no direct quote) ----------------
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x2000);  // ptr = $1000
        h.apply(CHA, false, /*decptr=*/true, false, false);
        const uint32_t got = h.read_ptr(CHA);

        checks.expect(got == 0x0FFF,
            "PTR-DEC-ONE-WORD [entailed, not a verbatim quote]: decptr moves "
            "channel A's pointer from $1000 to $0FFF, one word-address unit the "
            "other way. No claim in this package's inventory names descending-"
            "mode decrement directly (6-dma-channels.c57, 6-dma-channels.c58 "
            "and ch6-dma.c60 describe only the ascending +2-byte case asserted by "
            "PTR-INC-ONE-WORD above); this is entailed by "
            "agnus_blitter_adrgen.v:107-108's subtractor, the mirror of the "
            "adder PTR-INC-ONE-WORD measures on line 106, and asserted as its "
            "own check because a mirror in the source is not itself a "
            "measurement");
    }

    // ---------------- MOD-ADD-SIGN-EXTENDED (p.172, p.270) ----------------
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x2000);  // ptr = $1000
        h.bus_write(BLTAMOD, 0xFFFA);  // -6 bytes = -3 words
        h.apply(CHA, false, false, /*addmod=*/true, false);
        const uint32_t got = h.read_ptr(CHA);

        checks.expect(got == 0x0FFD,
            "MOD-ADD-SIGN-EXTENDED: addmod with modulo $FFFA (-6 bytes) moves "
            "the pointer from $1000 to $0FFD, i.e. -3 words - the negative "
            "modulo's sign bit is correctly extended across the pointer's full "
            "width rather than the pointer merely growing ('value is "
            "sign-extended to the full width of the address pointer', "
            "6-dma-channels.c63, ch6-dma.c65, p.172; 'the signed 16-bit modulo "
            "value ... is added to' / 'is automatically added to the address', "
            "6-dma-channels.c59, ch6-dma.c61, 6-appa-registers.c91, "
            "6-appa-registers.c92, ch6-appa-bltxmod.c5, ch6-appa-bltxmod.c6, "
            "p.172/p.270; register offset $064 for channel A matches "
            "6-appa-registers.c87, 6-appa-registers.c88, ch6-appa-bltxmod.c1, "
            "ch6-appa-bltxmod.c2, p.270)");
    }

    // ---------------- MOD-SUB-SIGN-EXTENDED (entailed, no direct quote) ----------------
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x2000);  // ptr = $1000
        h.bus_write(BLTAMOD, 0x000A);  // +10 bytes = +5 words
        h.apply(CHA, false, false, false, /*submod=*/true);
        const uint32_t got = h.read_ptr(CHA);

        checks.expect(got == 0x0FFB,
            "MOD-SUB-SIGN-EXTENDED [entailed, not a verbatim quote]: submod "
            "with modulo $000A (+10 bytes = +5 words) moves the pointer from "
            "$1000 to $0FFB, i.e. -5 words - subtracting a positive modulo. No "
            "claim in this package's inventory names submod directly; this is "
            "entailed by agnus_blitter_adrgen.v:116-117's subtractor, which "
            "sign-extends bltmod_out identically to the adder "
            "MOD-ADD-SIGN-EXTENDED measures on line 115, and asserted as its "
            "own check for the same reason PTR-DEC-ONE-WORD is");
    }

    // ---------------- MOD-ARITH-000A-PLUS5-WORDS (prior-art citation fix) ----------------
    //
    // docs/evidence/blitter-prior-art.md names this exact case as hrm/agnus_blitter_
    // modulo's one interesting uncited claim: modulo $000A advances the word
    // pointer by 5, and $FFF6 moves it back by 5, "where the manual says so is
    // not recorded." It is recorded here: $000A = 10 bytes = 5 words (p.172
    // "modulo values are in bytes, not words"; p.200 "least significant bit of
    // all pointers and modulos is ... ignored" - irrelevant to this even value,
    // but the same byte-to-word rule); $FFF6 is -10 in 16-bit two's complement =
    // -10 bytes = -5 words once sign-extended (p.172 "sign-extended to the full
    // width of the address pointer"; Appendix A p.270's BLTxMOD entry gives the
    // register this is written to).
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x2000);  // ptr = $1000
        h.bus_write(BLTAMOD, 0x000A);
        h.apply(CHA, false, false, /*addmod=*/true, false);
        const uint32_t pos = h.read_ptr(CHA);

        h.bus_write(BLTAPTL, 0x2000);  // ptr back to $1000
        h.bus_write(BLTAMOD, 0xFFF6);
        h.apply(CHA, false, false, /*addmod=*/true, false);
        const uint32_t neg = h.read_ptr(CHA);

        checks.expect(pos == 0x1005 && neg == 0x0FFB,
            "MOD-ARITH-000A-PLUS5-WORDS: modulo $000A (10 bytes) advances the "
            "pointer by 5 words to $1005, and modulo $FFF6 (-10 bytes) moves it "
            "back by 5 words to $0FFB - the prior art's uncited case "
            "(docs/evidence/blitter-prior-art.md), now cited against p.172 ('The modulo "
            "values are in bytes, not words', 6-dma-channels.c61, ch6-dma.c63; "
            "'the least significant bit is ignored', 6-dma-channels.c62, "
            "ch6-dma.c64; 'value is sign-extended to the full width of the "
            "address pointer', 6-dma-channels.c63, ch6-dma.c65), p.200 "
            "('pixels. The least significant bit of all pointers and modulos "
            "is ... ignored', 6-blitter-key-points.c2, 6-blitter-key-points.c7, "
            "ch6-keypoints.c3, ch6-keypoints.c8) and Appendix A p.270 (BLTxMOD "
            "entry, 6-appa-registers.c87, 6-appa-registers.c88, "
            "6-appa-registers.c91, 6-appa-registers.c92, ch6-appa-bltxmod.c1, "
            "ch6-appa-bltxmod.c2, ch6-appa-bltxmod.c5, ch6-appa-bltxmod.c6)");
    }

    // ---------------- MOD-NEGATIVE-REPEATS-ROW (p.172, p.184) ----------------
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x2000);  // ptr = $1000 = row start
        h.apply(CHA, /*incptr=*/true, false, false, false);  // word 1 of 3
        h.apply(CHA, /*incptr=*/true, false, false, false);  // word 2 of 3
        h.apply(CHA, /*incptr=*/true, false, false, false);  // word 3 of 3
        const uint32_t row_end = h.read_ptr(CHA);

        h.bus_write(BLTAMOD, 0xFFFA);  // -(3 words * 2 bytes) = -6 bytes
        h.apply(CHA, false, false, /*addmod=*/true, false);
        const uint32_t after_mod = h.read_ptr(CHA);

        checks.expect(row_end == 0x1003 && after_mod == 0x1000,
            "MOD-NEGATIVE-REPEATS-ROW: after three incptr applies simulating a "
            "3-word row (pointer $1000 -> $1003), a modulo of -6 bytes (the "
            "negative of the row's own width) returns the pointer to $1000, "
            "the row's start - 'described as repeating a row by setting the "
            "modulo to the negative of the width' (6-dma-channels.c64, "
            "ch6-dma.c66, p.172), the same mechanism p.184 states for a mask: "
            "'the mask will be repeatedly fetched for each row' when its A "
            "modulo is set to the negative of the mask's width (ch6-copy.c17)");
    }

    // ---------------- FOUR-INDEPENDENT-PTR-BANKS (p.170, p.287, p.289) ----------------
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x00); h.bus_write(BLTAPTL, 0x2000);  // even
        h.bus_write(BLTBPTH, 0x01); h.bus_write(BLTBPTL, 0x4001);  // odd
        h.bus_write(BLTCPTH, 0x02); h.bus_write(BLTCPTL, 0x6000);  // even
        h.bus_write(BLTDPTH, 0x03); h.bus_write(BLTDPTL, 0x8001);  // odd

        const uint32_t a = h.read_ptr(CHA);
        const uint32_t b = h.read_ptr(CHB);
        const uint32_t c = h.read_ptr(CHC);
        const uint32_t d = h.read_ptr(CHD);

        checks.expect(a == 0x1000 && b == 0xA000 && c == 0x13000 && d == 0x1C000,
            "FOUR-INDEPENDENT-PTR-BANKS: four channels written with four "
            "distinct pointer values (B and D via odd byte addresses) each read "
            "back as their own distinct, correct, even word address "
            "($1000/$A000/$13000/$1C000) - every blitter DMA channel has its "
            "own address pointer (6-dma-channels.c6, ch6-dma.c4, p.170; "
            "6-appa-registers.c111, p.270) that is a writable pair forming an "
            "even address (Appendix B's '+' mark, 'Address register pair. "
            "Must be an even address...', ch6-appb-legend.plus, p.287; applied "
            "per-register to all eight BLTxPTH/BLTxPTL entries: "
            "ch6-appb-bltapth.dma, ch6-appb-bltaptl.dma, ch6-appb-bltbpth.dma, "
            "ch6-appb-bltbptl.dma, ch6-appb-bltcpth.dma, ch6-appb-bltcptl.dma, "
            "ch6-appb-bltdpth.dma, ch6-appb-bltdptl.dma, p.289 - the 'pointing "
            "to chip memory' half of that mark is not observable here and is "
            "not asserted, see header)");
    }

    // ---------------- FOUR-INDEPENDENT-MOD-BANKS (p.170, p.173, p.270) ----------------
    {
        Harness h;
        // A common pointer baseline on every channel, so the only thing that
        // can make the four results differ is each channel's OWN modulo.
        h.bus_write(BLTAPTH, 0x00); h.bus_write(BLTAPTL, 0x2000);
        h.bus_write(BLTBPTH, 0x00); h.bus_write(BLTBPTL, 0x2000);
        h.bus_write(BLTCPTH, 0x00); h.bus_write(BLTCPTL, 0x2000);
        h.bus_write(BLTDPTH, 0x00); h.bus_write(BLTDPTL, 0x2000);

        h.bus_write(BLTAMOD, 0x0004);  // +2 words
        h.bus_write(BLTBMOD, 0x0008);  // +4 words
        h.bus_write(BLTCMOD, 0xFFF8);  // -4 words
        h.bus_write(BLTDMOD, 0x000C);  // +6 words

        h.apply(CHA, false, false, true, false);
        h.apply(CHB, false, false, true, false);
        h.apply(CHC, false, false, true, false);
        h.apply(CHD, false, false, true, false);

        const uint32_t a = h.read_ptr(CHA);
        const uint32_t b = h.read_ptr(CHB);
        const uint32_t c = h.read_ptr(CHC);
        const uint32_t d = h.read_ptr(CHD);

        checks.expect(a == 0x1002 && b == 0x1004 && c == 0x0FFC && d == 0x1006,
            "FOUR-INDEPENDENT-MOD-BANKS: four channels started from the SAME "
            "pointer ($1000) but given four different modulos each land at a "
            "different address after their own addmod ($1002/$1004/$0FFC/"
            "$1006) - every DMA channel has its own modulo register "
            "(6-dma-channels.c7, ch6-dma.c5, p.170; 6-appa-registers.c90, "
            "6-appa-registers.c94, 6-appa-registers.c95, ch6-appa-bltxmod.c4, "
            "ch6-appa-bltxmod.c7, ch6-appa-bltxmod.c8, p.270), which is what "
            "lets 'data ... be moved among bitplanes of different widths' "
            "(6-dma-channels.c72, ch6-dma.c73, p.173)");
    }

    // ---------------- POINTER-VALUE-AFTER-BLIT (p.172, p.270) ----------------
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x2000);  // ptr = $1000, blit start
        h.apply(CHA, true, false, false, false);  // word 1
        h.apply(CHA, true, false, false, false);  // word 2
        h.apply(CHA, true, false, false, false);  // word 3, ptr now $1003
        h.bus_write(BLTAMOD, 0x0006);             // +3 words
        h.apply(CHA, false, false, true, false);  // end-of-row modulo
        const uint32_t got = h.read_ptr(CHA);

        checks.expect(got == 0x1006,
            "POINTER-VALUE-AFTER-BLIT: after three word-increments and one "
            "modulo add (simulating a 3-word row with a 3-word/6-byte modulo), "
            "the pointer holds $1000 + 3 (the increments) + 3 (the modulo) = "
            "$1006 - 'will contain the last data address (plus increment ... "
            "and modulo)' (6-appa-registers.c114, ch6-appa-bltxpt.c8, p.270), "
            "and the same additive relationship 'width ... times two plus the "
            "modulo value (in bytes) should equal the full width' describes "
            "(ch6-dma.c67, p.172); the same landing point - increments through "
            "the row, then the modulo added at its end - is what 'address "
            "point to the start of the next line' names (6-appa-registers.c93, "
            "p.270; reclassified into this package)");
    }

    // ---------------- FIGURE-6-2-PTR-STIMULUS (p.173, figure) ----------------
    //
    // Figure 6-2's BLTxPTR = <Mem_Addr> + (40*13) + 12 = <Mem_Addr> + 532 is a
    // host-side address computation (row stride * row + column offset) that
    // this module never performs - it only holds whatever byte address
    // software preloads. What IS testable here is that preloading it with the
    // figure's own resulting byte address (taking <Mem_Addr> = word address 0)
    // is held and reported correctly, using the manual's own worked number as
    // the stimulus.
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x0214);  // 532 decimal, the figure's own value
        const uint32_t got = h.read_ptr(CHA);

        checks.expect(got == 0x010A,
            "FIGURE-6-2-PTR-STIMULUS: preloading the pointer with byte address "
            "532 - Figure 6-2's own worked value, 'BLTxPTR = <Mem_Addr> + "
            "(40*13) + 12 = <Mem_Addr> + 532' with <Mem_Addr> taken as word "
            "address 0 (6-dma-channels.c77, ch6-dma.f9, ch6-dma.f10, p.173) - "
            "reads back as word address $010A (532/2), the same byte-address-"
            "with-LSB-dropped rule PTR-BYTE-LSB-IGNORED measures above, applied "
            "to the manual's own figure rather than an arbitrary number");
    }

    // ---------------- FIGURE-6-2-MOD-STIMULUS (p.173, figure) ----------------
    {
        Harness h;
        h.bus_write(BLTAPTH, 0x0000);
        h.bus_write(BLTAPTL, 0x0000);  // ptr = 0
        h.bus_write(BLTAMOD, 0x001E);  // 30 decimal, the figure's own value
        h.apply(CHA, false, false, /*addmod=*/true, false);
        const uint32_t got = h.read_ptr(CHA);

        checks.expect(got == 0x000F,
            "FIGURE-6-2-MOD-STIMULUS: applying a modulo of 30 bytes - Figure "
            "6-2's own worked value, 'BLTxMOD = 12 + 18 = 30 bytes' "
            "(6-dma-channels.c78, ch6-dma.f11, ch6-dma.f12, p.173) - advances "
            "the pointer by 15 words (30/2), the same sign-extended-add rule "
            "MOD-ADD-SIGN-EXTENDED measures above, applied to the manual's own "
            "figure rather than an arbitrary number");
    }

    // ---------------- ECS-FIVE-BIT-PTH (p.270, p.309) ----------------
    {
        Harness h;
        h.bus_write(BLTAPTL, 0x0000);
        h.bus_write(BLTAPTH, 0x001F);  // all five low bits of the high word set
        const uint32_t got = h.read_ptr(CHA);

        checks.expect(got == 0xF8000,
            "ECS-FIVE-BIT-PTH: writing all five low bits of BLTAPTH ($001F) "
            "and reading channel A's pointer gives $F8000 - every one of the "
            "five bits took the write, unconditionally (this module has no "
            "`ecs` input at all - agnus_blitter_adrgen.v:4-20's port list has "
            "no such signal to gate it), matching the ECS-widened form 'high 5 "
            "bits, was 3 bits' (6-appc-modifications.c1, "
            "6-appc-modifications.c2, 6-appc-modifications.c3, "
            "6-appc-modifications.c10, ch6-appc-ecs.c1, ch6-appc-ecs.c2, "
            "ch6-appc-ecs.c3, ch6-appc-map.bltxpth, p.309; "
            "6-appa-registers.c106, ch6-appa-bltxpt.c3, p.270). The original "
            "chip set's 3-bit/18-bit alternative (6-appa-registers.c110, "
            "ch6-appa-bltxpt.c2, ch6-appa-bltxpt.c6, p.270) is not separately "
            "constructible at this boundary and is not asserted - see header");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
