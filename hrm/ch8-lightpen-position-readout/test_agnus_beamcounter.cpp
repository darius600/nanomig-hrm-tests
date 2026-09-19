// Test purpose: ch8-lightpen-position-readout Oracle: Amiga Hardware
//   Reference Manual, page 239 (light pen position register resolution table
//   and light-pen-trigger detection procedures).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 227, 239, 241
// Non-goals: anything outside the `agnus_beamcounter` fixture this package
//   builds, and any claim outside the 14 it cites. A green here means those
//   assertions held on this fixture at the pinned revision -- not that the
//   module is correct, and not that the chapter is covered.
// Claim ids: 14 ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in the
//   chapter's claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
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
// MUTATION VRES-INTERLACED-VPOS-AMBIGUOUS-ACROSS-FIELDS|opus:8-lpen.c29|sol:8-pen.c30 stimulus red forced the BPLCON0 write to 0x0000 (LACE off) instead of 0x0004 (LACE on), so field1 sticks at 1 and no probe vpos is ever seen under field1=0
// MUTATION FIELD-IDENTITY-PREDICTS-FIELD-LENGTH|opus:8-lpen.c30|sol:8-pen.c31 stimulus red forced the BPLCON0 write to 0x0000 (LACE off) instead of 0x0004 (LACE on), so every captured field ends at vpos 311 and no 312-line field is ever observed to check the correlation against
// MUTATION HRES-2LOWRESPX-PER-VHPOSR-STEP-LOWRES|opus:8-lpen.c31|sol:8-pen.c32 stimulus red forced the cck input constantly high instead of toggling each tick, collapsing the 2:1 hpos:VHPOSR ratio to 1:1
// MUTATION HRES-2LOWRESPX-PER-VHPOSR-STEP-HIRES|opus:8-lpen.c31|sol:8-pen.c33 stimulus red forced the cck input constantly high instead of toggling each tick, collapsing the 2:1 hpos:VHPOSR ratio to 1:1
// MUTATION MASKED-LONGWORD-VARIES-WITH-VERTICAL-POSITION stimulus red forced the V=260 measurement point to V=261 instead, colliding two of the three sampled positions
// MUTATION MASKED-LONGWORD-V261-EQUALS-0x10500|opus:8-lpen.c46|opus:8-lpen.c51|sol:8-pen.c43 stimulus red forced the V=261 measurement point to V=262 instead, leaving the $10500 comparison target unchanged
//
// Repair 2026-09-15 (independent validation, finding 7): three assertions
// carried claims they did not test.
//   - opus:8-lpen.c29/sol:8-pen.c30 and opus:8-lpen.c30/sol:8-pen.c31 were
//     carried by assertions that only measured field-length parity
//     (311/312 alternation) and field1 toggling -- neither is the
//     resolution-loss claim or the recovery-of-position claim itself. Both
//     are now measured directly below: A2 shows the SAME vpos value
//     recurring under both field1 states while LACE is on (the resolution
//     loss the manual names), and A3 shows field1 reliably predicts which
//     field-length regime -- 311- or 312-line -- is active, i.e. that field
//     identity is decodable information and not just a toggle.
//   - opus:8-lpen.c45 and sol:8-pen.c42 quote the manual's read-twice /
//     mask / compare SOFTWARE procedure, not a register-layout fact.
//     Declined below (see RECORDED lines): agnus_beamcounter.v has no LPEN
//     input and no freeze-on-trigger logic -- grepping this whole pinned
//     RTL tree turns up only a commented-out `lpendis` wire here and a
//     commented-out `lpen`/bplcon0[3] wire in denise.v. The freeze this
//     procedure depends on (so that "two reads equal" means anything) is
//     not implemented anywhere in this submodule, so no module boundary in
//     this suite can carry it as built. The masked-longword-layout fact
//     these two uids were previously stretched to cover is still real and
//     still measured below, but it now carries no uid of its own: the
//     uids' actual words are the procedure, not the layout.

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

    // --- P2 / A2 and P3 / A3: vpos ambiguity across interlaced fields, and field
    // identity as the disambiguating bit page 239's parenthetical names ---
    //
    // A2 tests the resolution-loss claim directly: three vpos values spread
    // across the field range are each shown to recur -- once under
    // field1==0 and once under field1==1 -- while LACE is on. That is the
    // literal content of "vertical resolution is 2 scan lines in interlaced
    // mode": the same digital vpos reading denotes two different field
    // instances, so vpos alone cannot tell them apart.
    //
    // A3 tests the recovery claim: field1 does not merely toggle, it
    // reliably predicts which field-length regime (the already-validated
    // 311- vs 312-line alternation) is active, across four consecutive
    // fields. That is what makes field identity decodable information a
    // program can use, rather than an arbitrary flag -- the literal content
    // of "if you know which interlaced frame is under display, you can
    // determine the correct position." Neither assertion claims to recover
    // the physical CRT line itself; that interleaving is outside the chip
    // (see SPECIFICATION.md).
    {
        uint16_t bplcon0_val = 0x0004; // LACE=1
#if defined(MUTATE_A2) || defined(MUTATE_A3)
        bplcon0_val = 0x0000; // stimulus mutation: LACE forced off
#endif
        write_reg(dut, BPLCON0_ADDR, bplcon0_val);
        force_vpos_hpos(dut, 0, 0, false); // vpos=0, field1=1, start of a field

        const uint32_t probes[3] = {50, 150, 300}; // well inside both the 311- and 312-line field lengths
        std::vector<int> probe_field1[3];
        std::vector<uint32_t> field_end_vpos;
        std::vector<int> field_end_field1;

        uint32_t prev_vpos = dut->vpos;
        int prev_field1 = dut->field1;
        int guard = 0;
        // Four consecutive field ends: enough to see both field lengths more
        // than once, and comfortably more than the two field passes needed
        // to revisit every probe.
        while (field_end_vpos.size() < 4 && guard < 1200000) {
            tick(dut);
            uint32_t v = dut->vpos;
            if (v != prev_vpos) {
                for (int i = 0; i < 3; i++)
                    if (v == probes[i]) probe_field1[i].push_back(dut->field1);
                if (v < prev_vpos) { // wrapped: prev_vpos was this field's last line
                    field_end_vpos.push_back(prev_vpos);
                    field_end_field1.push_back(prev_field1);
                }
                prev_vpos = v;
            }
            prev_field1 = dut->field1;
            guard++;
        }

        auto fmt_seen = [](const std::vector<int>& v) {
            std::string s;
            for (size_t i = 0; i < v.size(); i++) s += (i ? "," : "") + std::to_string(v[i]);
            return s;
        };
        note("probe vpos " + std::to_string(probes[0]) + "/" + std::to_string(probes[1]) + "/" + std::to_string(probes[2]) +
             " seen with field1=[" + fmt_seen(probe_field1[0]) + "]/[" + fmt_seen(probe_field1[1]) + "]/[" + fmt_seen(probe_field1[2]) + "]");
        {
            std::string s;
            for (size_t i = 0; i < field_end_vpos.size(); i++)
                s += (i ? ", " : "") + std::to_string(field_end_vpos[i]) + "(field1=" + std::to_string(field_end_field1[i]) + ")";
            note("four consecutive field ends: " + s);
        }

        bool a2_ok = true;
        for (int i = 0; i < 3; i++) {
            bool saw0 = false, saw1 = false;
            for (int f : probe_field1[i]) { if (f == 0) saw0 = true; if (f == 1) saw1 = true; }
            a2_ok = a2_ok && saw0 && saw1;
        }
        expect(a2_ok, "VRES-INTERLACED-VPOS-AMBIGUOUS-ACROSS-FIELDS|opus:8-lpen.c29|sol:8-pen.c30");

        bool saw_311 = false, saw_312 = false, consistent = true;
        for (size_t i = 0; i < field_end_vpos.size(); i++) {
            if (field_end_vpos[i] == 311) { saw_311 = true; if (field_end_field1[i] != 1) consistent = false; }
            else if (field_end_vpos[i] == 312) { saw_312 = true; if (field_end_field1[i] != 0) consistent = false; }
            else consistent = false; // an unexpected field length is itself a failure of the correlation
        }
        bool a3_ok = saw_311 && saw_312 && consistent;
        expect(a3_ok, "FIELD-IDENTITY-PREDICTS-FIELD-LENGTH|opus:8-lpen.c30|sol:8-pen.c31");
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

        // opus:8-lpen.c45 and sol:8-pen.c42 quote the manual's read-twice /
        // mask / compare procedure -- a software operation performed by the
        // CPU on two already-read longwords, not a hardware layout fact.
        // agnus_beamcounter has no LPEN input and no freeze-on-trigger
        // logic at all (confirmed by reading the module: the only trace is
        // a commented-out `lpendis` wire here and a commented-out `lpen`/
        // bplcon0[3] wire in denise.v), so the "two reads equal" half of
        // the procedure -- the thing that gives the mask-and-compare step
        // its meaning -- cannot be exercised at this or, as pinned, any
        // module boundary in this tree. Declined; a CPU/firmware-level
        // harness driving the actual detection routine against a system
        // model that implements the LPEN freeze is what would carry it,
        // and no such model exists in this submodule.
        record("opus:8-lpen.c45 declined: quotes the read-twice/mask/compare procedure, a CPU-side operation; agnus_beamcounter has no LPEN input and no freeze-on-trigger logic (grep of this RTL tree finds only commented-out lpendis/lpen wires), so the equal-reads precondition this procedure depends on cannot be exercised here or, as built, anywhere in this submodule. Would need: a CPU/firmware-level test against a system model that implements the LPEN freeze.");
        record("sol:8-pen.c42 declined: same reason as opus:8-lpen.c45 -- it quotes the same procedural step (\"the trigger test masks off the upper 15 bits\"), not a register-layout fact.");

        // The layout fact those two uids' classify-time "split" had
        // reinterpreted them as -- that the masked remainder still
        // discriminates distinct vertical positions -- is real and still
        // worth checking as a regression, but it is not what either uid's
        // own words say, so it now carries no uid.
        bool layout_ok = m260 != m261 && m261 != m262 && m260 < m261 && m261 < m262;
        expect(layout_ok, "MASKED-LONGWORD-VARIES-WITH-VERTICAL-POSITION");

        bool a6_ok = m261 == 0x10500u;
        expect(a6_ok, "MASKED-LONGWORD-V261-EQUALS-0x10500|opus:8-lpen.c46|opus:8-lpen.c51|sol:8-pen.c43");
    }

    std::cout << checks.passed << " passed, " << checks.failed << " failed\n";
    dut->final();
    delete dut;
    return checks.failed == 0 ? 0 : 1;
}
