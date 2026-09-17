// appDE-lightpen-beam
//
// Grades 4 claims (2 propositions, each claimed independently by readers sol
// and opus) against `agnus_beamcounter` (compiled alone), from Appendix E
// p.333's LIGHT PEN figure -- the two boxes:
//
//     VPOSR  read only    DFF004
//     VHPOSR read only    DFF006
//
// uid -> proposition:
//   sol:E-lightpen.c15, opus:E-lightpen.c18  "VPOSR is read-only at DFF004"
//   sol:E-lightpen.c16, opus:E-lightpen.c19  "VHPOSR is read-only at DFF006"
//
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 333 (Appendix E, LIGHT PEN figure).
//
// BOUNDARY: nanomig/src/minimig-aga/agnus_beamcounter.v:29-56 -- the
//   module's whole port list. Confirmed by direct read: no LPEN, trigger or
//   freeze input anywhere in it, and the only lpen-shaped signal in this
//   file is a commented-out wire (agnus_beamcounter.v:129,
//   `//wire lpendis = beamcon0_reg[13];`). This module cannot latch a beam
//   position on an external pen trigger; it can only be read for its own
//   free-running vpos/hpos content, which is all any of this package's four
//   claims need.
//
// DISPOSITION -- all four claims DECLINE, both readers, both propositions.
//   Neither register's address is in dispute (unlike POTINP/BPLCON0 two
//   boxes below them on the same page, which both print $DFF104 -- see
//   appDE-lightpen-userio and appDE-lightpen-denise for that defect).
//   $DFF004/$DFF006 are undisputed and each proposition splits cleanly into
//   an ADDRESS half and a DIRECTION (read-only) half, and BOTH halves of
//   BOTH propositions are already graded in accepted chapters:
//
//   ADDRESS: hrm/ch3-register-map-agnus-beamcounter's VPOSR-ADDRESS-004 and
//     VHPOSR-ADDRESS-006 assertions (verified read directly in that
//     package's test_agnus_beamcounter.cpp) grade exactly "this register
//     lives at this address", citing appendix B p.289 ('VPOSR *004 R',
//     'VHPOSR *006 R') and chapter 8 p.229 ('VPOSR ($DFF004) Read light pen
//     position (high order bits)', 'VHPOSR ($DFF006) Read light pen
//     position (low order bits)') -- chapter 8's own citation is this same
//     LIGHT PEN material, one page earlier than appendix E's own figure.
//
//   DIRECTION: hrm/ch7-beam-position-word-layout's VPOSR-IS-READ-ONLY and
//     VHPOSR-IS-READ-ONLY assertions (also verified read directly) grade
//     exactly "writing to this address changes nothing a subsequent read at
//     the same address reports", citing p.216, p.285 and p.298. This
//     package's own classify.note claimed the direction half was "graded
//     from the p.287 legend in the same package" as the address half
//     (ch3-register-map-agnus-beamcounter) -- CHECKED AND WRONG: that
//     package's test file contains no occurrence of "287" at all, and its
//     own DIRECTION methodology note (lines 95-103) scopes DIRECTION
//     assertions to the ten Appendix-C write-only registers only, which do
//     not include VPOSR/VHPOSR. The correct citation, confirmed by reading
//     ch7-beam-position-word-layout/test_agnus_beamcounter.cpp directly, is
//     the VPOSR-IS-READ-ONLY / VHPOSR-IS-READ-ONLY pair above, in a
//     DIFFERENT accepted package than the one the note names. The
//     underlying disposition (decline as a re-grade) is unaffected; only
//     the citation is corrected here.
//
//   Both halves of both propositions being independently already graded
//   means this page's four claims add no new axis: p.333 restates the same
//   two register facts chapter 7/chapter 8 already state, under the LIGHT
//   PEN heading. Declined as a re-grade in full, not split further.
//
// FIXTURE LIVENESS. No claim id, no manual page: proves the harness can
//   drive vpos/hpos to a chosen, non-default value and read it back at both
//   VPOSR and VHPOSR before any decline above is trusted to have had a real
//   fixture behind it. Harness/force_vpos mechanics reused verbatim from
//   hrm/ch3-register-map-agnus-beamcounter/test_agnus_beamcounter.cpp (same
//   module, same already-public boundary) per the precedent in
//   hrm/ch3-collision-registers/ for reusing an already-built sibling
//   harness for the identical top module.
//
// MUTATION FIXTURE-LIVENESS stimulus red: force_vpos's second call was
//   replayed with the FIRST call's own parameters (vpos11 0x2AA->0x000,
//   long_frame true->false, hpos8_1 0x55->0x00), so nothing about the
//   forced state actually changed and both the VPOSR and VHPOSR
//   before/after comparisons found no difference. Built with
//   `make MUT=FIXTURE_LIVENESS`, confirmed "0 passed, 1 failed", reverted
//   (obj_dir cleaned, rebuilt with no MUT flag) and confirmed "1 passed, 0
//   failed" restored.

#include <cstdint>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>

#include <verilated.h>
#include "Vagnus_beamcounter.h"

