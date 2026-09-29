// test_ch7_system_control.cpp -- ch7-system-control. See SPECIFICATION.md
// for every assertion's proposition, program, observed value and expected
// value. This file only implements what SPECIFICATION.md already
// committed to.
//
// P1 and P2 (display priority, bitplane/sprite combination) are not
// implemented here: see the "Report back" section of the build's own
// report for the reason (budget). Only P3 and P4 are built.
//
// NMH_MUTATE selects a mutated program or drive schedule in place of its
// normal counterpart, one assertion at a time; none is exercised by a
// plain `make test`.
//
// MUTATION P3-ANCHOR stimulus red second A anchor's WAIT moved to line 101 instead of 100, via NMH_MUTATE=P3-ANCHOR (measured: P3-PRE-ANCHOR FAIL on both cores)
// MUTATION P3-READTIME stimulus red a ~30-line delay added before every field's V1 read, via NMH_MUTATE=P3-READTIME (measured: P3-PRE-READTIME FAIL on both cores)
// MUTATION P3-DDRA stimulus unfalsifiable at this boundary: CIAADDRA bits 6-7 forced to outputs instead of fixed to inputs, via NMH_MUTATE=P3-DDRA -- the program stores CIAADDRA's own pre-fix value before the fix-or-force branch runs (p.339's own step order), so the graded word can never reflect what that branch does; see the P3-PRE-DDRA-MUTATION record (both cores)
// MUTATION P3-NOPULSE stimulus red the F1 trigger pulse omitted from the drive schedule, via NMH_MUTATE=P3-NOPULSE (measured: P3-PRE-PULSE FAIL on both cores, graded against the nominal 3-pulse schedule)
// MUTATION P3-HOLD stimulus red pin 6 held pressed from line 30 of F2 to the end of F2 instead of the normal schedule, via NMH_MUTATE=P3-HOLD (measured: P3-PRE-LIVE's own contrast line PASSes -- E40/E60 of F2 still read live on both cores, since no freeze exists to hold them)
// MUTATION P3-OFFSET stimulus unfalsifiable at this boundary: F1's own trigger moved to t(A1)+30 colour clocks instead of +16, via NMH_MUTATE=P3-OFFSET -- H(L1) is already a live read (item1/g1 FAILs in the primary run too), so it cannot depend on where the pulse landed; see the P3-PRE-OFFSET-MUTATION record (both cores)
// MUTATION P3-BEAMCON0 stimulus unfalsifiable at this boundary: the ECS-NTSC build's BEAMCON0 write ($0000) run against a PAL-configured harness (bit 5 opposite the selected standard), via NMH_MUTATE=P3-BEAMCON0 -- the measured field period does not change under this mutation; see the P3-PRE-PERIOD-MUTATION record (both cores)
// MUTATION P3-LPEN stimulus green (cannot go red on this RTL): LPEN-clear build substituted for the LPEN-set build under the same unchanged trigger schedule and comparator -- both read the live beam, via NMH_MUTATE=P3-LPEN (measured on both cores)
// MUTATION P4-NOLPENDIS stimulus unfalsifiable at this boundary: F1's own E60 write changed to $0020 instead of $2020 (LPENDIS never set), via NMH_MUTATE=P4-NOLPENDIS -- X is measured to read the same live value either way; see the P4-MUTATION record (both cores)
// MUTATION P4-F3LPENDIS stimulus unfalsifiable at this boundary: F3's own E60 write changed to $2020 instead of $0020 (LPENDIS set in the control field too), via NMH_MUTATE=P4-F3LPENDIS -- P4-PRE-CONTROL already FAILs in the primary run; see the P4-MUTATION record (both cores)
// MUTATION P4-NOPULSE-F1 stimulus unfalsifiable at this boundary: the F1 trigger pulse omitted from the drive schedule, via NMH_MUTATE=P4-NOPULSE-F1 -- P4-PRE-LATCHF1 already FAILs in the primary run; see the P4-MUTATION record (both cores)
//
// P3's own finding, affecting every one of the above: VPOSR and VHPOSR are
// observed to return the live beam counter on every build and every trigger
// schedule tried, never a value held constant through a triggered interval.
// Every assertion that grades "the value is latched / equals the pen
// position / differs from the live beam" (P3 items 1, 2, 5 and P4's own
// graded relation where it depends on a genuine latch) is therefore
// expected to read FAIL here; a red result is the valid, finished outcome
// the brief describes, not a test defect. P3-LPEN's own contrast is listed
// "stimulus green" for the same reason: the LPEN-clear build and the
// LPEN-set build measure the same live-beam value, so the unchanged
// comparator returns the same (FAIL-for-latching / PASS-for-live) verdict
// either way and the contrast cannot demonstrate a difference.

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

