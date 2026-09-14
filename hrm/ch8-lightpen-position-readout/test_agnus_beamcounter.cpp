// ch8-lightpen-position-readout
// Oracle: Amiga Hardware Reference Manual, page 239 (light pen position
// register resolution table and light-pen-trigger detection procedures).
// Fixture: agnus_beamcounter.v, compiled alone.
// Chipset scope: OCS baseline, PAL -- ecs=0, ntsc=0, aga=0, a1k=0 held for
// every assertion (page 239 names no chip set; see SPECIFICATION.md).
// clk7_en is tied high throughout (this testbench treats "clk" as the
// clk7 domain directly). cck is toggled by this testbench once per tick
// (0,1,0,1,...) so that the module's own raw hpos[8:0] output advances by
// exactly one "140 ns" unit per tick -- matching the comment at
// agnus_beamcounter.v:40 -- which is what makes the VHPOSR 2:1 granularity
// claim (P4a/P4b) meaningful to test at all: tying cck constantly high
// collapses that ratio to 1:1, which is exactly assertion P4a/P4b's own
// mutation below.
//
// MUTATION LIVENESS-VPOS-DISTINCT-READBACK stimulus red forced two of the three target vpos values identical (50,50,261) instead of distinct (50,150,261)
// MUTATION VRES-NONINTERLACED-1-LINE-PER-VPOS-STEP|opus:8-lpen.c28|sol:8-pen.c29 stimulus red injected a stray VPOSW/VHPOSW write mid-run forcing vpos to 9 right after the 2nd eol pulse, skipping the expected 0,1,2,3,4 sequence
// MUTATION VRES-INTERLACED-FIELD-LENGTH-ALTERNATES|opus:8-lpen.c29|sol:8-pen.c30 stimulus red forced the BPLCON0 write to 0x0000 (LACE off) instead of 0x0004 (LACE on), so long_frame never auto-toggles and both captured field ends read 311
// MUTATION FIELD-IDENTITY-ALTERNATES-WITH-FIELD-LENGTH|opus:8-lpen.c30|sol:8-pen.c31 stimulus red forced the BPLCON0 write to 0x0000 (LACE off) instead of 0x0004 (LACE on), so field1 never changes across the two captured field ends
// MUTATION HRES-2LOWRESPX-PER-VHPOSR-STEP-LOWRES|opus:8-lpen.c31|sol:8-pen.c32 stimulus red forced the cck input constantly high instead of toggling each tick, collapsing the 2:1 hpos:VHPOSR ratio to 1:1
// MUTATION HRES-2LOWRESPX-PER-VHPOSR-STEP-HIRES|opus:8-lpen.c31|sol:8-pen.c33 stimulus red forced the cck input constantly high instead of toggling each tick, collapsing the 2:1 hpos:VHPOSR ratio to 1:1
// MUTATION MASKED-LONGWORD-LAYOUT-DISCRIMINATES-POSITION|opus:8-lpen.c45|opus:8-lpen.c51|sol:8-pen.c42 stimulus red forced the V=260 measurement point to V=261 instead, colliding two of the three sampled positions
// MUTATION MASKED-LONGWORD-V261-EQUALS-0x10500|opus:8-lpen.c46|opus:8-lpen.c51|sol:8-pen.c43 stimulus red forced the V=261 measurement point to V=262 instead, leaving the $10500 comparison target unchanged

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <array>
#include <vector>
#include "verilated.h"
#include "Vagnus_beamcounter.h"

// Register addresses, straight from the manual's own chip register map
// (already established in this suite by hrm/ch3-register-map-agnus-beamcounter).
static const uint16_t VPOSR_ADDR   = 0x004;
static const uint16_t VPOSW_ADDR   = 0x02A;
static const uint16_t VHPOSR_ADDR  = 0x006;
static const uint16_t VHPOSW_ADDR  = 0x02C;
static const uint16_t BPLCON0_ADDR = 0x100;
static const uint8_t  IDLE_ADDR    = 0xFF; // matches no register on this module

struct Checks { int passed = 0; int failed = 0; };
Checks checks;

bool expect(bool cond, const std::string& label) {
    if (cond) { checks.passed++; std::cout << "PASS: " << label << "\n"; }
    else      { checks.failed++; std::cout << "FAIL: " << label << "\n"; }
    return cond;
}
void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }

std::string to_hex(uint32_t v) {
    char buf[16];
    std::snprintf(buf, sizeof(buf), "0x%x", v);
    return std::string(buf);
}