namespace {

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "NOTE: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

std::string hex16(uint16_t v) {
    std::ostringstream os;
    os << "0x" << std::hex << std::setw(4) << std::setfill('0') << v;
    return os.str();
}

// Register addresses, as reg_address_in values (port carries address bits
// [8:1]; a custom-chip byte address is always even, so the port value is
// the byte address shifted right by one).
constexpr int A_VPOSR  = 0x004 >> 1;
constexpr int A_VPOSW  = 0x02A >> 1;
constexpr int A_VHPOSR = 0x006 >> 1;
constexpr int A_VHPOSW = 0x02C >> 1;

class Harness {
public:
    explicit Harness(bool ecs, bool ntsc, bool aga = false)
        : dut(new Vagnus_beamcounter) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->cck = 0;
        dut->ntsc = ntsc ? 1 : 0;
        dut->aga = aga ? 1 : 0;
        dut->ecs = ecs ? 1 : 0;
        dut->a1k = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        for (int i = 0; i < 10; ++i) {
            dut->clk7_en = 1;
            tick();
        }
        dut->reset = 0;
        dut->clk7_en = 0;
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    void write_reg(int addr, uint16_t data) {
        dut->reg_address_in = static_cast<uint16_t>(addr & 0xFF);
        dut->data_in = data;
        dut->clk7_en = 1;
        tick();
        dut->clk7_en = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->eval();
    }

    // VPOSR/VHPOSR readback: combinational mux, no clock edge needed.
    uint16_t read(int addr) {
        dut->reg_address_in = static_cast<uint16_t>(addr & 0xFF);
        dut->eval();
        uint16_t v = static_cast<uint16_t>(dut->data_out);
        dut->reg_address_in = 0;
        dut->eval();
        return v;
    }

    Vagnus_beamcounter* dut;
};

// Writes VHPOSW then VPOSW. hpos8_1 lets a caller park hpos past the
// vpos_inc trigger point so the forced value survives. Reused verbatim from
// hrm/ch3-register-map-agnus-beamcounter/test_agnus_beamcounter.cpp (same
// module).
void force_vpos(Harness& h, uint32_t vpos11, bool long_frame, uint8_t hpos8_1 = 0) {
    uint16_t vhposw_data = static_cast<uint16_t>(((vpos11 & 0xFF) << 8) | hpos8_1);
    h.write_reg(A_VHPOSW, vhposw_data);
    uint16_t vposw_data = static_cast<uint16_t>((long_frame ? 0x8000 : 0x0000) |
                                                 ((vpos11 >> 8) & 0x7));
    h.write_reg(A_VPOSW, vposw_data);
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ================= FIXTURE-LIVENESS (no page, no uid) =================
    {
        Harness h(/*ecs=*/true, /*ntsc=*/false);
        force_vpos(h, /*vpos11=*/0x000, /*long_frame=*/false, /*hpos8_1=*/0x00);
        uint16_t vposr_before = h.read(A_VPOSR);
        uint16_t vhposr_before = h.read(A_VHPOSR);
        uint32_t vpos11_second = 0x2AA;
        bool long_frame_second = true;
        uint8_t hpos8_1_second = 0x55;
#ifdef MUT_FIXTURE_LIVENESS
        // Replays the FIRST call's parameters exactly: no change forced.
        vpos11_second = 0x000;
        long_frame_second = false;
        hpos8_1_second = 0x00;
#endif
        force_vpos(h, vpos11_second, long_frame_second, hpos8_1_second);
        uint16_t vposr_after = h.read(A_VPOSR);
        uint16_t vhposr_after = h.read(A_VHPOSR);
        checks.expect(
            vposr_after != vposr_before && vhposr_after != vhposr_before,
            "FIXTURE-LIVENESS: forcing vpos/long_frame/hpos through VPOSW/VHPOSW to a "
            "second, chosen, non-default combination changes what a subsequent read "
            "reports at BOTH $004 (" + hex16(vposr_before) + " -> " + hex16(vposr_after) +
            ") and $006 (" + hex16(vhposr_before) + " -> " + hex16(vhposr_after) +
            ") -- the write-then-read data path is alive at this boundary before any "
            "claim below is declined on the strength of what this fixture COULD measure.");
    }

    // ================= sol:E-lightpen.c15, opus:E-lightpen.c18 =================
    checks.record(
        "sol:E-lightpen.c15, opus:E-lightpen.c18 (p.333 LIGHT PEN figure: 'VPOSR read "
        "only  DFF004'): both halves of this proposition are already graded in accepted "
        "chapters. ADDRESS: hrm/ch3-register-map-agnus-beamcounter's VPOSR-ADDRESS-004 "
        "grades 'this register lives at $004' (p.289 'VPOSR *004 R', p.229 'VPOSR "
        "($DFF004) Read light pen position (high order bits)' -- chapter 8's own "
        "citation for this address is this same LIGHT PEN material, one page earlier). "
        "DIRECTION: hrm/ch7-beam-position-word-layout's VPOSR-IS-READ-ONLY grades "
        "'writing to $004 changes nothing a subsequent read there reports' (p.216, "
        "p.285, p.298). Identical proposition on both axes, no narrower or "
        "differently-axised statement here; declined as a re-grade in full.");

    // ================= sol:E-lightpen.c16, opus:E-lightpen.c19 =================
    checks.record(
        "sol:E-lightpen.c16, opus:E-lightpen.c19 (p.333 LIGHT PEN figure: 'VHPOSR read "
        "only  DFF006'): both halves of this proposition are already graded in accepted "
        "chapters. ADDRESS: hrm/ch3-register-map-agnus-beamcounter's VHPOSR-ADDRESS-006 "
        "grades 'this register lives at $006' (p.289 'VHPOSR *006 R', p.229 'VHPOSR "
        "($DFF006) Read light pen position (low order bits)' -- chapter 8's own "
        "citation for this address is this same LIGHT PEN material, one page earlier). "
        "DIRECTION: hrm/ch7-beam-position-word-layout's VHPOSR-IS-READ-ONLY grades "
        "'writing to $006 changes nothing a subsequent read there reports' (p.216, "
        "p.285). Identical proposition on both axes, no narrower or differently-axised "
        "statement here; declined as a re-grade in full.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