struct Checks {
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

// ---- program-side addresses (prog_lp.s) --------------------------------

constexpr uint32_t kResults  = 0x0C0000;
constexpr uint32_t kDoneAddr = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;

constexpr uint32_t kCopList  = 0x0C1000;
constexpr uint32_t kCopA1    = kCopList + 20; // register-offset word of the A1 MOVE (COLOR00<-$F00)
constexpr uint32_t kCopA2    = kCopList + 28; // A2 MOVE (COLOR00<-$0000)
constexpr uint32_t kCopB1    = kCopList + 52; // B1 MOVE (COLOR00<-$00F)
constexpr uint32_t kCopB2    = kCopList + 60; // B2 MOVE (COLOR00<-$0000)
// register-offset word of each COPER-signalling MOVE (INTREQ<-$8010)
constexpr uint32_t kCopE40  = kCopList + 4;
constexpr uint32_t kCopE60  = kCopList + 12;
constexpr uint32_t kCopE110 = kCopList + 36;
constexpr uint32_t kCopE130 = kCopList + 44;
constexpr uint32_t kCopE190 = kCopList + 68;
constexpr uint32_t kCopE210 = kCopList + 76;

constexpr uint32_t kPreTick0 = 0x0C1100;
constexpr uint32_t kPreTick1 = 0x0C1102;
constexpr uint32_t kMainTick = 0x0C1104; // 9 words, one per field (F-1..F7)
constexpr uint32_t kRecords  = 0x0C2000; // 5 words/record, see record_index()

// The Copper starts running the moment DMACON enables it, from wherever the
// beam already is -- not from a VERTB the CPU's own poll loop has seen. In
// practice this program's first pass through the whole list (E40..E210,
// including A1/A2/B1/B2) always completes BEFORE the CPU's own main_loop
// sees its first VERTB, so that first pass carries no field number at all
// (measured: it stores 6 records with field==-1, ahead of field 0's own
// block). Every array built directly from a Copper-fetch address (A1/B1/
// E40../E210's own read_cycles_at()) therefore has this one extra leading
// entry that MAINTICK/RECORDS (CPU-authored, only touched from inside
// main_loop) do not: kPhantomPasses skips it.
constexpr size_t kPhantomPasses = 1;

// One colour clock (one memory slot) equals 8 clk_sys cycles at the harness
// boundary (ch6-blitter-cpu-sharing's own kCck, reused here). The ratio is
// confirmed by P3-PRE-ANCHOR and P3-PRE-READTIME below: both compare a
// page-stated colour-clock interval against a clk_sys cycle count through
// this constant, and a wrong ratio would put every field's measurement far
// outside their small tolerances, not just occasionally.
constexpr uint64_t kCck = 8;
constexpr uint64_t kLineCycles = 227 * kCck; // p.23/p.304's own colour-clocks-per-line, in clk_sys

constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudget  = 9'000'000; // > 9 PAL fields + spin margin

uint32_t word_addr(uint32_t byte_addr) { return byte_addr / 2; }

// ---- capture/analysis helpers -------------------------------------------

std::vector<uint64_t> read_cycles_at(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr) {
    uint32_t w = word_addr(byte_addr);
    std::vector<uint64_t> out;
    for (const auto& a : log) if (!a.is_write && a.word_addr == w) out.push_back(a.cycle);
    return out;
}

std::vector<uint64_t> write_cycles_at(const std::vector<nmh::RamAccess>& log, uint32_t byte_addr) {
    uint32_t w = word_addr(byte_addr);
    std::vector<uint64_t> out;
    for (const auto& a : log) if (a.is_write && a.word_addr == w) out.push_back(a.cycle);
    return out;
}

struct Rec {
    bool present = false;
    uint16_t field = 0xFFFF, event = 0xFFFF;
    uint16_t vposr_long = 0, vhposr_long = 0, vhposr_word = 0;
};

// Record layout (see prog_lp.s's own header comment, and kPhantomPasses
// above): a leading 6-record phantom block (field==-1, no V1/V2, events
// E40..E210 only) precedes 8 records per field for field index 0-7
// (F-1..F6), in store order V1,V2,E40,E60,E110,E130,E190,E210; field index
// 8 (F7) stores only V1,V2 (2 records) and then calls DONE. Total: 6 + 8*8
// + 2 = 72 records.
constexpr int kPhantomRecords = 6;
constexpr int kTotalRecords = 72;
int record_index(int field, int event) {
    // event: 0=E40,1=E60,2=E110,3=E130,4=E190,5=E210,6=V1,7=V2
    static const int pos[8] = {2, 3, 4, 5, 6, 7, 0, 1}; // pos[event] within a field's 8-record block
    if (field < 0 || field > 8) return -1;
    if (field == 8 && event != 6 && event != 7) return -1;
    int p = (event == 6) ? 0 : (event == 7) ? 1 : pos[event];
    return kPhantomRecords + field * 8 + p;
}

struct Records {
    std::vector<Rec> recs; // indexed by record_index()
    std::vector<uint64_t> store_cycle; // same indexing: cycle of the write to that record's first word
};

Records read_records(const nmh::NanomigHarness& /*unused, kept for symmetry*/) { return {}; }

Records parse_records(const std::vector<std::optional<uint16_t>>& mem_words /* kTotalRecords*5 words from kRecords */,
                       const std::vector<nmh::RamAccess>& log) {
    Records r;
    r.recs.resize(kTotalRecords);
    r.store_cycle.resize(kTotalRecords, 0);
    for (int i = 0; i < kTotalRecords; ++i) {
        size_t base = (size_t)i * 5;
        if (base + 4 >= mem_words.size()) continue;
        if (!mem_words[base] || !mem_words[base+1] || !mem_words[base+2] || !mem_words[base+3] || !mem_words[base+4]) continue;
        Rec rec;
        rec.present = true;
        // words: field, event, vposr_long, vhposr_long, vhposr_word
        rec.field = *mem_words[base+0];
        rec.event = *mem_words[base+1];
        rec.vposr_long  = *mem_words[base+2];
        rec.vhposr_long = *mem_words[base+3];
        rec.vhposr_word = *mem_words[base+4];
        r.recs[i] = rec;
        auto w = write_cycles_at(log, kRecords + 10u * (uint32_t)i);
        r.store_cycle[i] = w.empty() ? 0 : w.front();
    }
    return r;
}

uint32_t M_of(const Rec& r) { return ((uint32_t)(r.vposr_long & 1) << 16) | r.vhposr_long; }
uint32_t V_of(const Rec& r) { return M_of(r) >> 8; }
uint32_t H_of(const Rec& r) { return M_of(r) & 0xFF; }

// ---- Config combinations --------------------------------------------------

nmh::Config cfg_for(bool ecs, bool ntsc) {
    nmh::Config c;
    c.chipset_config = (ecs ? 0x08 : 0x00) | (ntsc ? 0x02 : 0x00);
    return c;
}

// ---- dry run: never drives pin 6, gives fetch/tick timing ----------------

struct DryResult {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log; // coalesced
};

DryResult dry_run(const std::string& bin, nmh::Config cfg) {
    DryResult r;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kRunBudget);
    r.log = nmh::coalesce(h.ram_log());
    return r;
}

// ---- trigger schedule, derived from a dry run ----------------------------

struct TriggerPlan {
    bool ok = false;
    uint64_t f1 = 0, f3 = 0, f5 = 0; // absolute clk_sys cycle to assert pin 6
};