static uint64_t g_cck_phase = 0;
static bool g_force_cck_high = false;

void tick(Vagnus_beamcounter* dut) {
    dut->cck = g_force_cck_high ? 1 : (g_cck_phase & 1);
    g_cck_phase++;
    dut->clk7_en = 1;
    dut->clk = 0; dut->eval();
    dut->clk = 1; dut->eval();
}

void write_reg(Vagnus_beamcounter* dut, uint16_t addr9, uint16_t data) {
    dut->reg_address_in = (addr9 >> 1) & 0xFF;
    dut->data_in = data;
    tick(dut);
    dut->reg_address_in = IDLE_ADDR;
    dut->data_in = 0;
    dut->eval();
}

uint16_t read_reg(Vagnus_beamcounter* dut, uint16_t addr9) {
    dut->reg_address_in = (addr9 >> 1) & 0xFF;
    dut->eval();
    uint16_t v = static_cast<uint16_t>(dut->data_out);
    dut->reg_address_in = IDLE_ADDR;
    dut->eval();
    return v;
}

// Force vpos and hpos[8:1] to known values in one VPOSW + one VHPOSW write.
void force_vpos_hpos(Vagnus_beamcounter* dut, uint32_t vpos_val, uint8_t hpos81, bool long_frame) {
    uint16_t vposw_data  = static_cast<uint16_t>(((long_frame ? 1u : 0u) << 15) | ((vpos_val >> 8) & 0x7));
    uint16_t vhposw_data = static_cast<uint16_t>(((vpos_val & 0xFF) << 8) | hpos81);
    write_reg(dut, VPOSW_ADDR, vposw_data);
    write_reg(dut, VHPOSW_ADDR, vhposw_data);
}

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Vagnus_beamcounter* dut = new Vagnus_beamcounter;

    dut->reset = 1;
    dut->clk7_en = 1;
    dut->ntsc = 0;
    dut->aga = 0;
    dut->ecs = 0;
    dut->a1k = 0;
    dut->data_in = 0;
    dut->reg_address_in = IDLE_ADDR;
    for (int i = 0; i < 8; i++) tick(dut);
    dut->reset = 0;
    for (int i = 0; i < 4; i++) tick(dut);

    // --- LIVENESS: the fixture can deliver a known word before anything reads one ---
    {
        uint32_t targets[3] = {50, 150, 261};
#ifdef MUTATE_A0
        targets[1] = 50; // stimulus mutation: collide target[1] with target[0]
#endif
        uint32_t seen[3];
        for (int i = 0; i < 3; i++) {
            force_vpos_hpos(dut, targets[i], 0, false);
            seen[i] = dut->vpos;
        }
        note("liveness targets " + std::to_string(targets[0]) + "," + std::to_string(targets[1]) + "," + std::to_string(targets[2])
             + " read back as " + std::to_string(seen[0]) + "," + std::to_string(seen[1]) + "," + std::to_string(seen[2]));
        expect(seen[0] == targets[0] && seen[1] == targets[1] && seen[2] == targets[2]
               && seen[0] != seen[1] && seen[1] != seen[2] && seen[0] != seen[2],
               "LIVENESS-VPOS-DISTINCT-READBACK");
    }

    // --- P1 / A1: vertical resolution, non-interlaced (page 239) ---
    {
        write_reg(dut, BPLCON0_ADDR, 0x0000); // LACE=0, ERSY=0, HIRES=0, SHRES=0
        force_vpos_hpos(dut, 0, 0, false);
        int eol_count = 0;
#ifdef MUTATE_A1
        bool stray_done = false;
#endif
        while (eol_count < 4) {
            tick(dut);
            if (dut->eol) eol_count++;
#ifdef MUTATE_A1
            if (!stray_done && eol_count == 2) {
                force_vpos_hpos(dut, 9, 0, false);
                stray_done = true;
            }
#endif
        }
        note("after 4 eol pulses from a vpos=0,hpos=0,LACE=0 start, vpos reads " + std::to_string((unsigned)dut->vpos));
        expect(dut->vpos == 4, "VRES-NONINTERLACED-1-LINE-PER-VPOS-STEP|opus:8-lpen.c28|sol:8-pen.c29");
    }

    // --- P2 / A2 and P3 / A3: interlaced field-length alternation and field identity ---
    {
        uint16_t bplcon0_val = 0x0004; // LACE=1
#if defined(MUTATE_A2) || defined(MUTATE_A3)
        bplcon0_val = 0x0000; // stimulus mutation: LACE forced off
#endif
        write_reg(dut, BPLCON0_ADDR, bplcon0_val);
        force_vpos_hpos(dut, 309, 0, false); // shortcut close to vtotal=311, long_frame starts 0

        // vpos_inc is asserted one tick before the edge that actually applies
        // it (the RTL's own comment: "vpos changes after hpos equals 3"), so
        // eol/eof read 1 one tick ahead of the vpos/long_frame update they
        // describe -- forcing a register write right when eof is first seen
        // pre-empts that same edge's natural wrap (a write's address match
        // takes priority over the vpos_inc branch in agnus_beamcounter.v's
        // own if-elseif chain) instead of following it. So this measurement
        // does not try to catch eof at all: it just watches vpos itself and
        // detects the wrap directly, letting both fields run to completion
        // (the shortcut only saves the run-up to the first field's end; the
        // second field is left to complete naturally, ~313 lines, still well
        // under a second of simulated ticks).
        std::vector<uint32_t> field_end_vpos;
        std::vector<int> field_end_field1;
        uint32_t prev_vpos = dut->vpos;
        int prev_field1 = dut->field1;
        int guard = 0;
        while (field_end_vpos.size() < 2 && guard < 200000) {
            tick(dut);
            if (dut->vpos < prev_vpos) { // wrapped: prev_vpos was this field's last line
                field_end_vpos.push_back(prev_vpos);
                field_end_field1.push_back(prev_field1);
            }
            prev_vpos = dut->vpos;
            prev_field1 = dut->field1;
            guard++;
        }
        note("two successive field ends captured at vpos " +
             (field_end_vpos.size() > 0 ? std::to_string(field_end_vpos[0]) : std::string("?")) + " and " +
             (field_end_vpos.size() > 1 ? std::to_string(field_end_vpos[1]) : std::string("?")) +
             ", field1 was " +
             (field_end_field1.size() > 0 ? std::to_string(field_end_field1[0]) : std::string("?")) + " and " +
             (field_end_field1.size() > 1 ? std::to_string(field_end_field1[1]) : std::string("?")) +
             " just before each");

        bool a2_ok = field_end_vpos.size() == 2 && field_end_vpos[0] == 311 && field_end_vpos[1] == 312;
        expect(a2_ok, "VRES-INTERLACED-FIELD-LENGTH-ALTERNATES|opus:8-lpen.c29|sol:8-pen.c30");

        bool a3_ok = field_end_field1.size() == 2 && field_end_field1[0] == 1 && field_end_field1[1] == 0;
        expect(a3_ok, "FIELD-IDENTITY-ALTERNATES-WITH-FIELD-LENGTH|opus:8-lpen.c30|sol:8-pen.c31");
    }

    // --- P4a / A4a: horizontal resolution, low-resolution display ---
    {
        write_reg(dut, BPLCON0_ADDR, 0x0000); // LACE=0, HIRES=0, SHRES=0 (low-res)
        force_vpos_hpos(dut, 0, 0, false);
        dut->reg_address_in = (VHPOSR_ADDR >> 1) & 0xFF; // hold VHPOSR read address
        dut->eval();
        // hpos_r only increments on a cck=1 tick and then holds through the
        // following cck=0 tick, so starting the phase at 0 (cck=0 first)
        // samples a short leading partial period before the first real
        // pair; starting at 1 (cck=1 first) makes every sampled pair clean
        // from index 0.
        g_cck_phase = 1;
#ifdef MUTATE_A4A
        g_force_cck_high = true;
#endif
        std::array<uint32_t, 6> vhposr_seq{};
        for (int i = 0; i < 6; i++) {
            tick(dut);
            dut->eval();
            vhposr_seq[i] = dut->data_out & 0xFF;
        }
        g_force_cck_high = false;
        dut->reg_address_in = IDLE_ADDR;
        dut->eval();
        note("low-res VHPOSR[7:0] over 6 ticks: " + std::to_string(vhposr_seq[0]) + "," + std::to_string(vhposr_seq[1]) + ","
             + std::to_string(vhposr_seq[2]) + "," + std::to_string(vhposr_seq[3]) + "," + std::to_string(vhposr_seq[4]) + "," + std::to_string(vhposr_seq[5]));
        bool pattern_ok = vhposr_seq[0] == vhposr_seq[1] && vhposr_seq[2] == vhposr_seq[3] && vhposr_seq[4] == vhposr_seq[5]
                        && vhposr_seq[1] + 1 == vhposr_seq[2] && vhposr_seq[3] + 1 == vhposr_seq[4];
        expect(pattern_ok, "HRES-2LOWRESPX-PER-VHPOSR-STEP-LOWRES|opus:8-lpen.c31|sol:8-pen.c32");
    }

    // --- P4b / A4b: horizontal resolution, high-resolution display ---
    {
        write_reg(dut, BPLCON0_ADDR, 0x8000); // LACE=0, HIRES=1
        force_vpos_hpos(dut, 0, 0, false);
        dut->reg_address_in = (VHPOSR_ADDR >> 1) & 0xFF;
        dut->eval();
        // hpos_r only increments on a cck=1 tick and then holds through the
        // following cck=0 tick, so starting the phase at 0 (cck=0 first)
        // samples a short leading partial period before the first real
        // pair; starting at 1 (cck=1 first) makes every sampled pair clean
        // from index 0.
        g_cck_phase = 1;
#ifdef MUTATE_A4B
        g_force_cck_high = true;
#endif
        std::array<uint32_t, 6> vhposr_seq{};
        for (int i = 0; i < 6; i++) {
            tick(dut);
            dut->eval();
            vhposr_seq[i] = dut->data_out & 0xFF;
        }
        g_force_cck_high = false;
        dut->reg_address_in = IDLE_ADDR;
        dut->eval();
        note("hi-res VHPOSR[7:0] over 6 ticks: " + std::to_string(vhposr_seq[0]) + "," + std::to_string(vhposr_seq[1]) + ","
             + std::to_string(vhposr_seq[2]) + "," + std::to_string(vhposr_seq[3]) + "," + std::to_string(vhposr_seq[4]) + "," + std::to_string(vhposr_seq[5]));
        bool pattern_ok = vhposr_seq[0] == vhposr_seq[1] && vhposr_seq[2] == vhposr_seq[3] && vhposr_seq[4] == vhposr_seq[5]
                        && vhposr_seq[1] + 1 == vhposr_seq[2] && vhposr_seq[3] + 1 == vhposr_seq[4];
        expect(pattern_ok, "HRES-2LOWRESPX-PER-VHPOSR-STEP-HIRES|opus:8-lpen.c31|sol:8-pen.c33");
    }

    // --- P5 / A5 and P6 / A6: masked long-word layout and the V=261 -> $10500 encoding ---
    {
        write_reg(dut, BPLCON0_ADDR, 0x0000);
        auto measure = [&](uint32_t vpos_val) -> uint32_t {
            force_vpos_hpos(dut, vpos_val, 0, false);
            uint16_t vposr = read_reg(dut, VPOSR_ADDR);
            uint16_t vhposr = read_reg(dut, VHPOSR_ADDR);
            uint32_t longword = (static_cast<uint32_t>(vposr) << 16) | vhposr;
            return longword & 0x1FFFFu; // mask off the upper 15 bits of the 32-bit word
        };
        uint32_t v260 = 260, v261 = 261, v262 = 262;
#ifdef MUTATE_A5
        v260 = 261; // stimulus mutation: collide the V=260 sample with V=261
#endif
#ifdef MUTATE_A6
        v261 = 262; // stimulus mutation: sample V=262 where V=261 was expected
#endif
        uint32_t m260 = measure(v260);
        uint32_t m261 = measure(v261);
        uint32_t m262 = measure(v262);
        note("masked long word at V=" + std::to_string(v260) + " is " + to_hex(m260)
             + ", at V=" + std::to_string(v261) + " is " + to_hex(m261)
             + ", at V=" + std::to_string(v262) + " is " + to_hex(m262));

        bool a5_ok = m260 != m261 && m261 != m262 && m260 < m261 && m261 < m262;
        expect(a5_ok, "MASKED-LONGWORD-LAYOUT-DISCRIMINATES-POSITION|opus:8-lpen.c45|opus:8-lpen.c51|sol:8-pen.c42");

        bool a6_ok = m261 == 0x10500u;
        expect(a6_ok, "MASKED-LONGWORD-V261-EQUALS-0x10500|opus:8-lpen.c46|opus:8-lpen.c51|sol:8-pen.c43");
    }

    std::cout << checks.passed << " passed, " << checks.failed << " failed\n";
    dut->final();
    delete dut;
    return checks.failed == 0 ? 0 : 1;
}