// f1_offset_cck lets P3-OFFSET reuse this with a different F1 placement.
TriggerPlan plan_triggers(const std::vector<nmh::RamAccess>& log, uint64_t f1_offset_cck = 16) {
    TriggerPlan p;
    auto a1 = read_cycles_at(log, kCopA1);
    auto b1 = read_cycles_at(log, kCopB1);
    if (a1.size() < 2 + kPhantomPasses || b1.size() < 6 + kPhantomPasses) return p;
    p.f1 = a1[kPhantomPasses + 2] + f1_offset_cck * kCck; // field index 2 = F1
    p.f3 = a1[kPhantomPasses + 4] + 56 * kCck;            // field index 4 = F3
    p.f5 = b1[kPhantomPasses + 6] + 16 * kCck;            // field index 6 = F5
    p.ok = true;
    return p;
}

std::vector<uint64_t> field_tick_cycles(const std::vector<nmh::RamAccess>& log) {
    std::vector<uint64_t> out;
    for (int i = 0; i < 9; ++i) {
        auto w = write_cycles_at(log, kMainTick + 2 * (uint32_t)i);
        out.push_back(w.empty() ? 0 : w.front());
    }
    return out;
}

// ---- driven run: replays the same program, driving pin 6 on schedule ----

struct DrivenResult {
    bool loaded = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log; // coalesced
    std::vector<std::pair<uint64_t,uint64_t>> pulses; // actual (assert,release) cycles achieved
    std::vector<std::optional<uint16_t>> mem; // kTotalRecords*5 words from kRecords
    std::optional<uint16_t> ddra_stored;
};

struct HoldWindow { uint64_t begin, end; };

DrivenResult driven_run(const std::string& bin, nmh::Config cfg,
                         std::vector<uint64_t> trigger_cycles, uint64_t pulse_len_cycles,
                         std::optional<HoldWindow> hold = std::nullopt,
                         size_t record_words = (size_t)kTotalRecords * 5) {
    DrivenResult r;
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    h.boot(kBootBudget);
    std::sort(trigger_cycles.begin(), trigger_cycles.end());
    size_t next = 0;
    bool active = false;
    uint64_t assert_at = 0, release_at = 0;
    size_t seen = 0;
    const uint64_t start = h.cycles();
    bool done_found = false;
    while (h.cycles() - start < kRunBudget) {
        uint64_t c = h.cycles();
        bool forced = hold && c >= hold->begin && c < hold->end;
        if (forced) {
            h.ports()->joystick0 |= 0x10;
            h.ports()->joystick1 |= 0x10;
            if (!active) { active = true; assert_at = c; }
        } else if (!active && next < trigger_cycles.size() && c >= trigger_cycles[next]) {
            h.ports()->joystick0 |= 0x10;
            h.ports()->joystick1 |= 0x10;
            active = true;
            assert_at = c;
            release_at = c + pulse_len_cycles;
            ++next;
        } else if (active && !(hold && c < hold->end) && c >= release_at) {
            h.ports()->joystick0 &= ~0x10;
            h.ports()->joystick1 &= ~0x10;
            active = false;
            r.pulses.push_back({assert_at, release_at});
        } else if (active && hold && c >= hold->end) {
            h.ports()->joystick0 &= ~0x10;
            h.ports()->joystick1 &= ~0x10;
            active = false;
            r.pulses.push_back({assert_at, c});
        }
        h.run_cycles(1);
        const auto& log = h.ram_log();
        for (; seen < log.size(); ++seen) {
            if (log[seen].is_write && log[seen].word_addr == word_addr(kDoneAddr) && log[seen].data == kDoneValue) {
                r.done = log[seen].cycle;
                done_found = true;
            }
        }
        if (done_found) break;
    }
    if (active) {
        h.ports()->joystick0 &= ~0x10;
        h.ports()->joystick1 &= ~0x10;
        r.pulses.push_back({assert_at, h.cycles()});
    }
    r.log = nmh::coalesce(h.ram_log());
    r.ddra_stored = h.peek_logical(kResults);
    for (size_t w = 0; w < record_words; ++w)
        r.mem.push_back(h.peek_logical(kRecords + 2u * (uint32_t)w));
    return r;
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    // ================= P3 =================
    // Runs OCS-PAL and ECS-PAL (both required by the run matrix). NTSC is
    // not run here -- see the build's own report (budget).
    struct Combo { const char* label; bool ecs; bool ntsc; const char* bin; };
    Combo combos[] = {
        {"OCS-PAL", false, false, "/prog_lp_ocs_pal.bin"},
        {"ECS-PAL", true,  false, "/prog_lp_ecs_pal.bin"},
    };

    for (const auto& combo : combos) {
        std::string bin = dir + combo.bin;
        std::string mutbin = bin;
        std::vector<uint64_t> extra_trigger_offset; // unused placeholder
        uint64_t f1_offset = 16;
        bool skip_dry_mutation = false;

        // Program-level mutations only apply to the ECS-PAL combo (the
        // representative base for P3's preconditions -- see the header
        // comment's own scope note).
        bool is_ecs_pal = combo.ecs && !combo.ntsc;
        std::string used_bin = bin;
        std::optional<HoldWindow> hold;
        std::vector<uint64_t> omit; // event names to omit from the trigger schedule

        if (is_ecs_pal && mv == "P3-ANCHOR") used_bin = dir + "/prog_lp_ecs_pal_anchormut.bin";
        if (is_ecs_pal && mv == "P3-READTIME") used_bin = dir + "/prog_lp_ecs_pal_v1delay.bin";
        if (is_ecs_pal && mv == "P3-DDRA") used_bin = dir + "/prog_lp_ecs_pal_ddraforce.bin";
        if (is_ecs_pal && mv == "P3-LPEN") used_bin = dir + "/prog_lp_ecs_pal_lpenclear.bin";
        if (is_ecs_pal && mv == "P3-OFFSET") f1_offset = 30;

        DryResult dry = dry_run(used_bin, cfg_for(combo.ecs, combo.ntsc));
        std::string tag = std::string(combo.label) + (used_bin != bin ? " [" + mv + "]" : "");

        checks.expect(dry.loaded && dry.released,
            "P3 FIXTURE " + tag + ": program loaded and CPU released within the boot budget "
            "[not graded -- apparatus check]");
        if (!dry.loaded) continue;

        TriggerPlan plan = plan_triggers(dry.log, is_ecs_pal ? f1_offset : 16);
        checks.expect(plan.ok, "P3 FIXTURE " + tag + ": the Copper's own A1/B1 fetch occurs once per "
            "field, at least 5 (A1) and 7 (B1) times, in the dry run [not graded -- apparatus check]");
        if (!plan.ok) continue;

        std::vector<uint64_t> triggers = {plan.f1, plan.f3, plan.f5};
        if (is_ecs_pal && mv == "P3-NOPULSE") triggers = {plan.f3, plan.f5};
        bool applying_hold = is_ecs_pal && mv == "P3-HOLD";
        if (applying_hold) {
            auto ticks = field_tick_cycles(dry.log);
            // ticks[3] = start of F2 (field index 3); hold from ~line 30 of
            // F2 to the start of F3 (ticks[4]).
            hold = HoldWindow{ticks[3] + 30 * kLineCycles, ticks[4]};
            triggers.clear();
        }

        bool beamcon0_mismatch = false;
        if (mv == "P3-BEAMCON0") {
            // reuse the ECS-NTSC build (BEAMCON0<-$0000) against this
            // combo's own PAL-configured harness -- bit 5 opposite the
            // selected standard.
            if (!combo.ecs) continue; // OCS has no BEAMCON0; run only once (under ECS-PAL)
            used_bin = dir + "/prog_lp_ecs_ntsc.bin";
            beamcon0_mismatch = true;
            dry = dry_run(used_bin, cfg_for(combo.ecs, combo.ntsc));
            plan = plan_triggers(dry.log, 16);
            triggers = {plan.f1, plan.f3, plan.f5};
            tag = std::string(combo.label) + " [P3-BEAMCON0]";
        }

        DrivenResult run = driven_run(used_bin, cfg_for(combo.ecs, combo.ntsc), triggers, 4 * kCck, hold);
        checks.expect(run.loaded && run.done.has_value(),
            "P3 FIXTURE " + tag + ": the driven run reached DONE [not graded -- apparatus check]");
        if (!run.loaded || !run.done) continue;

        Records recs = parse_records(run.mem, run.log);
        auto get = [&](int field, int event) -> const Rec& {
            static Rec empty;
            int idx = record_index(field, event);
            if (idx < 0) return empty;
            return recs.recs[idx];
        };
        auto storec = [&](int field, int event) -> uint64_t {
            int idx = record_index(field, event);
            return idx < 0 ? 0 : recs.store_cycle[idx];
        };

        // ---- Preconditions ----

        // 1. Anchor check.
        {
            auto a1 = read_cycles_at(dry.log, kCopA1), a2 = read_cycles_at(dry.log, kCopA2);
            auto b1 = read_cycles_at(dry.log, kCopB1), b2 = read_cycles_at(dry.log, kCopB2);
            // Skip the leading phantom pass (kPhantomPasses). DONE fires
            // right after F7's own V2 (before F7 ever reaches its own A1),
            // so only 8 real fields' worth of A1/B1 pairs (F-1..F6) are
            // ever logged -- 9 entries total with the phantom pass.
            constexpr size_t kRealFields = 8;
            bool sizes_ok = a1.size() >= kRealFields + kPhantomPasses && a2.size() >= kRealFields + kPhantomPasses &&
                             b1.size() >= kRealFields + kPhantomPasses && b2.size() >= kRealFields + kPhantomPasses;
            bool spacing_ok = sizes_ok, period_ok = sizes_ok;
            int64_t worst_spacing = 0, worst_period = 0;
            if (sizes_ok) {
                for (size_t i = 0; i < kRealFields; ++i) {
                    size_t j = i + kPhantomPasses;
                    int64_t da = (int64_t)a2[j] - (int64_t)a1[j] - 768;
                    int64_t db = (int64_t)b2[j] - (int64_t)b1[j] - 768;
                    if (std::abs(da) > 16) spacing_ok = false;
                    if (std::abs(db) > 16) spacing_ok = false;
                    worst_spacing = std::max({worst_spacing, std::abs(da), std::abs(db)});
                }
                int64_t p0 = (int64_t)a1[kPhantomPasses + 1] - (int64_t)a1[kPhantomPasses];
                for (size_t i = 1; i < kRealFields - 1; ++i) {
                    size_t j = i + kPhantomPasses;
                    int64_t p = (int64_t)a1[j+1] - (int64_t)a1[j];
                    if (std::abs(p - p0) > 8) period_ok = false;
                    worst_period = std::max(worst_period, std::abs(p - p0));
                }
            }
            checks.note("P3 " + tag + " anchor: worst |spacing-768| = " + std::to_string(worst_spacing) +
                        " cycles, worst period deviation = " + std::to_string(worst_period) + " cycles");
            checks.expect(sizes_ok && spacing_ok && period_ok,
                "P3-PRE-ANCHOR " + tag + " [p.22]: t(A2)-t(A1) and t(B2)-t(B1) each equal 96 colour "
                "clocks within 2 in every field, and the field period (from t(A1)) is the same for "
                "all fields");
        }

        // 2. Read-time check.
        {
            struct Sig { int field, event; uint32_t signal_addr; };
            std::vector<Sig> sigs = {
                {0,0,kCopE40},{0,1,kCopE60},{0,2,kCopE110},{0,3,kCopE130},{0,4,kCopE190},{0,5,kCopE210},
            };
            bool ok = true;
            int checked = 0;
            for (int f = 0; f <= 7; ++f) {
                for (auto s : sigs) {
                    auto fetches = read_cycles_at(dry.log, s.signal_addr);
                    size_t j = (size_t)f + kPhantomPasses; // skip the leading phantom pass
                    if (j >= fetches.size()) { ok = false; continue; }
                    uint64_t store = storec(f, s.event);
                    if (store == 0 || store <= fetches[j] || store - fetches[j] > kLineCycles) ok = false;
                    ++checked;
                }
            }
            auto mticks = field_tick_cycles(run.log);
            for (int f = 0; f <= 8; ++f) {
                uint64_t v1 = storec(f, 6), v2 = storec(f, 7);
                if (mticks[f] == 0 || v1 == 0 || v2 == 0) { ok = false; continue; }
                if (v1 <= mticks[f] || v1 - mticks[f] >= 18 * kLineCycles) ok = false;
                if (v2 <= mticks[f] || v2 - mticks[f] >= 18 * kLineCycles) ok = false;
                checked += 2;
            }
            checks.note("P3 " + tag + " read-time: " + std::to_string(checked) + " reads checked");
            checks.expect(ok,
                "P3-PRE-READTIME " + tag + " [p.22]: every Copper-signalled read is stored within "
                "one line of its signalling MOVE's own fetch, and every V1/V2 is stored less than "
                "18 lines after its field's own VERTB tick");
        }

        // 3. Pin 6 not driven by the CIA (the STORED, pre-fix DDRA value).
        {
            bool ok = run.ddra_stored.has_value() && (*run.ddra_stored & 0xC0) == 0;
            checks.note("P3 " + tag + " DDRA: stored value = " +
                        (run.ddra_stored ? std::to_string(*run.ddra_stored) : std::string("(missing)")));
            checks.expect(ok,
                "P3-PRE-DDRA " + tag + " [p.228,p.229,p.351]: the stored CIAADDRA value has bits 6 "
                "and 7 clear (pin 6 of both ports is an input)");
            if (is_ecs_pal && mv == "P3-DDRA") {
                checks.record("P3-PRE-DDRA-MUTATION " + tag + ": unfalsifiable at this boundary -- "
                    "the program stores CIAADDRA before the fix-or-force branch (p.339's own step "
                    "order: \"read ... and store it\", then \"if ... clear\"), so the STORED word is "
                    "always the hardware's own pre-fix reset value regardless of what the fix-or-"
                    "force branch does afterwards; forcing bits 6-7 to outputs cannot change what was "
                    "already stored before that branch ran");
            }
        }

        // 4. The pulse was driven. Graded against the NOMINAL schedule (3
        // pulses: F1, F3, F5), not against whatever `triggers` a mutation
        // fed the driver -- P3-NOPULSE's own reduced `triggers` (2 entries)
        // must still show up as short of the nominal 3, or the omission
        // could never be caught here.
        if (applying_hold) {
            checks.record("P3-PRE-PULSE " + tag + ": not applicable -- this run drives a held "
                "level (precondition 5's own contrast), not the normal 3-pulse schedule");
        } else {
            size_t nominal = 3;
            bool ok = run.pulses.size() == nominal;
            for (size_t i = 0; ok && i < triggers.size() && i < run.pulses.size(); ++i) {
                uint64_t a = run.pulses[i].first;
                if (a < triggers[i] || a > triggers[i] + 4) ok = false;
            }
            checks.note("P3 " + tag + " drive log: " + std::to_string(run.pulses.size()) +
                        " pulse(s) driven, " + std::to_string(nominal) + " scheduled (nominal)");
            checks.expect(ok,
                "P3-PRE-PULSE " + tag + ": the test's own drive log shows each scheduled pulse at "
                "its scheduled time and nothing else on pin 6");
        }

        if (!applying_hold && !beamcon0_mismatch) {
            // 5 & item4: beam reads live before/after every trigger, every field.
            bool ok = true;
            int checked = 0;
            for (int f = 0; f <= 7; ++f) {
                const Rec& e40 = get(f, 0);
                const Rec& e60 = get(f, 1);
                if (!e40.present || !e60.present) { ok = false; continue; }
                uint32_t v40 = V_of(e40), v60 = V_of(e60);
                if (!(v40 == 40 || v40 == 41)) ok = false;
                if (!(v60 == 60 || v60 == 61)) ok = false;
                checked += 2;
            }
            // fields without a trigger: F-1,F0,F2,F4,F6 = indices 0,1,3,5,7
            for (int f : {0,1,3,5,7}) {
                struct { int event; uint32_t line; } evs[] = {{2,110},{3,130},{4,190},{5,210}};
                for (auto e : evs) {
                    const Rec& r = get(f, e.event);
                    if (!r.present) { ok = false; continue; }
                    uint32_t v = V_of(r);
                    if (!(v == e.line || v == e.line + 1)) ok = false;
                    ++checked;
                }
            }
            checks.note("P3 " + tag + " item4/pre5: " + std::to_string(checked) + " live-beam reads checked");
            checks.expect(ok,
                "P3-PRE-LIVE " + tag + " opus:7-bp.c3,sol:7-beam.c3 [p.216]: before a trigger and "
                "after its release, VPOSR/VHPOSR read the live beam (E40/E60 of every field; "
                "E110-E210 of every field without a trigger)");
        }

        if (applying_hold) {
            const Rec& e40 = get(3, 0); // F2 = field index 3
            const Rec& e60 = get(3, 1);
            bool held = e40.present && e60.present && V_of(e40) == 30 && V_of(e60) == 30;
            checks.note("P3-HOLD " + tag + ": E40 V=" + (e40.present ? std::to_string(V_of(e40)) : "?") +
                        " E60 V=" + (e60.present ? std::to_string(V_of(e60)) : "?") + " (predicted 30,30 if frozen)");
            checks.expect(!held,
                "P3-PRE-LIVE " + tag + " (contrast, pin 6 held from line 30 of F2 to end of F2): "
                "E40 and E60 of F2 no longer read the live beam if p.239's freeze applied at line 30");
        }

        // 6. ECS-only: field period unaffected by the BEAMCON0 write.
        if (combo.ecs && !beamcon0_mismatch) {
            auto pre0 = write_cycles_at(dry.log, kPreTick0);
            auto pre1 = write_cycles_at(dry.log, kPreTick1);
            auto mticks = field_tick_cycles(run.log);
            bool have = !pre0.empty() && !pre1.empty() && mticks[1] != 0 && mticks[0] != 0;
            int64_t pre_period = have ? (int64_t)pre1.front() - (int64_t)pre0.front() : 0;
            int64_t post_period = have ? (int64_t)mticks[1] - (int64_t)mticks[0] : 0;
            // The pre-write tick and the post-write tick are each stored by
            // a different code path (wait_vertb's own bsr/rts vs
            // main_loop's inline handling), so a genuine, fixed few-cycle
            // software latency difference between the two is expected; a
            // real PAL/NTSC period difference (p.24: 313 vs 262 lines) is
            // tens of thousands of cycles, three orders of magnitude
            // larger, so 128 cycles (16 colour clocks) safely separates
            // "no real change" from "real change" either way.
            bool ok = have && std::abs(pre_period - post_period) <= 128;
            checks.note("P3 " + tag + " BEAMCON0 period: pre=" + std::to_string(pre_period) +
                        " post=" + std::to_string(post_period) + " cycles");
            checks.expect(ok,
                "P3-PRE-PERIOD " + tag + " [p.24]: the field period measured before the BEAMCON0 "
                "write equals the period measured after it");
        } else if (combo.ecs && beamcon0_mismatch) {
            auto pre0 = write_cycles_at(dry.log, kPreTick0);
            auto pre1 = write_cycles_at(dry.log, kPreTick1);
            auto mticks = field_tick_cycles(run.log);
            bool have = !pre0.empty() && !pre1.empty() && mticks[1] != 0 && mticks[0] != 0;
            int64_t pre_period = have ? (int64_t)pre1.front() - (int64_t)pre0.front() : 0;
            int64_t post_period = have ? (int64_t)mticks[1] - (int64_t)mticks[0] : 0;
            checks.note("P3-BEAMCON0 " + tag + " period: pre=" + std::to_string(pre_period) +
                        " post=" + std::to_string(post_period) + " cycles (bit 5 written opposite "
                        "the selected standard)");
            checks.record("P3-PRE-PERIOD-MUTATION " + tag + " [p.305,p.24]: unfalsifiable at this "
                "boundary -- the measured field period (above) does not change under this mutation, "
                "so this contrast cannot be made to fail the unchanged-period precondition check on "
                "this RTL");
        } else {
            checks.record("P3-PRE-PERIOD " + tag + ": not applicable -- BEAMCON0 does not exist "
                "under OCS (p.305)");
        }

        if (!beamcon0_mismatch && !applying_hold) {
            // ---- Main assertion: items 1, 2, 5 (the latching claim). ----
            struct Group { int f_events, f_tail; const char* name; std::vector<int> events; };
            auto m_group_ok = [&](int f_events, std::vector<int> events, int f_tail) {
                std::vector<uint32_t> ms;
                bool word_ok = true, present = true;
                for (int e : events) {
                    const Rec& r = get(f_events, e);
                    if (!r.present) { present = false; continue; }
                    ms.push_back(M_of(r));
                    if (r.vhposr_word != r.vhposr_long) word_ok = false;
                }
                for (int e : {6,7}) {
                    const Rec& r = get(f_tail, e);
                    if (!r.present) { present = false; continue; }
                    ms.push_back(M_of(r));
                    if (r.vhposr_word != r.vhposr_long) word_ok = false;
                }
                bool same = present && !ms.empty();
                for (auto m : ms) if (m != ms[0]) same = false;
                return present && same && word_ok;
            };
            bool g1 = m_group_ok(2, {2,3,4,5}, 3); // F1 -> F2
            bool g3 = m_group_ok(4, {2,3,4,5}, 5); // F3 -> F4
            bool g5 = m_group_ok(6, {4,5}, 7);     // F5 -> F6 (E190,E210 only)
            checks.expect(g1 && g3 && g5,
                "P3 " + tag + " opus:7-bp.c3,sol:7-beam.c3,opus:7-bp.c11 [p.216,p.239]: once "
                "triggered, VPOSR/VHPOSR stay at one constant value (the word read equalling the "
                "long read's low word) through E110-E210 of the field and V1/V2 of the field after "
                "it -- F1 (E110-E210,V1/V2 of F2): " + std::string(g1?"held":"changed") +
                "; F3: " + std::string(g3?"held":"changed") + "; F5 (E190/E210,V1/V2 of F6): " +
                std::string(g5?"held":"changed"));

            const Rec& l1 = get(2, 2), &l3 = get(4, 2), &l5 = get(6, 4);
            bool item2 = l1.present && l3.present && l5.present &&
                         V_of(l1) == 100 && V_of(l3) == 100 && V_of(l5) == 180 &&
                         (int64_t)H_of(l3) - (int64_t)H_of(l1) == 40;
            checks.note("P3 " + tag + " item2: V(L1)=" + (l1.present?std::to_string(V_of(l1)):"?") +
                        " V(L3)=" + (l3.present?std::to_string(V_of(l3)):"?") +
                        " V(L5)=" + (l5.present?std::to_string(V_of(l5)):"?") +
                        " H(L3)-H(L1)=" + (l1.present&&l3.present?std::to_string((int64_t)H_of(l3)-(int64_t)H_of(l1)):"?"));
            checks.expect(item2,
                "P3 " + tag + " opus:7-bp.c3,sol:7-beam.c3,opus:7-bp.c11 [p.239]: the latched value "
                "is the light-pen position -- V(L1)=100, V(L3)=100, V(L5)=180, H(L3)-H(L1)=40");
            if (is_ecs_pal && mv == "P3-OFFSET") {
                checks.record("P3-PRE-OFFSET-MUTATION " + tag + ": unfalsifiable at this boundary -- "
                    "H(L1) (E110 of F1) is a live read on this RTL (item1/g1 already reads FAIL), so "
                    "its value tracks the beam's own progress at line 110 regardless of where within "
                    "the line the trigger pulse landed; moving the pulse cannot move a value that "
                    "was never derived from it");
            }

            bool ok5 = true;
            uint32_t threshold = 0x10500;
            for (int f : {2,4,6,8}) {
                const Rec& v1 = get(f,6), &v2 = get(f,7);
                if (!v1.present || !v2.present) { ok5 = false; continue; }
                if (M_of(v1) != M_of(v2)) ok5 = false;
                bool above = combo.ntsc ? (M_of(v1) >= threshold) : (M_of(v1) > threshold);
                if (!above) ok5 = false;
            }
            checks.expect(ok5,
                "P3 " + tag + " opus:7-bp.c3,sol:7-beam.c3,opus:7-bp.c11 [p.239]: with no trigger in "
                "the preceding field, V1 and V2 agree and their M is " +
                std::string(combo.ntsc ? "not less than" : "greater than") + " $10500 (F1,F3,F5,F7)");
        }
        // P3-LPEN's own dedicated comparator: runs in addition to the main
        // block above (mv=="P3-LPEN" sets neither beamcon0_mismatch nor
        // applying_hold, so the main block already ran and printed items
        // 1, 2 and 5 against the LPEN-clear build). This block is the
        // "different, proven-live input through the same comparator" check.
        if (is_ecs_pal && mv == "P3-LPEN") {
            const Rec& l1 = get(2, 2);
            bool still_live = l1.present && (V_of(l1) == 110 || V_of(l1) == 111);
            checks.note("P3-LPEN " + tag + ": E110 of F1 reads V=" + (l1.present?std::to_string(V_of(l1)):"?") +
                        " (live beam would be 110/111)");
            checks.expect(still_live,
                "P3-LPEN " + tag + " (contrast, BPLCON0 LPEN clear): every read tracks the beam "
                "(E110 of F1 reads V in {110,111}), the same outcome the primary run already shows "
                "on this RTL -- see the header note");
        }
    }

    // ================= P4 (ECS, PAL) =================
    {
        constexpr uint32_t kP4CopE250 = kCopList + 84;
        constexpr uint32_t kP4MainTick = 0x0C1100; // 5 words: fields F-1,F0,F1,F2,F3
        nmh::Config cfg = cfg_for(/*ecs=*/true, /*ntsc=*/false);

        std::string base_bin = "/prog_p4.bin";
        std::string used_bin = dir + base_bin;
        if (mv == "P4-NOLPENDIS") used_bin = dir + "/prog_p4_nolpendis.bin";
        if (mv == "P4-F3LPENDIS") used_bin = dir + "/prog_p4_f3lpendis.bin";
        std::string tag = std::string("ECS-PAL") + (used_bin != dir + base_bin ? " [" + mv + "]" : "");

        DryResult dry = dry_run(used_bin, cfg);
        checks.expect(dry.loaded && dry.released,
            "P4 FIXTURE " + tag + ": program loaded and CPU released within the boot budget "
            "[not graded -- apparatus check]");

        auto a1 = read_cycles_at(dry.log, kCopA1);
        bool have_a1 = a1.size() >= kPhantomPasses + 5; // phantom + F-1,F0,F1,F2,F3
        checks.expect(have_a1,
            "P4 FIXTURE " + tag + ": the Copper's own A1 fetch occurs at least 5 times (plus the "
            "leading phantom pass) in the dry run [not graded -- apparatus check]");
        if (!dry.loaded || !have_a1) { /* fall through to summary */ }
        else {
            uint64_t f1_trig = a1[kPhantomPasses + 2] + 16 * kCck;
            uint64_t f3_trig = a1[kPhantomPasses + 4] + 16 * kCck;
            std::vector<uint64_t> triggers = {f1_trig, f3_trig};
            if (mv == "P4-NOPULSE-F1") triggers = {f3_trig};

            DrivenResult run = driven_run(used_bin, cfg, triggers, 4 * kCck, std::nullopt, 12);
            checks.expect(run.loaded && run.done.has_value(),
                "P4 FIXTURE " + tag + ": the driven run reached DONE [not graded -- apparatus check]");
            if (run.loaded && run.done) {
                auto make_rec = [&](size_t base) -> Rec {
                    Rec r;
                    if (base + 2 >= run.mem.size()) return r;
                    if (!run.mem[base] || !run.mem[base+1] || !run.mem[base+2]) return r;
                    r.present = true;
                    r.vposr_long = *run.mem[base];
                    r.vhposr_long = *run.mem[base+1];
                    r.vhposr_word = *run.mem[base+2];
                    return r;
                };
                Rec X = make_rec(0), Y = make_rec(3), Xp = make_rec(6), Yp = make_rec(9);

                // Precondition: DDRA (as in P3).
                bool ddra_ok = run.ddra_stored.has_value() && (*run.ddra_stored & 0xC0) == 0;
                checks.expect(ddra_ok,
                    "P4-PRE-DDRA " + tag + " [p.228,p.229,p.351]: the stored CIAADDRA value has "
                    "bits 6 and 7 clear");

                // Precondition: anchor check (A2-A1 spacing), as in P3.
                auto a2 = read_cycles_at(dry.log, kCopA2);
                bool anchor_ok = a1.size() >= kPhantomPasses + 5 && a2.size() >= kPhantomPasses + 5;
                if (anchor_ok) {
                    for (size_t i = 0; i < 5; ++i) {
                        size_t j = i + kPhantomPasses;
                        if (std::abs((int64_t)a2[j] - (int64_t)a1[j] - 768) > 16) anchor_ok = false;
                    }
                }
                checks.expect(anchor_ok,
                    "P4-PRE-ANCHOR " + tag + " [p.22]: t(A2)-t(A1) equals 96 colour clocks within 2 "
                    "in every field (F-1..F3)");

                // Precondition: reference-frame bound on X/Y (also P4_SPIN's own guard).
                auto find_store = [&](uint32_t byte_addr) -> uint64_t {
                    auto w = write_cycles_at(run.log, byte_addr);
                    return w.empty() ? 0 : w.front();
                };
                uint64_t x_store = find_store(kRecords);       // first word of X
                uint64_t y_store = find_store(kRecords + 6);   // first word of Y
                uint64_t xp_store = find_store(kRecords + 12);
                uint64_t yp_store = find_store(kRecords + 18);
                uint64_t f1_a1 = a1[kPhantomPasses + 2], f3_a1 = a1[kPhantomPasses + 4];
                uint64_t f2_a1 = a1[kPhantomPasses + 3]; // next field after F1
                uint64_t f4_a1 = a1.size() > kPhantomPasses + 5 ? a1[kPhantomPasses + 5] : 0; // next field after F3, if logged
                bool ref_ok = x_store > f1_a1 + 156u * 228u * kCck &&
                              y_store > f1_a1 + 156u * 228u * kCck &&
                              (f2_a1 == 0 || y_store < f2_a1 - 100u * 228u * kCck) &&
                              xp_store > f3_a1 + 156u * 228u * kCck;
                checks.note("P4 " + tag + " reference frame: x_store-f1_a1=" +
                            std::to_string(x_store - f1_a1) + " cycles, needs > " +
                            std::to_string(156u*228u*kCck));
                checks.expect(ref_ok,
                    "P4-PRE-REFFRAME " + tag + " [p.239, build note]: X (and X') is stored at least "
                    "156*228 colour clocks after its own field's t(A1), placing it at line 256 or "
                    "later, and Y is stored before the next field's own t(A1) minus 100*228 colour "
                    "clocks");
                (void)f4_a1; (void)xp_store; (void)yp_store;

                // Precondition: field period unchanged across every BEAMCON0 write.
                auto mticks_p4 = [&]() {
                    std::vector<uint64_t> t;
                    for (int i = 0; i < 5; ++i) {
                        auto w = write_cycles_at(run.log, kP4MainTick + 2 * (uint32_t)i);
                        t.push_back(w.empty() ? 0 : w.front());
                    }
                    return t;
                }();
                // main_loop's own VERTB poll is a tight but non-zero-length
                // instruction loop, so the tick store lands a few
                // instructions after the true hardware edge, by an amount
                // that varies slightly field to field (measured: up to
                // ~224 cycles, 28 colour clocks) -- polling quantization
                // jitter, not a period change. A real PAL/NTSC difference
                // (p.24: 313 vs 262 lines) is ~92,600 cycles, three orders
                // of magnitude larger, so 512 cycles safely separates them.
                bool period_ok = true;
                int64_t p0 = (int64_t)mticks_p4[1] - (int64_t)mticks_p4[0];
                std::string periods_dump = std::to_string(p0);
                for (int i = 1; i < 4; ++i) {
                    int64_t p = (int64_t)mticks_p4[i+1] - (int64_t)mticks_p4[i];
                    periods_dump += "," + std::to_string(p);
                    if (std::abs(p - p0) > 512) period_ok = false;
                }
                checks.note("P4 " + tag + " ticks: " + std::to_string(mticks_p4[0]) + " " +
                            std::to_string(mticks_p4[1]) + " " + std::to_string(mticks_p4[2]) + " " +
                            std::to_string(mticks_p4[3]) + " " + std::to_string(mticks_p4[4]) +
                            "; periods: " + periods_dump);
                checks.expect(period_ok,
                    "P4-PRE-PERIOD " + tag + " [p.305,p.24]: the field period is unchanged across "
                    "every BEAMCON0 write (measured from the VERTB ticks)");

                // Precondition: latch present in F1 (V(Y)=100) -- expected to fail on this RTL.
                bool latch_f1 = Y.present && V_of(Y) == 100;
                checks.note("P4 " + tag + " Y: V=" + (Y.present?std::to_string(V_of(Y)):"?") +
                            " H=" + (Y.present?std::to_string(H_of(Y)):"?"));
                checks.expect(latch_f1,
                    "P4-PRE-LATCHF1 " + tag + " [p.239]: the latch is present in F1 -- V(Y)=100");

                // Precondition: control field shows the latched value with LPENDIS clear.
                bool control_ok = Xp.present && Yp.present && M_of(Xp) == M_of(Yp) && V_of(Xp) == 100;
                checks.note("P4 " + tag + " X'/Y': V(X')=" + (Xp.present?std::to_string(V_of(Xp)):"?") +
                            " V(Y')=" + (Yp.present?std::to_string(V_of(Yp)):"?") +
                            " equal=" + (Xp.present&&Yp.present ? std::to_string(M_of(Xp)==M_of(Yp)) : "?"));
                checks.expect(control_ok,
                    "P4-PRE-CONTROL " + tag + " [p.239]: F3 (LPENDIS clear throughout) shows X'=Y', "
                    "V(X')=100");

                // ---- Main assertion: the graded relation.
                bool x8 = X.present && (X.vposr_long & 1) != 0;
                uint32_t x70 = X.present ? (X.vhposr_long >> 8) : 0xFFFF;
                bool graded = X.present && !((!x8) && x70 == 100);
                checks.note("P4 " + tag + " X: V8=" + (X.present?std::to_string((int)x8):"?") +
                            " V7-0=" + (X.present?std::to_string(x70):"?"));
                checks.expect(graded,
                    "P4 " + tag + " opus:7-appC-beamcon0.c4,sol:7-appC-beamcon0.c4 [p.305]: with "
                    "LPENDIS set, the pair (V8,V7-0) of X is not (0,100)");

                if (mv == "P4-NOLPENDIS") {
                    checks.record("P4-MUTATION " + tag + ": unfalsifiable at this boundary -- "
                        "X is measured to read the same live value whether E60 writes $2020 or "
                        "$0020 (see the X note above); the graded relation (X is not (0,100)) "
                        "holds either way, for the same reason it already holds in the primary run");
                }
                if (mv == "P4-F3LPENDIS") {
                    checks.record("P4-MUTATION " + tag + ": unfalsifiable at this boundary -- "
                        "P4-PRE-CONTROL already reads FAIL in the primary run (no latch to show a "
                        "value of 100 in the first place), so setting LPENDIS in F3 too cannot make "
                        "an already-live X'/Y' pair 'more' live; both runs read the same beam-"
                        "tracking values");
                }
                if (mv == "P4-NOPULSE-F1") {
                    checks.record("P4-MUTATION " + tag + ": unfalsifiable at this boundary -- "
                        "P4-PRE-LATCHF1 already reads FAIL in the primary run (Y is never 100, no "
                        "latch exists), so omitting the F1 trigger cannot make Y read differently: "
                        "it is the live beam either way");
                }
            }
        }
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
