// test_ch2_interlace_fields.cpp
//
// Integration package ch2-interlace-fields: see SPECIFICATION.md. Program I
// and Program T each drive the design only through the top-level ports that
// nanomig_harness.h exposes (Config, load_program/boot/run_until_write,
// peek_logical(), ram_log()) and are graded only against what those ports
// showed.
//
// Design note: unlike the shorter ch6 blitter packages, Program I and
// Program T each need to observe many complete video fields (8 PAL fields
// / 13 NTSC-or-PAL fields), so a single run of either program already
// costs many million clk_sys cycles. Re-running the whole program once
// per assertion (as ch6-blitter-geometry does) would multiply that cost
// by the number of assertions. Instead, the *normal* run loads Program I
// once (plus the LACE-clear control I-a) and Program T once, and every
// assertion and precondition below is graded from those shared
// measurements -- ram_log() and peek_logical() are read-only, so sharing
// them cannot let one assertion affect another's verdict.
//
// Mutation runs (NMH_MUTATE=<label>) load the one variant program the label
// calls for, run the label's own assertion function -- the same function,
// with the same label text and the same comparator the normal run uses -- and
// print its line, which must be FAIL. The other preconditions of the run print
// before or after it as "<label>/<id>: ..." lines: PASS where they hold, FAIL
// where they do not, and RECORDED "NOT GRADED" where the run lacks what they
// read or they do not apply. The lines downstream of the preconditions (P1,
// P1-NOLACE, P4a to P4e, P5, P6, P5-NOREWRITE, P6-NOREWRITE, P2, P3) print
// through their normal gates, so a mutation that closes a gate shows the line
// NOT GRADED. The "<label>/..." lines are not assertions of the package. A
// precondition that fails in a mutation run turns the labelled line into NOT
// GRADED, never PASS or FAIL.
//
// P3 is red in the normal run (the measured last line of every NTSC field is
// 227 colour clocks, p23-p24 give 227 and 228 alternating by field type);
// it stays graded. No input in this package makes its comparator green.
//
// MUTATION FIX-I stimulus red: NMH_MUTATE=FIX-I runs prog_i_n (FIXMARK stores $DEAD, not $BEEF, I-n); the known-value comparator prints FAIL
// MUTATION PI-1 stimulus red: NMH_MUTATE=PI-1 runs prog_i_j (M1 waits between samples, I-j); PI-1's comparator prints FAIL
// MUTATION PI-2 stimulus red: NMH_MUTATE=PI-2 runs prog_i_f (DMACON leaves COPEN clear, I-f) and NMH_MUTATE=PI-2(long) runs prog_i_v (both lists, relocated, wait for line 255 and then hold 5,200 MOVEs in front of the terminal wait, I-v: p20 gives four memory cycle times per MOVE, so the Copper is still in the list when p25's forced restart comes); PI-2's comparator prints FAIL, in the second run through its third clause alone, one restart per M1 field and consecutive matched restarts being intact
// MUTATION SELFTEST-PI-2-ANCHOR stimulus red: NMH_MUTATE=SELFTEST-PI-2-ANCHOR runs prog_i_s (both lists end in a MOVE to register $000, which the Copper cannot write, in front of the terminal wait, I-s); a self-test of the third clause of PI-2's comparator, not page-predicted: on this design the Copper fetches the MOVE's first word and nothing more of the list, so each field still holds one restart but none is preceded by a terminal wait; PI-2's comparator prints FAIL under the label SELFTEST-PI-2-ANCHOR; controls: one restart per M1 field, consecutive matched restarts
// MUTATION PI-4 stimulus red: NMH_MUTATE=PI-4 runs prog_i_h (DMACON leaves BPLEN clear, I-h); PI-4's comparator prints FAIL
// MUTATION PI-5 stimulus red: NMH_MUTATE=PI-5 runs prog_i_g (INTENA leaves COPER clear, I-g); PI-5's comparator prints FAIL
// MUTATION PI-6 stimulus red: NMH_MUTATE=PI-6 runs prog_i_k (PIC filled with $0000, I-k); PI-6's comparator prints FAIL through the tags
// MUTATION PI-7(bpl) stimulus red: NMH_MUTATE=PI-7(bpl) runs prog_i_l (the BPL1PT MOVEs sit after the WAIT, I-l); the comparator locates the MOVEs in the assembled list and prints FAIL; the gates of P4 and P5 close, so P4a to P4e and P5 print NOT GRADED, as does PI-12(bpl)
// MUTATION PI-7(spr) stimulus red: NMH_MUTATE=PI-7(spr) runs prog_i_m (the SPR0PT MOVEs sit after the WAIT, I-m); the comparator locates the MOVEs in the assembled list and prints FAIL; the gate of P6 closes
// GATE PI-12(bpl) (printed as a MEASURED gate state, not graded; no assertion, so no MUTATION record): NMH_MUTATE=PI-12(bpl) runs prog_i_t (the BPL1PT MOVEs sit after WAIT $2001,$FF00, line 32: in front of the first bitplane fetch, so PI-7(bpl) holds, and after the end of vertical blanking, I-t); the gate prints closed; P4a to P4e, P5 and P5-NOREWRITE print NOT GRADED
// GATE PI-12(spr) (printed as a MEASURED gate state, not graded; no assertion, so no MUTATION record): NMH_MUTATE=PI-12(spr) runs prog_i_u (the SPR0PT MOVEs sit after WAIT $1901,$FF00, line 25: in front of the sprite's first fetch, so PI-7(spr) holds, and outside the 25 lines of vertical blanking p219 guarantees, I-u); the gate prints closed; P6 and P6-NOREWRITE print NOT GRADED
// MUTATION PI-8 stimulus red: NMH_MUTATE=PI-8 runs prog_i_i (DMACON leaves SPREN clear, I-i); PI-8's comparator prints FAIL
// MUTATION PI-9 stimulus red: NMH_MUTATE=PI-9 runs prog_i_o as the control run (LACE clear and M1 waits between samples, I-o); PI-9's comparator prints FAIL; P1-NOLACE prints NOT GRADED
// MUTATION PI-3 stimulus red: NMH_MUTATE=PI-3 runs prog_i_a (LACE clear, I-a); PI-3 is the check of P1 itself, so P1 and the first half of P1-NOLACE are red on the same run by construction and are not printed; PI-3's comparator prints FAIL and P4a to P4e print NOT GRADED; controls: all 8 lengths equal, the other preconditions
// MUTATION P1 stimulus red: NMH_MUTATE=P1 runs prog_i_a (LACE clear, I-a); P1's comparator prints FAIL; controls: all 8 lengths equal, the preconditions other than PI-3 (the check of P1 itself)
// MUTATION P1-NOLACE stimulus red: NMH_MUTATE=P1-NOLACE runs prog_i_a as the interlace run (LACE clear, I-a) and NMH_MUTATE=P1-NOLACE(ctrl) runs prog_i as the control run (LACE set); the unchanged conjunction prints FAIL; the other half is a live control; controls the preconditions other than PI-3, PI-9
// MUTATION P4a stimulus red: NMH_MUTATE=P4a runs prog_i_b (LISTB writes PIC, not PIC+40, I-b); P4a's comparator prints FAIL; controls PI-1 to PI-8, PI-12, P4e (the gate)
// MUTATION P4b stimulus red: NMH_MUTATE=P4b runs prog_i_b (I-b); P4b's comparator prints FAIL; controls PI-1 to PI-8, PI-12, P4e (the gate)
// MUTATION P4c stimulus red: NMH_MUTATE=P4c runs prog_i_b (I-b); P4c's comparator prints FAIL; controls PI-1 to PI-8, PI-12, P4e (the gate)
// MUTATION P4d stimulus red: NMH_MUTATE=P4d runs prog_i_b (I-b) and NMH_MUTATE=P4d(step) runs prog_i_p (both lists keep PIC and PIC+40 but BPL1MOD = BPL2MOD = 38, I-p, so A-group i to B-group i still rises by 40 and B-group i to A-group i+1 does not); P4d's comparator prints FAIL; the three clauses of the comparator print as measured lines; controls PI-1 to PI-8, PI-12, P4e (the gate)
// MUTATION P4e stimulus red: NMH_MUTATE=P4e runs prog_i_d (the handler never repoints COP1LC, I-d); P4e's comparator prints FAIL and P4a to P4d print NOT GRADED; controls PI-1 to PI-8, PI-12
// MUTATION P5 stimulus red: NMH_MUTATE=P5 runs prog_i_c (both lists omit the BPL1PT MOVEs, I-c); P5's comparator prints FAIL through the first 20-word fetch measured wherever it lies, against the address its list would have written; controls: the search populates every field, PI-1, PI-2; PI-4, PI-6, PI-7(bpl) and PI-12(bpl) do not apply
// MUTATION P5-NOREWRITE stimulus red: NMH_MUTATE=P5-NOREWRITE runs prog_i_c as the rewrite run (both lists omit the BPL1PT MOVEs, I-c) and NMH_MUTATE=P5-NOREWRITE(ctrl) runs prog_i as the no-rewrite run (the lists rewrite, I-0); the unchanged conjunction prints FAIL; the other half is a live control; controls PI-1, PI-2, PI-10
// MUTATION PI-10 stimulus red: NMH_MUTATE=PI-10 runs prog_i_q as the no-rewrite run (I-c and M1 waits between samples, I-q); PI-10's comparator prints FAIL; P5-NOREWRITE prints NOT GRADED
// MUTATION P6 stimulus red: NMH_MUTATE=P6 runs prog_i_e (both lists omit the SPR0PT MOVEs, I-e); P6's comparator prints FAIL; controls PI-1, PI-2, PI-8; PI-7(spr) and PI-12(spr) do not apply
// MUTATION P6-NOREWRITE stimulus red: NMH_MUTATE=P6-NOREWRITE runs prog_i_e as the rewrite run (both lists omit the SPR0PT MOVEs, I-e) and NMH_MUTATE=P6-NOREWRITE(ctrl) runs prog_i as the no-rewrite run (the lists rewrite, I-0); the unchanged conjunction prints FAIL; the other half is a live control; controls PI-1, PI-2, PI-8, PI-11
// MUTATION PI-11 stimulus red: NMH_MUTATE=PI-11 runs prog_i_r as the no-rewrite run (I-e and M1 waits between samples, I-r); PI-11's comparator prints FAIL; P6-NOREWRITE prints NOT GRADED
// MUTATION FIX-T stimulus red: NMH_MUTATE=FIX-T runs prog_t_i (FIXMARK stores $DEAD, not $BEEF, T-i); the known-value comparator prints FAIL
// MUTATION PT-1 stimulus red: NMH_MUTATE=PT-1 runs prog_t_f (M1 waits between samples, T-f); PT-1's comparator prints FAIL
// MUTATION PT-2 stimulus red: NMH_MUTATE=PT-2 runs prog_t_e (DIWSTOP is the normal NTSC value, T-e); PT-2's comparator prints FAIL
// MUTATION PT-3 stimulus red: NMH_MUTATE=PT-3 runs prog_t_d (line 100's mark uses H=$C0, T-d); PT-3's comparator prints FAIL
// MUTATION PT-5 stimulus red: NMH_MUTATE=PT-5 runs prog_t_g (line 10's WAIT uses H=$40 and every mark stays live, T-g); PT-5's comparator prints FAIL through measured line lengths outside {227,228}; controls PT-1, PT-3, PT-4 and every mark present
// MUTATION PT-7 stimulus red: NMH_MUTATE=PT-7 runs prog_t_h (DMACON leaves COPEN clear, T-h) and NMH_MUTATE=PT-7(long) runs prog_t_k (LISTT waits for line 255 after its last mark and then holds 1,800 MOVEs in front of the terminal wait, T-k: p20 gives four memory cycle times per MOVE, so the Copper is still in the list when p25's forced restart comes); PT-7's comparator prints FAIL, in the second run through its third clause alone, one restart per M1 field and consecutive matched restarts being intact
// MUTATION SELFTEST-PT-7-ANCHOR stimulus red: NMH_MUTATE=SELFTEST-PT-7-ANCHOR runs prog_t_j (LISTT ends in a MOVE to register $000, which the Copper cannot write, in front of the terminal wait, T-j); a self-test of the third clause of PT-7's comparator, not page-predicted: on this design the Copper fetches the MOVE's first word and nothing more of the list, so each field still holds one restart but none is preceded by a terminal wait; PT-7's comparator prints FAIL under the label SELFTEST-PT-7-ANCHOR; controls: one restart per M1 field, consecutive matched restarts
// MUTATION P2 stimulus red: NMH_MUTATE=P2 runs prog_t with PAL selected instead of NTSC (T-b); P2's comparator prints FAIL; controls PT-1, PT-2, PT-3, PT-5, PT-7
// GATE PT-4 (printed as a MEASURED gate state, not graded; no assertion, so no MUTATION record): NMH_MUTATE=PT-4 runs prog_t_c (the first WAIT is (0,$00), T-c); the gate prints closed through line 0-to-1, PT-5 and P3 print NOT GRADED; controls PT-1, PT-2, PT-3, PT-7
// GATE PT-6 (printed as a MEASURED gate state, not graded; no assertion, so no MUTATION record): NMH_MUTATE=PT-6 runs prog_t_a (LACE clear, T-a); the gate prints closed and P3 prints "not graded, gate closed"; controls PT-1, PT-2, PT-3, PT-7; P2 is not printed, a non-interlaced display is not the display it grades
// MUTATION P3 none impossible: already red in the normal run and not exercised further: NMH_MUTATE=P3 runs prog_t with PAL selected (T-b) and P3's comparator prints FAIL again; no input in this package makes P3's comparator green, so what is shown is that the controls PT-1..PT-7 hold while it is red, not that the comparator can pass

#include <cstdio>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <optional>
#include <memory>
#include <iostream>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

namespace {

constexpr uint32_t kDoneAddr   = 0x0C0FFE;
constexpr uint16_t kDoneValue  = 0x600D;
constexpr uint64_t kBootBudget = 6'000'000;
constexpr uint64_t kRunBudgetI = 16'000'000;
constexpr uint64_t kRunBudgetT = 20'000'000;

// One colour clock (p194, p24) is 8 clk_sys cycles: 4 clk_sys cycles per
// the harness's own 7 MHz RAM-port sample point (nanomig_harness.h), and
// the colour clock is half of that again. A fixed clock-generator ratio,
// not a value fitted to a measurement (SPECIFICATION.md 2.3).
constexpr uint64_t kClkSysPerColourClock = 8;

// Custom-register offsets (Appendix B) the lists are searched for.
constexpr uint16_t kBPL1PTH = 0x0E0, kBPL1PTL = 0x0E2, kSPR0PTH = 0x120, kSPR0PTL = 0x122;

// ---- Program I memory layout, mirrors prog_i.s -----------------------
constexpr uint32_t kI_PIC        = 0x082000;
constexpr uint32_t kI_PIC_END    = kI_PIC + 20480;
constexpr uint32_t kI_SPR        = 0x087000;
constexpr uint32_t kI_LISTA      = 0x080800;
constexpr uint32_t kI_LISTB      = 0x080900;
constexpr uint32_t kI_HCNT       = 0x087222;
constexpr int       kI_K         = 8;
constexpr uint32_t kI_M1_STARTW  = 0x087224;
constexpr uint32_t kI_M1_DONEW   = 0x087226;
constexpr uint32_t kI_M1_RECBASE = 0x087230;
constexpr uint32_t kI_FIXMARK    = 0x087400;

// ---- Program T memory layout, mirrors prog_t.s -----------------------
constexpr uint32_t kT_PLANE      = 0x082000;
constexpr uint32_t kT_PLANE_END  = kT_PLANE + 11000;
constexpr uint32_t kT_LISTT      = 0x080800;
constexpr uint32_t kT_LISTT_LONG = 0x0A0000;   // T-k: LISTT relocated, its list is longer than the room in front of PLANE
constexpr uint32_t kT_LISTT_LONG_SPAN = 0x6000;
constexpr int       kT_K         = 13;
constexpr uint32_t kT_M1_STARTW  = 0x086000;
constexpr uint32_t kT_M1_DONEW   = 0x086002;
constexpr uint32_t kT_M1_RECBASE = 0x086010;
constexpr uint32_t kT_FIXMARK    = 0x086100;

// The assertion call form is fixed by BRIEF.md.
class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    // A live control of a mutation run: printed like a graded line, but it is
    // not an assertion of the package -- it only shows that the mutation
    // changed the one thing it was meant to change.
    void control(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    int passed = 0, failed = 0;
};
Checks checks;

const char* g_mutate = std::getenv("NMH_MUTATE");

// A control line of mutation run <label>: "<label>/<what>: live control".
void ctl(const std::string& label, const std::string& what, bool c) {
    checks.control(c, label + "/" + what + ": live control, expected to hold in this run");
}

// ---- one harness run ---------------------------------------------------

struct Run {
    std::unique_ptr<nmh::NanomigHarness> h;
    size_t loaded = 0;
    bool released = false;
    std::optional<uint64_t> done;
    std::vector<nmh::RamAccess> log; // coalesced
};

// After the completion word the harness keeps the system running until the
// Copper's next fetch of the first word of a list has been logged
// (SPECIFICATION.md 2.5), so that the last recorded field is not cut off
// before its own end. The budget is a few fields; a run that never restarts
// again simply stops at it.
constexpr uint64_t kRunOnChunk  = 2'048;
constexpr uint64_t kRunOnBudget = 2'000'000;

void run_to_next_restart(nmh::NanomigHarness& h, const std::vector<uint32_t>& listBases) {
    size_t seen = h.ram_log().size();
    for (uint64_t ran = 0; ran < kRunOnBudget; ran += kRunOnChunk) {
        h.run_cycles(kRunOnChunk);
        const auto& lg = h.ram_log();
        for (; seen < lg.size(); ++seen) {
            if (lg[seen].is_write) continue;
            for (uint32_t b : listBases) if (lg[seen].word_addr == b / 2) return;
        }
    }
}

Run run_program(const std::string& path, const nmh::Config& cfg, uint64_t run_budget,
                const std::vector<uint32_t>& listBases) {
    Run r;
    r.h = std::make_unique<nmh::NanomigHarness>(cfg);
    r.loaded = r.h->load_program(path);
    if (r.loaded) {
        r.released = r.h->boot(kBootBudget).cpu_released;
        r.done = r.h->run_until_write(kDoneAddr, kDoneValue, kBootBudget + run_budget);
        if (r.done) run_to_next_restart(*r.h, listBases);
    }
    r.log = nmh::coalesce(r.h->ram_log());
    return r;
}

bool ok(const Run& r) { return r.loaded > 0 && r.released && r.done.has_value(); }

std::optional<uint16_t> W(const Run& r, uint32_t byte_addr) { return r.h->peek_logical(byte_addr); }

std::vector<uint64_t> write_cycles(const Run& r, uint32_t byte_addr) {
    uint32_t wa = byte_addr / 2;
    std::vector<uint64_t> out;
    for (const auto& a : r.log) if (a.is_write && a.word_addr == wa) out.push_back(a.cycle);
    return out;
}
std::vector<uint64_t> write_cycles_of(const Run& r, uint32_t byte_addr, uint16_t data) {
    uint32_t wa = byte_addr / 2;
    std::vector<uint64_t> out;
    for (const auto& a : r.log) if (a.is_write && a.word_addr == wa && a.data == data) out.push_back(a.cycle);
    return out;
}
size_t read_count_range(const Run& r, uint32_t lo_addr, uint32_t hi_addr_excl) {
    uint32_t lo = lo_addr / 2, hi = hi_addr_excl / 2;
    size_t n = 0;
    for (const auto& a : r.log) if (!a.is_write && a.word_addr >= lo && a.word_addr < hi) ++n;
    return n;
}

// The byte offset, inside the Copper list at listbase, of the MOVE to custom
// register `reg`, read from the list as the program assembled it (stimulus
// data in chip memory, not a fetch); UINT32_MAX if the list holds none.
uint32_t list_move_offset(const Run& r, uint32_t listbase, uint16_t reg) {
    for (uint32_t off = 0; off < 0x100; off += 4) {
        auto w1 = W(r, listbase + off);
        if (!w1 || *w1 == 0xFFFF) return UINT32_MAX;
        if (!(*w1 & 1) && (*w1 & 0x1FE) == reg) return off;
    }
    return UINT32_MAX;
}

// ---- Measurement M1 (SPECIFICATION.md 2.5) -----------------------------

struct M1Record {
    bool present = false;
    uint16_t firstV = 0xFFFF, lastV = 0xFFFF, gap = 1, lofOR = 0, lofAND = 0;
    uint64_t closeCycle = 0; // port time of the write that closed this record (its last word)
};

std::vector<M1Record> read_m1(const Run& r, uint32_t recbase, int k) {
    std::vector<M1Record> out(k);
    for (int i = 0; i < k; ++i) {
        uint32_t base = recbase + i * 10;
        auto fv = W(r, base + 0), lv = W(r, base + 2), gp = W(r, base + 4),
             lo = W(r, base + 6), la = W(r, base + 8);
        if (fv && lv && gp && lo && la) {
            M1Record m;
            m.present = true;
            m.firstV = *fv; m.lastV = *lv; m.gap = *gp; m.lofOR = *lo; m.lofAND = *la;
            auto wc = write_cycles(r, base + 8);
            if (!wc.empty()) m.closeCycle = wc.front();
            out[i] = m;
        }
    }
    return out;
}

uint64_t start_cycle_of(const Run& r, uint32_t startw_addr) {
    auto wc = write_cycles_of(r, startw_addr, 0xFACE);
    return wc.empty() ? 0 : wc.front();
}
bool m1_completed(const Run& r, uint32_t donew_addr) {
    return !write_cycles_of(r, donew_addr, 0xD1CE).empty();
}
bool m1_all_valid(const std::vector<M1Record>& recs) {
    for (const auto& m : recs) if (!m.present || m.firstV != 0 || m.gap != 0 || m.lastV < 255) return false;
    return true;
}

// ---- Measurement M2: the Copper restart as field anchor ---------------

struct Restart { uint64_t cycle; char list; }; // list is 'A'/'B' (Program I) or ' ' (Program T)

std::vector<Restart> find_restarts(const Run& r, uint32_t listA, uint32_t listB /*0 if single-list*/) {
    std::vector<Restart> out;
    uint32_t wa = listA / 2, wb = listB / 2;
    for (const auto& a : r.log) {
        if (a.is_write) continue;
        if (a.word_addr == wa) out.push_back({a.cycle, listB ? 'A' : ' '});
        else if (listB && a.word_addr == wb) out.push_back({a.cycle, 'B'});
    }
    std::sort(out.begin(), out.end(), [](const Restart& x, const Restart& y) { return x.cycle < y.cycle; });
    return out;
}

// The port-side field index j such that restarts[j].cycle <= t (< the next
// restart's cycle, if any) -- the restart interval containing time t.
int field_index_for_time(const std::vector<Restart>& rs, uint64_t t) {
    int j = -1;
    for (size_t i = 0; i < rs.size(); ++i) { if (rs[i].cycle <= t) j = (int)i; else break; }
    return j;
}

uint64_t interval_end(const std::vector<Restart>& rs, int idx) {
    return (idx + 1 < (int)rs.size()) ? rs[idx + 1].cycle : UINT64_MAX;
}

// section 2.6's own matching rule: M1 record k <-> the restart interval
// containing the midpoint of the writes that open and close it.
std::vector<int> match_m1_to_restarts(const std::vector<M1Record>& m1, uint64_t start_cycle,
                                       const std::vector<Restart>& rs) {
    std::vector<int> out(m1.size(), -1);
    for (size_t k = 0; k < m1.size(); ++k) {
        if (!m1[k].present) continue;
        uint64_t opening = (k == 0) ? start_cycle : m1[k - 1].closeCycle;
        uint64_t closing = m1[k].closeCycle;
        uint64_t mid = opening + (closing - opening) / 2;
        out[k] = field_index_for_time(rs, mid);
    }
    return out;
}

// ---- Measurement M2 anchor: one restart per M1 field (SPECIFICATION.md 2.6) ----

// Offset of the terminal `$FFFF,$FFFE` wait in the list at listbase, read from
// the list as the program assembled it (stimulus data in chip memory, not a
// fetch); UINT32_MAX if the list holds none within span bytes.
uint32_t list_terminal_offset(const Run& r, uint32_t listbase, uint32_t span) {
    for (uint32_t off = 0; off + 4 <= span; off += 4) {
        auto w1 = W(r, listbase + off), w2 = W(r, listbase + off + 2);
        if (w1 && w2 && *w1 == 0xFFFF && *w2 == 0xFFFE) return off;
    }
    return UINT32_MAX;
}

struct AnchorList { uint32_t base, span; };

// What the anchor measured, clause by clause. `fields` is the number of M1
// fields (one window each, from the write that opens the field to the write
// that closes it); `oneRestart` counts the windows that hold exactly one read
// of a list's first word; `ordered` is true when the restart interval each M1
// field is matched to follows the previous field's by exactly one;
// `afterTerminal` counts the windows whose restart is preceded, among the list
// words up to the terminal waits, by a read of a terminal wait.
struct AnchorMeasure {
    bool listsOk = false;
    int fields = 0, oneRestart = 0, afterTerminal = 0;
    bool ordered = false;
    bool ok() const {
        return listsOk && fields > 0 && oneRestart == fields && afterTerminal == fields && ordered;
    }
};

AnchorMeasure measure_anchor(const Run& r, const std::vector<M1Record>& m1, uint64_t startCycle,
                             const std::vector<Restart>& rs, const std::vector<int>& matched,
                             const std::vector<AnchorList>& lists) {
    AnchorMeasure m;
    m.fields = (int)m1.size();
    std::set<uint32_t> terminalWords;
    std::vector<std::pair<uint32_t, uint32_t>> regions; // word-address ranges [lo,hi) up to the terminal wait
    m.listsOk = true;
    for (const auto& l : lists) {
        uint32_t off = list_terminal_offset(r, l.base, l.span);
        if (off == UINT32_MAX) { m.listsOk = false; continue; }
        terminalWords.insert((l.base + off) / 2);
        terminalWords.insert((l.base + off) / 2 + 1);
        regions.push_back({l.base / 2, (l.base + off + 4) / 2});
    }
    if (!m.listsOk) return m;
    // The reads of the lists' words, in port order: the Copper's list fetches.
    std::vector<std::pair<uint64_t, uint32_t>> lf;
    for (const auto& a : r.log) {
        if (a.is_write) continue;
        for (const auto& g : regions)
            if (a.word_addr >= g.first && a.word_addr < g.second) { lf.push_back({a.cycle, a.word_addr}); break; }
    }
    std::stable_sort(lf.begin(), lf.end(), [](const auto& x, const auto& y) { return x.first < y.first; });
    m.ordered = matched.size() == m1.size();
    for (size_t k = 0; k < m1.size(); ++k) {
        if (m.ordered && (matched[k] < 0 || (k > 0 && matched[k] != matched[k - 1] + 1))) m.ordered = false;
        if (!m1[k].present) continue;
        uint64_t opening = (k == 0) ? startCycle : m1[k - 1].closeCycle;
        uint64_t closing = m1[k].closeCycle;
        int n = 0, which = -1;
        for (size_t j = 0; j < rs.size(); ++j)
            if (rs[j].cycle > opening && rs[j].cycle <= closing) { ++n; which = (int)j; }
        if (n != 1) continue;
        ++m.oneRestart;
        auto it = std::lower_bound(lf.begin(), lf.end(), rs[(size_t)which].cycle,
                                   [](const auto& e, uint64_t c) { return e.first < c; });
        if (it == lf.begin() || it == lf.end() || it->first != rs[(size_t)which].cycle) continue;
        if (terminalWords.count((it - 1)->second)) ++m.afterTerminal;
    }
    return m;
}

std::string anchor_text(const AnchorMeasure& m) {
    if (!m.listsOk) return "a list holds no terminal $FFFF,$FFFE wait";
    return std::to_string(m.oneRestart) + " of " + std::to_string(m.fields) + " M1 fields hold exactly one restart, " +
           std::to_string(m.afterTerminal) + " of them preceded, among the list words up to the terminal waits, by a terminal wait, matched restarts consecutive = " +
           (m.ordered ? "yes" : "no");
}

// ---- bitplane fetch grouping (20-word lines, SPECIFICATION.md 2.7) ----

struct BplGroup { uint64_t cycle; uint32_t addr; int len; };

std::vector<BplGroup> group_reads_in(const Run& r, uint32_t lo_addr, uint32_t hi_addr_excl,
                                      uint64_t begin, uint64_t end) {
    uint32_t lo = lo_addr / 2, hi = hi_addr_excl / 2;
    std::vector<BplGroup> out;
    uint32_t expect = 0; bool open = false;
    for (const auto& a : r.log) {
        if (a.cycle < begin || a.cycle >= end) { open = false; continue; }
        if (a.is_write) continue;
        if (a.word_addr < lo || a.word_addr >= hi) continue;
        if (open && a.word_addr == expect) { out.back().len++; expect++; }
        else { out.push_back({a.cycle, a.word_addr, 1}); expect = a.word_addr + 1; open = true; }
    }
    return out;
}

// Program T sets BPL1MOD/BPL2MOD = 0 (section 4.3): the bitplane pointer
// advances with no modulo skip at all, so a field's fetches form one
// single, address-contiguous run with no gap between lines (unlike
// Program I's interlaced BPL1MOD=40, which leaves a genuine 40-byte
// address gap between each 20-word line -- group_reads_in() above finds
// those gaps). Here the 20-word lines are instead cut by count, in fetch
// order; each cut is still checked for 20 consecutive ascending
// addresses (section 2.7's own wording), just not relied on to find the
// cut points. `total` receives the number of reads, so that a remainder of
// fewer than chunk_size reads is not lost.
std::vector<BplGroup> chunk_reads_in(const Run& r, uint32_t lo_addr, uint32_t hi_addr_excl,
                                      uint64_t begin, uint64_t end, int chunk_size, size_t* total) {
    uint32_t lo = lo_addr / 2, hi = hi_addr_excl / 2;
    std::vector<std::pair<uint64_t, uint32_t>> reads;
    for (const auto& a : r.log) {
        if (a.is_write) continue;
        if (a.cycle < begin || a.cycle >= end) continue;
        if (a.word_addr < lo || a.word_addr >= hi) continue;
        reads.push_back({a.cycle, a.word_addr});
    }
    if (total) *total = reads.size();
    std::vector<BplGroup> out;
    for (size_t i = 0; i + (size_t)chunk_size <= reads.size(); i += (size_t)chunk_size) {
        bool consecutive = true;
        for (int j = 1; j < chunk_size; ++j)
            if (reads[i + (size_t)j].second != reads[i].second + (uint32_t)j) consecutive = false;
        out.push_back({reads[i].first, reads[i].second, consecutive ? chunk_size : -1});
    }
    return out;
}

//=========================================================================
// Program I: P1, P4, P5, P6 and their preconditions PI-1..PI-12
//=========================================================================

nmh::Config i_config() {
    nmh::Config c;
    c.chipset_config = 0x08; // ECS, PAL (chipset_config[1]=0)
    return c;
}

// The addresses of the two Copper lists of one Program I binary: the normal
// ones, and the relocated ones of I-v, whose lists are longer than the room
// between them and PIC.
struct ILayout { uint32_t listA, listB, span; };
ILayout i_layout(const std::string& prog) {
    if (prog == "prog_i_v") return {0x0A0000, 0x0A6000, 0x6000};
    return {kI_LISTA, kI_LISTB, kI_LISTB - kI_LISTA};
}

// A precondition is graded (it holds or it fails) or it is not graded: the
// restart frame, the M1 records or the population it reads are missing from
// the run, or it does not apply to the run (the lists hold no such MOVE).
struct Pv {
    enum S { Holds, Fails, NotGraded } s = NotGraded;
    std::string why; // the reason, when not graded
    bool holds() const { return s == Holds; }
    bool ng() const { return s == NotGraded; }
};
Pv graded(bool c) { Pv p; p.s = c ? Pv::Holds : Pv::Fails; return p; }
Pv not_graded(const std::string& why) { Pv p; p.s = Pv::NotGraded; p.why = why; return p; }

// One precondition line: PASS or FAIL through its comparator, or NOT GRADED
// with the reason. `label` is the full label, "<id>: <text>".
void pre_line(const Pv& v, const std::string& label) {
    if (v.ng()) { checks.record(label.substr(0, label.find(':')) + ": NOT GRADED, " + v.why); return; }
    checks.expect(v.holds(), label);
}

// The pointer value a list's two MOVEs to regH and regL write, read from the
// list as the program assembled it (stimulus data in chip memory, not a fetch).
bool list_holds_pair(const Run& r, uint32_t listbase, uint16_t regH, uint16_t regL) {
    return list_move_offset(r, listbase, regH) != UINT32_MAX && list_move_offset(r, listbase, regL) != UINT32_MAX;
}
std::optional<uint32_t> list_pair_value(const Run& r, uint32_t listbase, uint16_t regH, uint16_t regL) {
    uint32_t oh = list_move_offset(r, listbase, regH), ol = list_move_offset(r, listbase, regL);
    if (oh == UINT32_MAX || ol == UINT32_MAX) return std::nullopt;
    auto hi = W(r, listbase + oh + 2), lo = W(r, listbase + ol + 2);
    if (!hi || !lo) return std::nullopt;
    return ((uint32_t)*hi << 16) | *lo;
}

struct IField {
    int m1 = -1;          // index into m1 records
    int restart = -1;     // index into restarts
    char list = '?';
    uint64_t begin = 0, end = 0;
    std::vector<BplGroup> bpl;
};

struct IAnalysis {
    Run r;
    ILayout lay{};
    std::vector<M1Record> m1;
    uint64_t startCycle = 0;
    bool completed = false;
    std::vector<Restart> restarts;
    std::vector<IField> fields; // kI_K entries, in M1/field order 0..K-1
    bool matched = false;
    // The first 20-word bitplane fetch group of every field, found wherever it
    // lies from PIC upward (p5_first_groups); firstsOk is false unless every
    // field has one. P4(a) and P4(b) grade these, not the first group inside
    // [PIC, PIC_END).
    std::vector<BplGroup> firsts;
    bool firstsOk = false;
    AnchorMeasure anchor; // PI-2
    // Both lists hold the pointer MOVEs (read from the assembled lists).
    bool hasBpl = false, hasSpr = false;
};

bool p5_first_groups(const IAnalysis& a, std::vector<BplGroup>& out);

IAnalysis analyze_i(const std::string& dir, const std::string& prog) {
    IAnalysis a;
    a.lay = i_layout(prog);
    a.r = run_program(dir + "/" + prog + ".bin", i_config(), kRunBudgetI, {a.lay.listA, a.lay.listB});
    a.m1 = read_m1(a.r, kI_M1_RECBASE, kI_K);
    a.startCycle = start_cycle_of(a.r, kI_M1_STARTW);
    a.completed = m1_completed(a.r, kI_M1_DONEW);
    a.restarts = find_restarts(a.r, a.lay.listA, a.lay.listB);
    auto idx = match_m1_to_restarts(a.m1, a.startCycle, a.restarts);
    a.fields.resize(kI_K);
    a.matched = true;
    for (int k = 0; k < kI_K; ++k) {
        IField f;
        f.m1 = k;
        f.restart = idx[k];
        if (f.restart < 0 || f.restart >= (int)a.restarts.size()) { a.matched = false; continue; }
        f.list = a.restarts[f.restart].list;
        f.begin = a.restarts[f.restart].cycle;
        f.end = interval_end(a.restarts, f.restart);
        f.bpl = group_reads_in(a.r, kI_PIC, kI_PIC_END, f.begin, f.end);
        a.fields[k] = f;
    }
    a.firstsOk = p5_first_groups(a, a.firsts);
    std::vector<int> matched;
    for (const auto& f : a.fields) matched.push_back(f.restart);
    a.anchor = measure_anchor(a.r, a.m1, a.startCycle, a.restarts, matched,
                              {{a.lay.listA, a.lay.span}, {a.lay.listB, a.lay.span}});
    a.hasBpl = list_holds_pair(a.r, a.lay.listA, kBPL1PTH, kBPL1PTL) && list_holds_pair(a.r, a.lay.listB, kBPL1PTH, kBPL1PTL);
    a.hasSpr = list_holds_pair(a.r, a.lay.listA, kSPR0PTH, kSPR0PTL) && list_holds_pair(a.r, a.lay.listB, kSPR0PTH, kSPR0PTL);
    return a;
}

bool fix_i(const IAnalysis& a) {
    return ok(a.r) && W(a.r, kI_FIXMARK) && *W(a.r, kI_FIXMARK) == 0xBEEF &&
           !write_cycles_of(a.r, kI_FIXMARK, 0xBEEF).empty() &&
           write_cycles_of(a.r, kI_FIXMARK, 0xBEEF).front() < a.startCycle;
}

bool pi1_valid(const IAnalysis& a) { return ok(a.r) && a.completed && m1_all_valid(a.m1); }

bool pi2_anchor(const IAnalysis& a) {
    if (!a.m1.back().present || !a.anchor.ok()) return false;
    for (const auto& f : a.fields) if (f.list != 'A' && f.list != 'B') return false;
    return true;
}

bool pi4_groups(const IAnalysis& a) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        if (f.bpl.size() != 256) return false;
        for (const auto& g : f.bpl) if (g.len != 20) return false;
    }
    return true;
}

bool p1_alternation(const IAnalysis& a, std::vector<int>* Ns = nullptr) {
    if (!a.m1.back().present) return false;
    std::vector<int> N(kI_K);
    for (int k = 0; k < kI_K; ++k) {
        if (!a.m1[k].present) return false;
        N[k] = a.m1[k].lastV + 1;
        if (N[k] != 312 && N[k] != 313) return false;
    }
    for (int k = 0; k + 1 < kI_K; ++k) if (N[k] == N[k + 1] || N[k] + N[k + 1] != 625) return false;
    if (Ns) *Ns = N;
    return true;
}

// All 8 recorded field lengths of a run are present and equal.
bool all_lengths_equal(const IAnalysis& a, std::vector<int>* Ns = nullptr) {
    if (!a.m1.back().present) return false;
    std::vector<int> N;
    for (int k = 0; k < kI_K; ++k) {
        if (!a.m1[(size_t)k].present) return false;
        N.push_back(a.m1[(size_t)k].lastV + 1);
    }
    for (size_t k = 1; k < N.size(); ++k) if (N[k] != N[0]) return false;
    if (Ns) *Ns = N;
    return true;
}

// The LACE-clear control run is a valid fixture: PI-1 and PI-2 on that run.
bool pi9_control_valid(const IAnalysis& c) { return pi1_valid(c) && pi2_anchor(c); }

bool pi5_hcnt(const IAnalysis& a) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        int n = 0;
        for (const auto& e : write_cycles(a.r, kI_HCNT))
            if (e >= f.begin && e < f.end) ++n;
        if (n != 1) return false;
    }
    return true;
}

// Every fetch from inside PIC returns its tag. (The population of those fetches
// is PI-4's: 256 groups of 20 in every field, so PI-6 is graded only where
// every field has fetches inside PIC.)
bool pi6_tags(const IAnalysis& a) {
    for (const auto& acc : a.r.log) {
        if (acc.is_write) continue;
        if (acc.word_addr * 2 < kI_PIC || acc.word_addr * 2 >= kI_PIC_END) continue;
        uint32_t line = (acc.word_addr * 2 - kI_PIC) / 40;
        if (acc.data != (uint16_t)(0x8000 | line)) return false;
    }
    return true;
}

// Finds the port cycle of the earliest read of word address `wa` within
// [begin,end), or UINT64_MAX if there is none.
uint64_t first_read_cycle(const Run& r, uint32_t wa, uint64_t begin, uint64_t end) {
    for (const auto& acc : r.log)
        if (!acc.is_write && acc.word_addr == wa && acc.cycle >= begin && acc.cycle < end) return acc.cycle;
    return UINT64_MAX;
}

// The Copper fetches of the two MOVEs that write `regH` and `regL` in field k
// are found at the offsets the assembled list holds them at (list_move_offset),
// so the same comparator serves the normal list and the variants that move them.
bool pair_fetch_cycles(const IAnalysis& a, size_t k, uint16_t regH, uint16_t regL, uint64_t& cycH, uint64_t& cycL) {
    const auto& f = a.fields[k];
    uint32_t listbase = (f.list == 'A') ? a.lay.listA : a.lay.listB;
    uint32_t offH = list_move_offset(a.r, listbase, regH);
    uint32_t offL = list_move_offset(a.r, listbase, regL);
    if (offH == UINT32_MAX || offL == UINT32_MAX) return false;
    cycH = first_read_cycle(a.r, (listbase + offH) / 2, f.begin, f.end);
    cycL = first_read_cycle(a.r, (listbase + offL) / 2, f.begin, f.end);
    return cycH != UINT64_MAX && cycL != UINT64_MAX;
}

// The first bitplane fetch of field k: the earlier of the first fetch inside PIC
// and the first 20-word group found from PIC upward; UINT64_MAX if there is none.
uint64_t first_bitplane_fetch(const IAnalysis& a, size_t k) {
    uint64_t t = UINT64_MAX;
    if (!a.fields[k].bpl.empty()) t = a.fields[k].bpl.front().cycle;
    if (a.firstsOk && k < a.firsts.size()) t = std::min(t, a.firsts[k].cycle);
    return t;
}
uint64_t first_sprite_fetch(const IAnalysis& a, size_t k) {
    const auto& f = a.fields[k];
    for (const auto& acc : a.r.log)
        if (!acc.is_write && acc.word_addr * 2 >= kI_SPR && acc.word_addr * 2 < kI_SPR + 16 &&
            acc.cycle >= f.begin && acc.cycle < f.end) return acc.cycle;
    return UINT64_MAX;
}

// Every field has a first bitplane fetch / a sprite-0 fetch to order a MOVE against.
bool every_field_has(const IAnalysis& a, bool bitplane) {
    if (!a.matched) return false;
    for (size_t k = 0; k < a.fields.size(); ++k)
        if ((bitplane ? first_bitplane_fetch(a, k) : first_sprite_fetch(a, k)) == UINT64_MAX) return false;
    return true;
}

bool move_pair_fetched_before(const IAnalysis& a, uint16_t regH, uint16_t regL, bool bitplane) {
    if (!a.matched) return false;
    for (size_t k = 0; k < a.fields.size(); ++k) {
        uint64_t cycH, cycL;
        if (!pair_fetch_cycles(a, k, regH, regL, cycH, cycL)) return false;
        uint64_t firstFetch = bitplane ? first_bitplane_fetch(a, k) : first_sprite_fetch(a, k);
        if (firstFetch == UINT64_MAX || cycH >= firstFetch) return false;
    }
    return true;
}
bool pi7_bpl(const IAnalysis& a) { return move_pair_fetched_before(a, kBPL1PTH, kBPL1PTL, true); }
bool pi7_spr(const IAnalysis& a) { return move_pair_fetched_before(a, kSPR0PTH, kSPR0PTL, false); }

// The write that opens M1 field k (the field's first accepted sample lies in
// line 0, section 2.5).
uint64_t m1_opening(const IAnalysis& a, size_t k) { return k == 0 ? a.startCycle : a.m1[k - 1].closeCycle; }

// Vertical blanking lasts at least 25 lines in PAL (p219), counted from line 0.
// The sample that opens an M1 field lies in line 0 and the write lies less than
// one and a half lines after the start of line 0 (SPECIFICATION.md 3.6), so a
// fetch less than kVblankLines after that write lies before line 24.5.
constexpr int64_t kVblankLines = 23;
constexpr int64_t kPalLineClk  = 227 * (int64_t)kClkSysPerColourClock;

// The later of the two fetches of the MOVE pair, in clk_sys from the write
// that opens the field (negative where the fetch precedes it), for every field;
// false if a fetch is missing.
bool pair_offsets_after_opening(const IAnalysis& a, uint16_t regH, uint16_t regL, std::vector<int64_t>& out) {
    out.clear();
    if (!a.matched) return false;
    for (size_t k = 0; k < a.fields.size(); ++k) {
        uint64_t cycH, cycL;
        if (!pair_fetch_cycles(a, k, regH, regL, cycH, cycL)) return false;
        out.push_back((int64_t)std::max(cycH, cycL) - (int64_t)m1_opening(a, k));
    }
    return true;
}
bool move_pair_in_blanking(const IAnalysis& a, uint16_t regH, uint16_t regL) {
    std::vector<int64_t> d;
    if (!pair_offsets_after_opening(a, regH, regL, d)) return false;
    for (int64_t x : d) if (x >= kVblankLines * kPalLineClk) return false;
    return true;
}
bool pi12_bpl(const IAnalysis& a) { return move_pair_in_blanking(a, kBPL1PTH, kBPL1PTL); }
bool pi12_spr(const IAnalysis& a) { return move_pair_in_blanking(a, kSPR0PTH, kSPR0PTL); }

bool pi8_spr_fetched(const IAnalysis& a) { return read_count_range(a.r, kI_SPR, kI_SPR + 16) > 0; }

// The first 20-word bitplane fetch group of every field (found from PIC
// upward, as P5 finds it) starts at the address its list wrote.
bool p4a_each(const IAnalysis& a) {
    if (!a.matched || !a.firstsOk || a.firsts.size() != a.fields.size()) return false;
    for (size_t k = 0; k < a.fields.size(); ++k) {
        uint32_t want = (a.fields[k].list == 'A') ? kI_PIC : kI_PIC + 40;
        if (a.firsts[k].addr * 2 != want) return false;
    }
    return true;
}

// Alternation without a fixed first list: no two consecutive fields run
// the same list, and every field runs one of the two. The gate of P4(a) to
// P4(d), and P4(e)'s own comparator.
bool p4e(const IAnalysis& a) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) if (f.list != 'A' && f.list != 'B') return false;
    for (int k = 0; k + 1 < kI_K; ++k) if (a.fields[(size_t)k].list == a.fields[(size_t)k + 1].list) return false;
    return true;
}

bool p4a(const IAnalysis& a) { return p4a_each(a); }

// For every two consecutive recorded fields, the B-field's starting address
// is 40 bytes past the A-field's, whichever of the two comes first; both
// orders (A then B, B then A) occur.
bool p4b(const IAnalysis& a) {
    if (!a.firstsOk || a.firsts.size() != a.fields.size()) return false;
    int ab = 0, ba = 0;
    for (int k = 0; k + 1 < kI_K; ++k) {
        const auto& f0 = a.fields[(size_t)k];
        const auto& g0 = a.firsts[(size_t)k];
        const auto& g1 = a.firsts[(size_t)k + 1];
        const auto& ga = (f0.list == 'A') ? g0 : g1;
        const auto& gb = (f0.list == 'A') ? g1 : g0;
        if ((int64_t)gb.addr * 2 - (int64_t)ga.addr * 2 != 40) return false;
        if (f0.list == 'A') ++ab; else ++ba;
    }
    return ab >= 1 && ba >= 1;
}

bool p4c(const IAnalysis& a) {
    for (const auto& f : a.fields) {
        if (f.bpl.size() != 256) return false;
        uint32_t base = (f.list == 'A') ? kI_PIC : (kI_PIC + 40);
        for (int i = 0; i < 256; ++i)
            if (f.bpl[(size_t)i].addr * 2 != base + 80u * (uint32_t)i) return false;
    }
    return true;
}

// Section 5, P4 (d), over every two consecutive recorded fields (an A-field
// and a B-field, in either order). `set`: together they fetch memory lines
// 0..511 each exactly once; `ab`: A-group i to B-group i rises by 40 bytes;
// `ba`: B-group i to A-group i+1 rises by 40 bytes (the screen order
// A-group 0, B-group 0, A-group 1, ...).
struct P4dParts { bool set = false, ab = false, ba = false; };
P4dParts p4d_parts(const IAnalysis& a) {
    P4dParts p;
    p.set = p.ab = p.ba = true;
    for (int k = 0; k + 1 < kI_K; ++k) {
        const auto& f0 = a.fields[(size_t)k];
        const auto& f1 = a.fields[(size_t)k + 1];
        const auto& fa = (f0.list == 'A') ? f0 : f1;
        const auto& fb = (f0.list == 'A') ? f1 : f0;
        if (fa.bpl.size() != 256 || fb.bpl.size() != 256) return P4dParts{};
        std::vector<bool> seen(512, false);
        int distinct = 0;
        for (int i = 0; i < 256; ++i) {
            for (const BplGroup* g : {&fa.bpl[(size_t)i], &fb.bpl[(size_t)i]}) {
                int64_t off = (int64_t)g->addr * 2 - (int64_t)kI_PIC;
                if (off < 0 || off % 40 != 0 || off / 40 >= 512) { p.set = false; continue; }
                size_t line = (size_t)(off / 40);
                if (seen[line]) p.set = false; else { seen[line] = true; ++distinct; }
            }
            int64_t addrA = (int64_t)fa.bpl[(size_t)i].addr * 2, addrB = (int64_t)fb.bpl[(size_t)i].addr * 2;
            if (addrB - addrA != 40) p.ab = false;
            if (i + 1 < 256 && (int64_t)fa.bpl[(size_t)i + 1].addr * 2 - addrB != 40) p.ba = false;
        }
        if (distinct != 512) p.set = false;
    }
    return p;
}
bool p4d(const IAnalysis& a) { P4dParts p = p4d_parts(a); return p.set && p.ab && p.ba; }

// The first 20-word bitplane fetch of every field, found wherever it lies in
// chip memory from PIC upward (the identity-mapped data area; the program's
// own code lies below PIC), so that a field whose pointer was not rewritten
// is measured at its real address and not through an empty window.
bool p5_first_groups(const IAnalysis& a, std::vector<BplGroup>& out) {
    out.clear();
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        auto groups = group_reads_in(a.r, kI_PIC, nmh::kChipIdentityHi, f.begin, f.end);
        const BplGroup* found = nullptr;
        for (const auto& g : groups) if (g.len == 20) { found = &g; break; }
        if (!found) return false;
        out.push_back(*found);
    }
    return true;
}

// The address the field's list writes into BPL1PT, read from the assembled list;
// for a list that holds no such MOVEs (I-c), the address its list would have
// written: PIC for an A-field, PIC + 40 for a B-field.
uint32_t rewritten_address(const IAnalysis& a, const IField& f) {
    uint32_t listbase = (f.list == 'A') ? a.lay.listA : a.lay.listB;
    auto v = list_pair_value(a.r, listbase, kBPL1PTH, kBPL1PTL);
    if (v) return *v;
    return (f.list == 'A') ? kI_PIC : kI_PIC + 40;
}
bool p5(const IAnalysis& a) {
    std::vector<BplGroup> firsts;
    if (!p5_first_groups(a, firsts)) return false;
    for (size_t k = 0; k < firsts.size(); ++k)
        if (firsts[k].addr * 2 != rewritten_address(a, a.fields[k])) return false;
    return true;
}

bool p6(const IAnalysis& a) {
    if (!a.matched) return false;
    const uint32_t words[4] = {kI_SPR + 4, kI_SPR + 6, kI_SPR + 8, kI_SPR + 10};
    for (const auto& f : a.fields) {
        for (uint32_t wbyte : words) {
            uint32_t wa = wbyte / 2;
            bool seen = false;
            for (const auto& acc : a.r.log)
                if (!acc.is_write && acc.word_addr == wa && acc.cycle >= f.begin && acc.cycle < f.end) { seen = true; break; }
            if (!seen) return false;
        }
    }
    return true;
}

// The "not rewritten" half of ch2-putting-together-a-copper-instruction-list.c3
// on the run whose lists omit the BPL1PT MOVEs: in no later field (index 1
// upward; the first recorded field may be the first field after DMA-on, which
// starts at the pointer the CPU wrote) does the first 20-word fetch start at
// the address the rewrite would have given. An unpopulated search is false.
bool p5_norewrite(const IAnalysis& c) {
    std::vector<BplGroup> firsts;
    if (!p5_first_groups(c, firsts) || firsts.size() != (size_t)kI_K) return false;
    for (size_t k = 1; k < firsts.size(); ++k) {
        uint32_t rewritten = (c.fields[k].list == 'A') ? kI_PIC : kI_PIC + 40;
        if (firsts[k].addr * 2 == rewritten) return false;
    }
    return true;
}

// The four data words of the sprite structure fetched in field f.
int sprite_words_fetched_in(const IAnalysis& a, const IField& f) {
    int n = 0;
    for (uint32_t wbyte : {kI_SPR + 4, kI_SPR + 6, kI_SPR + 8, kI_SPR + 10}) {
        uint32_t wa = wbyte / 2;
        for (const auto& acc : a.r.log)
            if (!acc.is_write && acc.word_addr == wa && acc.cycle >= f.begin && acc.cycle < f.end) { ++n; break; }
    }
    return n;
}

// The "not rewritten" half of ch2-putting-together-a-copper-instruction-list.c4
// on the run whose lists omit the SPR0PT MOVEs: none of the four data words is
// fetched in any later field (index 1 upward, as for p5_norewrite).
bool p6_norewrite(const IAnalysis& c) {
    if (!c.matched) return false;
    for (size_t k = 1; k < c.fields.size(); ++k)
        if (sprite_words_fetched_in(c, c.fields[k]) != 0) return false;
    return true;
}

// The no-rewrite runs are valid fixtures: PI-1 and PI-2 on the run, and the
// population the comparator reads is there (the 20-word search finds a group in
// every field of the I-c run; every field of the I-e run is a live bitplane
// field, PI-4, and the sprite structure is fetched at some time, PI-8).
bool pi10_ctrl_valid(const IAnalysis& c) {
    std::vector<BplGroup> firsts;
    return pi1_valid(c) && pi2_anchor(c) && p5_first_groups(c, firsts);
}
bool pi11_ctrl_valid(const IAnalysis& c) {
    return pi1_valid(c) && pi2_anchor(c) && pi4_groups(c) && pi8_spr_fetched(c);
}

// The preconditions of Program I on one run. A precondition that reads the M1
// records is graded only where they are valid (PI-1); one that reads the
// restart frame only where every M1 field is matched to a restart; one that
// reads the bitplane or the sprite population only where it is there; PI-4,
// PI-6 and PI-7(bpl) only where the lists rewrite BPL1PT and PI-7(spr) only
// where they rewrite SPR0PT (the MOVE is read from the assembled list).
struct IPre {
    Pv pi1, pi2, pi3, pi4, pi5, pi6, pi7b, pi7s, pi8, pi12b, pi12s;
    bool hasBpl = false, hasSpr = false, searchOk = false, p4eOk = false;
};
IPre i_pre(const IAnalysis& a) {
    IPre p;
    p.hasBpl = a.hasBpl; p.hasSpr = a.hasSpr; p.searchOk = a.firstsOk; p.p4eOk = p4e(a);
    const std::string noM1 = "the M1 records are not valid (PI-1), so the fields cannot be located";
    p.pi1 = graded(pi1_valid(a));
    bool frame = p.pi1.holds() && a.matched;
    std::string noFrame = p.pi1.holds() ? "no M1 field is matched to a restart: the run has no restart frame"
                                        : noM1;
    p.pi2 = p.pi1.holds() ? graded(pi2_anchor(a)) : not_graded(noM1);
    p.pi3 = p.pi1.holds() ? graded(p1_alternation(a)) : not_graded(noM1);
    p.pi4 = !frame ? not_graded(noFrame)
          : !a.hasBpl ? not_graded("the lists hold no BPL1PT MOVEs, so the fetches lie outside PIC's window")
          : graded(pi4_groups(a));
    p.pi5 = frame ? graded(pi5_hcnt(a)) : not_graded(noFrame);
    p.pi6 = !p.pi4.holds() ? not_graded("PI-4 did not hold or was not graded in this run, so it is not shown that every field has fetches inside PIC")
          : graded(pi6_tags(a));
    p.pi7b = !frame ? not_graded(noFrame)
           : !a.hasBpl ? not_graded("the lists hold no BPL1PT MOVEs: not applicable")
           : !every_field_has(a, true) ? not_graded("some field has no bitplane fetch to order the MOVEs against")
           : graded(pi7_bpl(a));
    p.pi8 = graded(pi8_spr_fetched(a));
    p.pi7s = !frame ? not_graded(noFrame)
           : !a.hasSpr ? not_graded("the lists hold no SPR0PT MOVEs: not applicable")
           : !p.pi8.holds() ? not_graded("PI-8 did not hold: the run has no sprite-0 fetch to order the MOVEs against")
           : graded(pi7_spr(a));
    p.pi12b = p.pi7b.holds() ? graded(pi12_bpl(a)) : not_graded("PI-7(bpl) did not hold or was not graded in this run");
    p.pi12s = p.pi7s.holds() ? graded(pi12_spr(a)) : not_graded("PI-7(spr) did not hold or was not graded in this run");
    return p;
}
bool p1_gate(const IPre& p)  { return p.pi1.holds() && p.pi2.holds() && p.pi4.holds(); }
// P5 reads the first 20-word fetch of every field; where the lists hold the
// BPL1PT MOVEs, they must be fetched in vertical blanking, in front of it.
bool p5_gate(const IPre& p)  {
    return p.pi1.holds() && p.pi2.holds() && p.searchOk && (!p.hasBpl || (p.pi7b.holds() && p.pi12b.holds()));
}
bool p6_gate(const IPre& p)  {
    return p.pi1.holds() && p.pi2.holds() && p.pi8.holds() && (!p.hasSpr || (p.pi7s.holds() && p.pi12s.holds()));
}
// The preconditions of P4 and of its gate P4(e).
bool p4_pre(const IPre& p) {
    return p.pi1.holds() && p.pi2.holds() && p.pi3.holds() && p.pi4.holds() && p.pi5.holds() && p.pi6.holds() &&
           p.pi7b.holds() && p.pi12b.holds();
}
bool p4e_gate(const IPre& p) { return p4_pre(p); }
bool p4_gate(const IPre& p)  { return p4_pre(p) && p.p4eOk; }

// ---- Program I assertions: one function per label, used by the normal
// run and by that label's mutation run alike ----------------------------

void g_FIX_I(const IAnalysis& a) {
    checks.expect(fix_i(a),
                  "FIX-I: Program I stores and delivers a known constant (BEEF) before M1 records anything");
}
void g_PI1(const Pv& v) {
    pre_line(v,
        "PI-1: Program I's fixture -- the completion word was written and all 8 M1 records are valid "
        "(first V is 0, gap flag clear, last V >= 255)");
}
void g_PI2(const IAnalysis& a, const Pv& v) {
    pre_line(v,
        "PI-2: each of the 8 recorded M1 fields holds exactly one read of LISTA's or LISTB's first word "
        "between the writes that open and close it, the restart intervals the fields are matched to follow "
        "each other one by one in field order, and the Copper's last read of LISTA's or LISTB's words up to "
        "their terminal $FFFF,$FFFE waits before each such read is a read of a terminal wait (section 2.6)");
    if (!v.ng()) checks.note("PI-2 measured: " + anchor_text(a.anchor));
}
void g_PI4(const Pv& v) {
    pre_line(v,
        "PI-4: every recorded field contains exactly 256 groups of 20 consecutive ascending bitplane fetches");
}
void g_PI5(const Pv& v) {
    pre_line(v,
        "PI-5: HCNT rises by exactly 1 in every recorded field");
}
void g_PI6(const Pv& v) {
    pre_line(v,
        "PI-6: every bitplane fetch from inside PIC returns the tag ($8000 OR its memory line) of its address");
}
void g_PI7bpl(const Pv& v) {
    pre_line(v,
        "PI-7(bpl): in every field, the Copper fetch of the BPL1PT MOVEs precedes the field's first bitplane fetch "
        "(the earlier of the first fetch inside PIC and the first 20-word group found from PIC upward)");
}
void g_PI7spr(const Pv& v) {
    pre_line(v,
        "PI-7(spr): in every field, the Copper fetch of the SPR0PT MOVEs precedes the field's first sprite-0 fetch");
}
void g_PI8(const Pv& v) {
    pre_line(v,
        "PI-8: the sprite-0 structure at SPR is fetched at least once during the run");
}
// PI-12 is a gate, not a graded line: p25 leaves open whether the Copper
// restarts at the start or at the end of vertical blanking (SPECIFICATION.md
// section 6, item 3), and after a restart at the end the MOVEs are fetched
// after line 25. The gate state is printed; when it is closed, P4 and P5
// (bpl) or P6 (spr) are NOT GRADED.
std::string pi12_gate_text(const char* id, const char* of, const Pv& v) {
    return std::string(id) + " gate of " + of + " (in every field, the Copper fetch of the MOVEs lies less than 23 "
        "lines after the write that opens the field, before line 25, the minimum PAL vertical blanking, p219): " +
        (v.ng() ? "not measured, " + v.why : v.holds() ? "open" : "closed");
}
void g_PI12bpl(const Pv& v) { checks.note(pi12_gate_text("PI-12(bpl)", "P4 and P5", v)); }
void g_PI12spr(const Pv& v) { checks.note(pi12_gate_text("PI-12(spr)", "P6", v)); }
// Where the MOVE pair is fetched, in colour clocks after the write that opens the field.
void note_pi12(const std::string& id, const IAnalysis& a, uint16_t regH, uint16_t regL) {
    std::vector<int64_t> d;
    if (!pair_offsets_after_opening(a, regH, regL, d) || d.empty()) return;
    int64_t lo = *std::min_element(d.begin(), d.end()), hi = *std::max_element(d.begin(), d.end());
    checks.note(id + " measured: the later of the two MOVE fetches lies " + std::to_string(lo / (int64_t)kClkSysPerColourClock) +
                " to " + std::to_string(hi / (int64_t)kClkSysPerColourClock) + " colour clocks from the write that opens the field (negative: before it; limit " +
                std::to_string(kVblankLines * kPalLineClk / (int64_t)kClkSysPerColourClock) + ")");
}
void g_PI9(const IAnalysis& ctrl) {
    checks.expect(pi9_control_valid(ctrl),
        "PI-9: the LACE-clear control run (I-a) is a valid fixture -- its completion word was written, all 8 "
        "of its M1 records are valid and its restart anchor holds (PI-1 and PI-2 on that run)");
}
void g_PI3(const Pv& v) {
    pre_line(v,
        "PI-3: interlace is active -- the check of P1 holds");
}
void g_P1(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P1: NOT GRADED, a precondition (PI-1, PI-2 or PI-4) did not hold on this run"); return; }
    checks.expect(p1_alternation(a),
        "P1 p24,p34 COP2-vertical-beam-position.c12 COP2-vertical-beam-position.c15 "
        "COP2-vertical-beam-position.c16 COP2-using-the-copper-in-interlaced-mode.c5 "
        "COP2-using-the-copper-in-interlaced-mode.c6 COP2-using-the-copper-in-interlaced-mode.c11 "
        "ch2-using-the-copper-in-interlaced-mode.c6: with LACE set, all 8 recorded PAL fields have "
        "312 or 313 lines, and every consecutive pair sums to 625 (alternates)");
}
// `ctrl` is the LACE-clear run; it is needed only where the gate is open.
void g_P1_NOLACE(const IAnalysis& p1run, const IAnalysis* ctrl, bool pre) {
    if (!pre || !ctrl) { checks.record("P1-NOLACE: NOT GRADED, a precondition (PI-1, PI-2, PI-4 or PI-9) did not hold"); return; }
    checks.expect(p1_alternation(p1run) && all_lengths_equal(*ctrl),
        "P1-NOLACE p24 ch2-vertical-beam-position.c12: long and short fields occur in interlace only -- "
        "with LACE set all 8 recorded PAL fields alternate between 312 and 313 lines, and with LACE "
        "clear (I-a) all 8 recorded field lengths are equal");
}
void g_P4a(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P4a: NOT GRADED, a precondition (PI-1 to PI-6, PI-7(bpl) or PI-12(bpl)) or the gate P4e did not hold on this run"); return; }
    checks.expect(p4a(a),
        "P4a p34 ch2-using-the-copper-in-interlaced-mode.c16 COP2-using-the-copper-in-interlaced-mode.c15: the "
        "first 20-word bitplane fetch group of every A-field, found from PIC upward, starts at PIC and of every "
        "B-field at PIC+40 (both kinds of field occur: the gate P4e)");
}
void g_P4b(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P4b: NOT GRADED, a precondition (PI-1 to PI-6, PI-7(bpl) or PI-12(bpl)) or the gate P4e did not hold on this run"); return; }
    checks.expect(p4b(a),
        "P4b p34 ch2-using-the-copper-in-interlaced-mode.c17 ch2-using-the-copper-in-interlaced-mode.c18 "
        "COP2-using-the-copper-in-interlaced-mode.c16 COP2-using-the-copper-in-interlaced-mode.c17: "
        "for every two consecutive recorded fields, which alternate between the A-list and the B-list, the "
        "B-field's starting address (its first 20-word bitplane fetch group, found from PIC upward) is 40 bytes "
        "past the A-field's (one picture line), in both orders (A then B, B then A)");
}
void g_P4c(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P4c: NOT GRADED, a precondition (PI-1 to PI-6, PI-7(bpl) or PI-12(bpl)) or the gate P4e did not hold on this run"); return; }
    checks.expect(p4c(a),
        "P4c p34 ch2-using-the-copper-in-interlaced-mode.c10 ch2-using-the-copper-in-interlaced-mode.c13 "
        "ch2-using-the-copper-in-interlaced-mode.c14 COP2-using-the-copper-in-interlaced-mode.c12 "
        "COP2-using-the-copper-in-interlaced-mode.c13: the recorded fields alternate between the A-list and "
        "the B-list (the gate P4e), in every A-field fetch group i reads memory line 2i, and in every B-field "
        "group i reads 2i+1");
}
void g_P4d(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P4d: NOT GRADED, a precondition (PI-1 to PI-6, PI-7(bpl) or PI-12(bpl)) or the gate P4e did not hold on this run"); return; }
    checks.expect(p4d(a),
        "P4d p34 ch2-using-the-copper-in-interlaced-mode.c15 COP2-using-the-copper-in-interlaced-mode.c19: every "
        "two consecutive recorded fields (an A-field and a B-field, in either order) together fetch memory "
        "lines 0..511 each exactly once, and in the order A-group 0, B-group 0, A-group 1, ... the "
        "group addresses rise by 40 bytes at every step (memory addresses only)");
}
void g_P4e(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P4e: NOT GRADED, a precondition (PI-1 to PI-6, PI-7(bpl) or PI-12(bpl)) did not hold on this run"); return; }
    checks.expect(p4e(a),
        "P4e p34,p35 ch2-using-the-copper-in-interlaced-mode.c20.s2: "
        "over the recorded fields the restart list strictly alternates between LISTA and LISTB, so every "
        "B-field is the field directly after an A-field (order only, whichever list runs first)");
}
void g_P5(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P5: NOT GRADED, a precondition (PI-1, PI-2, the populated 20-word search, or PI-7(bpl) and PI-12(bpl) where the lists hold the BPL1PT MOVEs) did not hold on this run"); return; }
    checks.expect(p5(a),
        "P5 p27 ch2-using-the-copper-in-interlaced-mode.c16 "
        "COP2-using-the-copper-in-interlaced-mode.c15: rewriting the bitplane pointer during vertical "
        "blanking starts the retrieval of every one of the 8 recorded fields at the rewritten address");
}
void g_P6(const IAnalysis& a, bool pre) {
    if (!pre) { checks.record("P6: NOT GRADED, a precondition (PI-1, PI-2, PI-8, or PI-7(spr) and PI-12(spr) where the lists hold the SPR0PT MOVEs) did not hold on this run"); return; }
    checks.expect(p6(a),
        "P6 p27: rewriting the sprite-0 pointer during "
        "vertical blanking fetches the sprite's data in every recorded field");
}
void g_PI10(const IAnalysis& c) {
    checks.expect(pi10_ctrl_valid(c),
        "PI-10: the no-BPL1PT-rewrite run (I-c) is a valid fixture -- its completion word was written, all 8 "
        "of its M1 records are valid, its restart anchor holds and the first 20-word fetch of every one of "
        "its fields is found (PI-1 and PI-2 on that run, and the search P5-NOREWRITE reads is populated)");
}
void g_PI11(const IAnalysis& c) {
    checks.expect(pi11_ctrl_valid(c),
        "PI-11: the no-SPR0PT-rewrite run (I-e) is a valid fixture -- its completion word was written, all 8 "
        "of its M1 records are valid, its restart anchor holds, every one of its fields has 256 groups of 20 "
        "bitplane fetches and the sprite structure is fetched at least once (PI-1, PI-2, PI-4 and PI-8 on "
        "that run)");
}
void g_P5_NOREWRITE(const IAnalysis& a, const IAnalysis* c, bool pre) {
    if (!pre || !c) { checks.record("P5-NOREWRITE: NOT GRADED, a precondition (PI-1, PI-2, the populated 20-word search, or PI-7(bpl) and PI-12(bpl) where the lists hold the BPL1PT MOVEs, on the rewrite run, or PI-10 on the no-rewrite run) did not hold"); return; }
    checks.expect(p5(a) && p5_norewrite(*c),
        "P5-NOREWRITE p27 ch2-putting-together-a-copper-instruction-list.c3: the bitplane pointers must be "
        "rewritten during vertical blanking so the data is properly retrieved -- with the rewrite every "
        "recorded field starts at the rewritten address, and without it (I-c) no later recorded field "
        "starts there");
}
void g_P6_NOREWRITE(const IAnalysis& a, const IAnalysis* c, bool pre) {
    if (!pre || !c) { checks.record("P6-NOREWRITE: NOT GRADED, a precondition (PI-1, PI-2, PI-8, or PI-7(spr) and PI-12(spr) where the lists hold the SPR0PT MOVEs, on the rewrite run, or PI-11 on the no-rewrite run) did not hold"); return; }
    checks.expect(p6(a) && p6_norewrite(*c),
        "P6-NOREWRITE p27 ch2-putting-together-a-copper-instruction-list.c4: the sprite pointers must be "
        "rewritten during vertical blanking so the data is properly retrieved -- with the rewrite the "
        "sprite's data is fetched in every recorded field, and without it (I-e) no later recorded field "
        "fetches it");
}

void run_program_i_assertions(const std::string& dir) {
    IAnalysis a = analyze_i(dir, "prog_i");
    IPre p = i_pre(a);

    g_FIX_I(a);
    g_PI1(p.pi1);
    g_PI2(a, p.pi2);
    g_PI4(p.pi4);
    g_PI5(p.pi5);
    g_PI6(p.pi6);
    g_PI7bpl(p.pi7b);
    g_PI7spr(p.pi7s);
    g_PI8(p.pi8);
    g_PI12bpl(p.pi12b);
    note_pi12("PI-12(bpl)", a, kBPL1PTH, kBPL1PTL);
    g_PI12spr(p.pi12s);
    note_pi12("PI-12(spr)", a, kSPR0PTH, kSPR0PTL);
    g_PI3(p.pi3);
    g_P1(a, p1_gate(p));
    checks.record("P1: ch2-using-the-copper-in-interlaced-mode.c1 and COP2-using-the-copper-in-interlaced-mode.c1 "
        "(twice the normal number of vertical lines) are not graded -- for scan lines \"twice\" is not exact on the "
        "page itself (p34 gives PAL as 312 lines normally and 625 in interlaced mode, and 625 is not 2 x 312), and "
        "for picture lines the doubling comes from the program's data layout, graded in P4d; the concrete counts "
        "of the same page are graded in P1 and P2");
    std::vector<int> N;
    if (p1_alternation(a, &N)) {
        std::string s;
        for (size_t i = 0; i < N.size(); ++i) s += (i ? "," : "") + std::to_string(N[i]);
        checks.note("P1 field lengths N_0..N_7 = " + s);
    }

    // The LACE-clear control run (I-a), run here (not as a mutation) so that
    // ch2-vertical-beam-position.c12's "(interlace only)" quantifier gets a
    // graded line of its own, behind its own fixture precondition.
    IAnalysis ctrl = analyze_i(dir, "prog_i_a");
    g_PI9(ctrl);
    g_P1_NOLACE(a, &ctrl, p1_gate(p) && pi9_control_valid(ctrl));
    std::vector<int> ctrlN;
    if (all_lengths_equal(ctrl, &ctrlN)) checks.note("LACE=0 control: N=" + std::to_string(ctrlN[0]) + " on all 8 fields");

    g_P4a(a, p4_gate(p));
    g_P4b(a, p4_gate(p));
    g_P4c(a, p4_gate(p));
    checks.record("P4c: ch2-using-the-copper-in-interlaced-mode.c8, ch2-using-the-copper-in-interlaced-mode.c9, "
        "COP2-using-the-copper-in-interlaced-mode.c8 and COP2-using-the-copper-in-interlaced-mode.c9 are not graded "
        "here -- their on-screen part (\"displayed\", \"interlaced with\") depends on the vertical half-line offset of "
        "alternate fields (p49), the test sees video only after a line doubler and no page says what an interlaced "
        "field looks like at that output, so the page gives no expected value there; P4c grades only fetch addresses");
    checks.record("P4c: COP2-using-the-copper-in-interlaced-mode.c10 (the even-numbered lines from the second scan "
        "are interlaced with the odd-numbered lines) is not graded -- whether the lines sit between each other on "
        "the screen depends on the vertical half-line offset of alternate fields (p49), and no page says what an "
        "interlaced field looks like at the test's video output; only the retrieval part of the claim is used, in P4");
    g_P4d(a, p4_gate(p));
    checks.record("P4d: ch2-using-the-copper-in-interlaced-mode.c19 and COP2-using-the-copper-in-interlaced-mode.c18 "
        "(the organization of the data in memory matches the apparent organization on the screen) are not graded "
        "-- P4d observes only the memory addresses the bitplane DMA fetches, where the lines land on the screen is "
        "not graded (SPECIFICATION.md section 1), so a design that fetched these addresses but displayed the lines "
        "in a different screen order would pass; the memory-only claim of the same sentence (odd and even "
        "lines interlaced together in memory) stays graded in P4d");
    checks.record("P4d: implied by P4c, no separate contrast is possible -- where P4c holds (every A-field group i "
        "reads memory line 2i, every B-field group i reads 2i+1, the fields alternating) two consecutive fields fetch "
        "lines 0..511 each exactly once and the group addresses rise by 40 bytes at every step, so no run turns P4d "
        "red without turning P4c red; the contrasts shown (I-b, I-p) turn both red");
    g_P4e(a, p4e_gate(p));
    checks.record("P4e: ch2-using-the-copper-in-interlaced-mode.c11 (Figure 2-1's odd field is the field at "
        "time t) is not graded -- P4e accepts either list as the first field and ties no field to the odd "
        "field or to time t; ch2-using-the-copper-in-interlaced-mode.c12 is not graded either, its 16.6ms "
        "figure is declined (section 1) and the id bundles that figure with the ordering claim P4e does grade");
    checks.record("P4e: COP2-using-the-copper-in-interlaced-mode.c14 (the illustrated even field occurs 16.6 "
        "milliseconds after the odd field) is not graded -- the figure label gives no precision and no tolerance, "
        "at 3,579,545 Hz and 227.5 colour clocks per line 16.6 ms is 261.2 lines and no field of whole lines lasts "
        "that long; the only gradable content, that the even field is the field directly after the odd field, is "
        "graded in P4e");

    g_P5(a, p5_gate(p));
    g_P6(a, p6_gate(p));

    // The no-rewrite runs (I-c, I-e), run here (not as mutations) so that the
    // "must be rewritten" half of c3 and c4 gets a graded line of its own,
    // each behind its own fixture precondition.
    {
        IAnalysis c5 = analyze_i(dir, "prog_i_c");
        g_PI10(c5);
        g_P5_NOREWRITE(a, &c5, p5_gate(p) && pi10_ctrl_valid(c5));
        std::vector<BplGroup> firsts;
        if (p5_first_groups(c5, firsts)) {
            std::string m;
            for (size_t k = 0; k < firsts.size(); ++k) m += (k ? "," : "") + std::to_string(firsts[k].addr * 2);
            checks.note("P5-NOREWRITE: first 20-word fetch address of fields 0.." + std::to_string(kI_K - 1) +
                        " of the no-rewrite run = " + m + " (PIC = " + std::to_string(kI_PIC) + ")");
        }
    }
    {
        IAnalysis c6 = analyze_i(dir, "prog_i_e");
        g_PI11(c6);
        g_P6_NOREWRITE(a, &c6, p6_gate(p) && pi11_ctrl_valid(c6));
        if (c6.matched) {
            std::string m;
            for (size_t k = 0; k < c6.fields.size(); ++k)
                m += (k ? "," : "") + std::to_string(sprite_words_fetched_in(c6, c6.fields[k]));
            checks.note("P6-NOREWRITE: sprite data words (of 4) fetched in fields 0.." + std::to_string(kI_K - 1) +
                        " of the no-rewrite run = " + m);
        }
    }
}

// ---- Program I mutation runs (one variant program per invocation) ------

// A control line of a mutation run for a precondition: "<label>/<id>: live
// control" where it holds, NOT GRADED where it does not apply or lacks what it
// reads, FAIL where it does not hold.
void pre_ctl(const std::string& label, const std::string& id, const Pv& v) {
    if (v.ng()) checks.record(label + "/" + id + ": NOT GRADED in this run, " + v.why);
    else ctl(label, id, v.holds());
}

// Every precondition of the run as a control, except those in `skip` (the line
// the mutation targets prints through its own assertion).
void i_dump(const std::string& label, const IPre& p, const std::set<std::string>& skip) {
    struct E { const char* id; const Pv* v; };
    const E es[] = {{"PI-1", &p.pi1}, {"PI-2", &p.pi2}, {"PI-3", &p.pi3}, {"PI-4", &p.pi4}, {"PI-5", &p.pi5},
                    {"PI-6", &p.pi6}, {"PI-7(bpl)", &p.pi7b}, {"PI-7(spr)", &p.pi7s}, {"PI-8", &p.pi8}};
    for (const auto& e : es) if (!skip.count(e.id)) pre_ctl(label, e.id, *e.v);
    if (!skip.count("PI-12(bpl)")) g_PI12bpl(p.pi12b);
    if (!skip.count("PI-12(spr)")) g_PI12spr(p.pi12s);
}

// The runs the downstream lines of a mutation run need besides the mutated
// run itself: the LACE-clear run and the two no-rewrite runs, run when a gate
// is open.
struct Ctrls {
    std::string dir;
    std::map<std::string, std::unique_ptr<IAnalysis>> runs;
    const IAnalysis& get(const std::string& prog) {
        auto it = runs.find(prog);
        if (it == runs.end()) it = runs.emplace(prog, std::make_unique<IAnalysis>(analyze_i(dir, prog))).first;
        return *it->second;
    }
};

// The graded lines that read the preconditions of the mutated run, in the
// order of the normal run, each through its normal gate: NOT GRADED where the
// mutation closes the gate, PASS or FAIL where it does not. `skip` names the
// lines a mutation prints itself.
void i_downstream(Ctrls& cs, const IAnalysis& a, const IPre& p, const std::set<std::string>& skip) {
    auto want = [&](const char* id) { return !skip.count(id); };
    if (want("P1")) g_P1(a, p1_gate(p));
    if (want("P1-NOLACE")) {
        if (!p1_gate(p)) g_P1_NOLACE(a, nullptr, false);
        else { const IAnalysis& c = cs.get("prog_i_a"); g_P1_NOLACE(a, &c, pi9_control_valid(c)); }
    }
    if (want("P4a")) g_P4a(a, p4_gate(p));
    if (want("P4b")) g_P4b(a, p4_gate(p));
    if (want("P4c")) g_P4c(a, p4_gate(p));
    if (want("P4d")) g_P4d(a, p4_gate(p));
    if (want("P4e")) g_P4e(a, p4e_gate(p));
    if (want("P5")) g_P5(a, p5_gate(p));
    if (want("P6")) g_P6(a, p6_gate(p));
    if (want("P5-NOREWRITE")) {
        if (!p5_gate(p)) g_P5_NOREWRITE(a, nullptr, false);
        else { const IAnalysis& c = cs.get("prog_i_c"); g_P5_NOREWRITE(a, &c, pi10_ctrl_valid(c)); }
    }
    if (want("P6-NOREWRITE")) {
        if (!p6_gate(p)) g_P6_NOREWRITE(a, nullptr, false);
        else { const IAnalysis& c = cs.get("prog_i_e"); g_P6_NOREWRITE(a, &c, pi11_ctrl_valid(c)); }
    }
}

// Returns false if `label` names no Program-I mutation.
bool run_i_mutation(const std::string& dir, const std::string& label) {
    static const std::map<std::string, std::string> kProg = {
        {"P1", "prog_i_a"}, {"PI-3", "prog_i_a"},
        {"P1-NOLACE", "prog_i_a"}, {"P1-NOLACE(ctrl)", "prog_i"}, {"PI-9", "prog_i_o"},
        {"P4a", "prog_i_b"},
        {"P4b", "prog_i_b"}, {"P4c", "prog_i_b"}, {"P4d", "prog_i_b"}, {"P4d(step)", "prog_i_p"},
        {"P4e", "prog_i_d"},
        {"P5", "prog_i_c"}, {"P5-NOREWRITE", "prog_i_c"}, {"P5-NOREWRITE(ctrl)", "prog_i"},
        {"P6-NOREWRITE", "prog_i_e"}, {"P6-NOREWRITE(ctrl)", "prog_i"},
        {"PI-10", "prog_i_q"}, {"PI-11", "prog_i_r"},
        {"PI-7(bpl)", "prog_i_l"}, {"PI-7(spr)", "prog_i_m"},
        {"PI-12(bpl)", "prog_i_t"}, {"PI-12(spr)", "prog_i_u"},
        {"P6", "prog_i_e"},
        {"PI-1", "prog_i_j"}, {"PI-2", "prog_i_f"}, {"PI-2(long)", "prog_i_v"}, {"SELFTEST-PI-2-ANCHOR", "prog_i_s"},
        {"PI-4", "prog_i_h"},
        {"PI-5", "prog_i_g"}, {"PI-6", "prog_i_k"}, {"PI-8", "prog_i_i"},
        {"FIX-I", "prog_i_n"},
    };
    auto it = kProg.find(label);
    if (it == kProg.end()) return false;
    IAnalysis a = analyze_i(dir, it->second);
    IPre p = i_pre(a);
    Ctrls cs{dir, {}};
    // P1, PI-3 and P1-NOLACE read one comparator (the check of P1): on I-a they are red together.
    const char* kSharedP1 = "PI-3 is the check of P1 itself (same comparator): red in this run by construction, printed by the line this run targets";
    if (label == "P1") {
        ctl(label, "all 8 non-interlaced field lengths are equal", all_lengths_equal(a));
        i_dump(label, p, {"PI-3"});
        checks.record(label + "/" + kSharedP1);
        g_P1(a, p1_gate(p));
        std::vector<int> N;
        if (all_lengths_equal(a, &N)) checks.note(label + " mutation: N=" + std::to_string(N[0]) + " on all 8 fields");
    } else if (label == "PI-3") {
        // The gate PI-3 closes P4: with LACE clear, the retrieval lines of P4
        // are not graded (they would not depend on interlace).
        ctl(label, "all 8 non-interlaced field lengths are equal", all_lengths_equal(a));
        i_dump(label, p, {"PI-3"});
        g_PI3(p.pi3);
        checks.record(label + "/P1 and P1-NOLACE: the same comparator as PI-3, red in this run by construction, not printed here");
        i_downstream(cs, a, p, {"P1", "P1-NOLACE"});
        std::vector<int> N;
        if (all_lengths_equal(a, &N)) checks.note(label + " mutation: N=" + std::to_string(N[0]) + " on all 8 fields");
    } else if (label == "P1-NOLACE" || label == "P1-NOLACE(ctrl)") {
        // The interlace run and the control run are both the variant program:
        // I-a as the interlace run (the first half of the conjunction fails,
        // the second half is the live control), I-0 as the control run (the
        // second half fails, the first half is the live control).
        const IAnalysis& ctrl = a;  // the same run serves as both
        bool pi9 = pi9_control_valid(ctrl);
        i_dump(label, p, {"PI-3"});
        if (label == "P1-NOLACE") checks.record(label + "/" + kSharedP1);
        ctl(label, "PI-9", pi9);
        if (label == "P1-NOLACE") ctl(label, "the LACE-clear half", all_lengths_equal(ctrl));
        else ctl(label, "the interlace half", p1_alternation(a));
        g_P1_NOLACE(a, &ctrl, p1_gate(p) && pi9);
    } else if (label == "PI-9") {
        g_PI9(a);
        g_P1_NOLACE(a, nullptr, pi9_control_valid(a));
    } else if (label == "P4a" || label == "P4b" || label == "P4c" || label == "P4d" || label == "P4d(step)") {
        i_dump(label, p, {});
        ctl(label, "P4e", p4e(a));
        if (label == "P4a") g_P4a(a, p4_gate(p));
        else if (label == "P4b") g_P4b(a, p4_gate(p));
        else if (label == "P4c") g_P4c(a, p4_gate(p));
        else {
            g_P4d(a, p4_gate(p));
            P4dParts q = p4d_parts(a);
            checks.note(label + " mutation measured: lines 0..511 each exactly once = " + (q.set ? "yes" : "no") +
                        ", A-group i to B-group i rises by 40 = " + (q.ab ? "yes" : "no") +
                        ", B-group i to A-group i+1 rises by 40 = " + (q.ba ? "yes" : "no"));
        }
    } else if (label == "P4e") {
        i_dump(label, p, {});
        g_P4e(a, p4e_gate(p));
        g_P4a(a, p4_gate(p)); g_P4b(a, p4_gate(p)); g_P4c(a, p4_gate(p)); g_P4d(a, p4_gate(p));
    } else if (label == "PI-10") {
        g_PI10(a);
        g_P5_NOREWRITE(a, nullptr, pi10_ctrl_valid(a));
    } else if (label == "PI-11") {
        g_PI11(a);
        g_P6_NOREWRITE(a, nullptr, pi11_ctrl_valid(a));
    } else if (label == "P5-NOREWRITE" || label == "P5-NOREWRITE(ctrl)") {
        // One run serves as both: I-c as the rewrite run (the first half goes
        // red, PI-4, PI-6 and PI-7(bpl) do not apply, and the second half is
        // a live control), I-0 as the no-rewrite run (the second half goes red,
        // the first half is a live control).
        const IAnalysis& c = a;
        bool pi10 = pi10_ctrl_valid(c);
        i_dump(label, p, {});
        ctl(label, "PI-10", pi10);
        if (label == "P5-NOREWRITE") ctl(label, "the no-rewrite half", p5_norewrite(c));
        else ctl(label, "the rewrite half", p5(a));
        g_P5_NOREWRITE(a, &c, p5_gate(p) && pi10);
    } else if (label == "P6-NOREWRITE" || label == "P6-NOREWRITE(ctrl)") {
        const IAnalysis& c = a;
        bool pi11 = pi11_ctrl_valid(c);
        i_dump(label, p, {});
        ctl(label, "PI-11", pi11);
        if (label == "P6-NOREWRITE") ctl(label, "the no-rewrite half", p6_norewrite(c));
        else ctl(label, "the rewrite half", p6(a));
        g_P6_NOREWRITE(a, &c, p6_gate(p) && pi11);
    } else if (label == "P5") {
        // I-c: no BPL1PT rewrite, so there is no PI-7(bpl) to hold, and the
        // fields' fetches leave the PIC window PI-4 counts in. The first 20-word
        // fetch of every field is found wherever it lies, so the comparator goes
        // red through an address mismatch and not through an empty population.
        std::vector<BplGroup> firsts;
        bool haveAll = p5_first_groups(a, firsts);
        ctl(label, "the 20-word search populates every field", haveAll);
        i_dump(label, p, {});
        g_P5(a, p5_gate(p));
        if (haveAll && firsts.size() >= 2) {
            // p61: the next field starts at the last fetch of the previous
            // field + 2 + modulo (BPL1MOD = 40). Computed from field 0's last
            // 20-word group, recorded, not graded.
            auto groups0 = group_reads_in(a.r, kI_PIC, nmh::kChipIdentityHi, a.fields[0].begin, a.fields[0].end);
            const BplGroup* last = nullptr;
            for (const auto& g : groups0) if (g.len == 20) last = &g;
            if (last) {
                uint32_t lastFetch = (last->addr + 20u - 1u) * 2u;
                uint32_t predicted = lastFetch + 2u + 40u;
                uint32_t measured = firsts[1].addr * 2;
                checks.note(label + " mutation measured: p61's predicted first address of field 1 = " +
                            std::to_string(predicted) + ", field 1's first fetch = " + std::to_string(measured));
            }
        }
    } else if (label == "P6") {
        // I-e: no SPR0PT rewrite, so there is no PI-7(spr) to hold. PI-8
        // shows the sprite structure was fetched earlier in the same run.
        i_dump(label, p, {});
        g_P6(a, p6_gate(p));
    } else if (label == "PI-1") {
        g_PI1(p.pi1);
        i_dump(label, p, {"PI-1"});
        i_downstream(cs, a, p, {});
        if (a.m1[0].present) checks.note(label + " mutation measured: M1 record 0's first V = " + std::to_string(a.m1[0].firstV));
    } else if (label == "PI-2" || label == "PI-2(long)") {
        g_PI2(a, p.pi2);
        i_dump(label, p, {"PI-2"});
        i_downstream(cs, a, p, {});
    } else if (label == "SELFTEST-PI-2-ANCHOR") {
        // I-s: a self-test of the third clause of pi2_anchor, not a page-predicted
        // contrast. Both lists end in a MOVE the Copper cannot execute in front
        // of the terminal wait; on this design the Copper does not go on to the
        // terminal wait. Every M1 field still holds exactly one restart and the
        // matched restarts still follow each other.
        ctl(label, "every M1 field holds exactly one restart", a.anchor.listsOk && a.anchor.oneRestart == a.anchor.fields);
        ctl(label, "the matched restarts follow each other one by one", a.anchor.ordered);
        i_dump(label, p, {"PI-2"});
        checks.expect(p.pi2.holds(),
            "SELFTEST-PI-2-ANCHOR: PI-2's comparator on I-s -- not page-predicted: on this design the Copper "
            "fetches the first word of the MOVE it cannot write and nothing more of the list, so each field "
            "holds one restart and none is preceded by a read of a terminal wait; a self-test of the third clause");
        checks.note("SELFTEST-PI-2-ANCHOR measured: " + anchor_text(a.anchor));
        i_downstream(cs, a, p, {});
    } else if (label == "PI-4") {
        g_PI4(p.pi4);
        i_dump(label, p, {"PI-4"});
        i_downstream(cs, a, p, {});
    } else if (label == "PI-5") {
        g_PI5(p.pi5);
        i_dump(label, p, {"PI-5"});
        i_downstream(cs, a, p, {});
    } else if (label == "PI-6") {
        g_PI6(p.pi6);
        i_dump(label, p, {"PI-6"});
        i_downstream(cs, a, p, {});
    } else if (label == "PI-7(bpl)") {
        // PI-7(bpl) is a precondition of P4 and P5: with the MOVEs after the
        // first fetch the gates close and P4a to P4e and P5 are not graded.
        g_PI7bpl(p.pi7b);
        i_dump(label, p, {"PI-7(bpl)"});
        i_downstream(cs, a, p, {});
        if (a.matched && !a.fields.empty()) {
            const auto& f = a.fields[0];
            uint32_t base = (f.list == 'A') ? a.lay.listA : a.lay.listB;
            uint32_t off = list_move_offset(a.r, base, kBPL1PTH);
            uint64_t bplhCycle = off == UINT32_MAX ? UINT64_MAX : first_read_cycle(a.r, (base + off) / 2, f.begin, f.end);
            if (bplhCycle != UINT64_MAX && !f.bpl.empty()) {
                int64_t sinceRestartH = ((int64_t)bplhCycle - (int64_t)f.begin) / (int64_t)kClkSysPerColourClock;
                int64_t firstBplSinceRestart = ((int64_t)f.bpl.front().cycle - (int64_t)f.begin) / (int64_t)kClkSysPerColourClock;
                checks.note(label + " mutation field 0 measured: BPL1PTH fetch at " + std::to_string(sinceRestartH) +
                            " colour clocks after the restart, field's first bitplane fetch at " +
                            std::to_string(firstBplSinceRestart) + " colour clocks after the restart");
            }
        }
    } else if (label == "PI-7(spr)") {
        g_PI7spr(p.pi7s);
        i_dump(label, p, {"PI-7(spr)"});
        i_downstream(cs, a, p, {});
    } else if (label == "PI-12(bpl)") {
        g_PI12bpl(p.pi12b);
        note_pi12(label, a, kBPL1PTH, kBPL1PTL);
        ctl(label, "the gate PI-12(bpl) is closed", p.pi12b.s == Pv::Fails);
        i_dump(label, p, {"PI-12(bpl)"});
        i_downstream(cs, a, p, {});
    } else if (label == "PI-12(spr)") {
        g_PI12spr(p.pi12s);
        note_pi12(label, a, kSPR0PTH, kSPR0PTL);
        ctl(label, "the gate PI-12(spr) is closed", p.pi12s.s == Pv::Fails);
        i_dump(label, p, {"PI-12(spr)"});
        i_downstream(cs, a, p, {});
    } else if (label == "PI-8") {
        g_PI8(p.pi8);
        i_dump(label, p, {"PI-8"});
        i_downstream(cs, a, p, {});
    } else if (label == "FIX-I") {
        g_FIX_I(a);
    }
    return true;
}


//=========================================================================
// Program T: P2, P3 and their preconditions PT-1..PT-7
//=========================================================================

nmh::Config t_config(bool ntsc) {
    nmh::Config c;
    c.chipset_config = ntsc ? 0x0A : 0x08; // ECS + (bit1: NTSC=1/PAL=0)
    return c;
}

// Address of pair p's MOVE-opcode word. Pair 0 is followed by the one-time
// BPL1PTH/BPL1PTL MOVE pair (2 words each, 8 bytes total, listt_body.i),
// which shifts every later pair by a fixed +8 bytes.
uint32_t listt_move_addr(uint32_t listt, int p) {
    uint32_t off = (p == 0) ? 0u : (8u * (uint32_t)p + 8u);
    return listt + off + 4;
}

struct TField {
    int m1 = -1, restart = -1;
    uint64_t begin = 0, end = 0;
    std::vector<BplGroup> bpl;
    size_t planeReads = 0; // all reads inside PLANE during the field, including a trailing partial group
    int N = -1; // from M1
};

struct TAnalysis {
    Run r;
    uint32_t listt = kT_LISTT; // LISTT's address in this program (T-k relocates it)
    std::vector<M1Record> m1;
    uint64_t startCycle = 0;
    bool completed = false;
    std::vector<Restart> restarts;
    std::vector<TField> fields;
    bool matched = false;
    std::map<uint32_t, std::vector<uint64_t>> readsByAddr; // word_addr -> cycles, ascending
    AnchorMeasure anchor; // PT-7
};

TAnalysis analyze_t(const std::string& dir, const std::string& prog, bool ntsc) {
    TAnalysis a;
    const uint32_t listt = prog == "prog_t_k" ? kT_LISTT_LONG : kT_LISTT;
    a.listt = listt;
    const uint32_t span = prog == "prog_t_k" ? kT_LISTT_LONG_SPAN : kT_PLANE - kT_LISTT;
    a.r = run_program(dir + "/" + prog + ".bin", t_config(ntsc), kRunBudgetT, {listt});
    a.m1 = read_m1(a.r, kT_M1_RECBASE, kT_K);
    a.startCycle = start_cycle_of(a.r, kT_M1_STARTW);
    a.completed = m1_completed(a.r, kT_M1_DONEW);
    a.restarts = find_restarts(a.r, listt, 0);
    for (const auto& acc : a.r.log) if (!acc.is_write) a.readsByAddr[acc.word_addr].push_back(acc.cycle);
    for (auto& kv : a.readsByAddr) std::sort(kv.second.begin(), kv.second.end());
    auto idx = match_m1_to_restarts(a.m1, a.startCycle, a.restarts);
    a.fields.resize(kT_K);
    a.matched = true;
    for (int k = 0; k < kT_K; ++k) {
        TField f;
        f.m1 = k;
        f.restart = idx[k];
        if (f.restart < 0 || f.restart >= (int)a.restarts.size()) { a.matched = false; continue; }
        f.begin = a.restarts[(size_t)f.restart].cycle;
        f.end = interval_end(a.restarts, f.restart);
        f.bpl = chunk_reads_in(a.r, kT_PLANE, kT_PLANE_END, f.begin, f.end, 20, &f.planeReads);
        if (a.m1[(size_t)k].present) f.N = a.m1[(size_t)k].lastV + 1;
        a.fields[(size_t)k] = f;
    }
    std::vector<int> matched;
    for (const auto& f : a.fields) matched.push_back(f.restart);
    a.anchor = measure_anchor(a.r, a.m1, a.startCycle, a.restarts, matched, {{listt, span}});
    return a;
}

bool fix_t(const TAnalysis& a) {
    return ok(a.r) && W(a.r, kT_FIXMARK) && *W(a.r, kT_FIXMARK) == 0xBEEF &&
           !write_cycles_of(a.r, kT_FIXMARK, 0xBEEF).empty() &&
           write_cycles_of(a.r, kT_FIXMARK, 0xBEEF).front() < a.startCycle;
}

// The Copper starts running LISTT as soon as DMACON sets COPEN (CPU
// sequence step 5), well before M1 even starts sampling (step 7), so the
// list restarts several times before M1's own field 0 opens. The k-th
// *global* occurrence of a given address is therefore not field k's own
// mark: this looks it up within field k's own restart interval instead
// (a.fields[k].begin/.end, from M2).
std::optional<uint64_t> copper_mark(const TAnalysis& a, int k, int v) {
    if (k < 0 || k >= (int)a.fields.size()) return std::nullopt;
    uint32_t addr = listt_move_addr(a.listt, v) / 2;
    auto it = a.readsByAddr.find(addr);
    if (it == a.readsByAddr.end()) return std::nullopt;
    uint64_t begin = a.fields[(size_t)k].begin, end = a.fields[(size_t)k].end;
    for (uint64_t c : it->second) if (c >= begin && c < end) return c;
    return std::nullopt;
}

bool pt1_valid(const TAnalysis& a) { return ok(a.r) && a.completed && m1_all_valid(a.m1); }

bool pt2_groups(const TAnalysis& a) {
    if (!a.matched) return false;
    for (const auto& f : a.fields) {
        if (f.N < 44) return false;
        if ((int)f.bpl.size() != f.N - 44) return false;
        if (f.planeReads != (size_t)20 * (size_t)(f.N - 44)) return false; // no read is left over after the last group
        for (const auto& g : f.bpl) if (g.len != 20) return false;
    }
    return true;
}

bool pt7_anchor(const TAnalysis& a) { return a.m1.back().present && a.anchor.ok(); }

// Delta = m_c(k,v) - m_b(k,v) for v = 44..254; returns true and *delta if a
// single value holds everywhere it is defined.
bool pt3_delta(const TAnalysis& a, int64_t* delta_out = nullptr) {
    if (!a.matched) return false;
    bool have = false; int64_t delta = 0;
    for (int k = 0; k < kT_K; ++k) {
        const auto& f = a.fields[(size_t)k];
        if (f.N < 0) return false;
        for (int v = 44; v <= 254 && v <= f.N - 1; ++v) {
            auto mc = copper_mark(a, k, v);
            int i = v - 44;
            if (!mc || i >= (int)f.bpl.size()) return false;
            int64_t d = (int64_t)*mc - (int64_t)f.bpl[(size_t)i].cycle;
            if (!have) { delta = d; have = true; } else if (d != delta) return false;
        }
    }
    if (have && delta_out) *delta_out = delta;
    return have;
}

// Gate of PT-5 and P3: in every field the Copper mark of line 1 follows the
// mark of line 0 by 227 or 228 colour clocks, so the two marks lie on
// consecutive lines and the mark of line 0 is anchored on line 0.
bool pt4_restart_wait(const TAnalysis& a) {
    if (!a.matched) return false;
    for (int k = 0; k < kT_K; ++k) {
        auto mc0 = copper_mark(a, k, 0);
        auto mc1 = copper_mark(a, k, 1);
        if (!mc0 || !mc1) return false;
        int64_t line01 = (int64_t)*mc1 - (int64_t)*mc0;
        if (line01 != 227 * (int64_t)kClkSysPerColourClock && line01 != 228 * (int64_t)kClkSysPerColourClock) return false;
    }
    return true;
}

// line lengths, in raw clk_sys (not colour clocks: dividing before
// comparing against 227/228 would let a value anywhere in an 8-wide band
// pass as either), for v = 0..N_k-1 of field k, using the formulas of
// section 2.7. Needs field k+1's v=0 Copper mark for the last line, so
// only k = 0..K-2 are computable this way. Returns false if a mark or a
// bitplane group the formulas need is missing.
bool line_lengths(const TAnalysis& a, int k, int64_t delta, std::vector<int64_t>& out) {
    const auto& f = a.fields[(size_t)k];
    if (f.N < 0) return false;
    out.clear();
    for (int v = 0; v <= 253 && v + 1 <= f.N - 1; ++v) {
        auto c0 = copper_mark(a, k, v), c1 = copper_mark(a, k, v + 1);
        if (!c0 || !c1) return false;
        out.push_back((int64_t)*c1 - (int64_t)*c0);
    }
    for (int v = 254; v + 1 <= f.N - 1; ++v) {
        int i0 = v - 44, i1 = v + 1 - 44;
        if (i1 >= (int)f.bpl.size()) return false;
        out.push_back((int64_t)f.bpl[(size_t)i1].cycle - (int64_t)f.bpl[(size_t)i0].cycle);
    }
    // last line: v = N_k - 1
    if (k + 1 >= kT_K) return false; // needs the next field
    auto c_next0 = copper_mark(a, k + 1, 0);
    int iLast = (f.N - 1) - 44;
    if (!c_next0 || iLast < 0 || iLast >= (int)f.bpl.size()) return false;
    int64_t lastLen = (int64_t)*c_next0 - (int64_t)f.bpl[(size_t)iLast].cycle - delta;
    out.push_back(lastLen);
    return true;
}

bool length_in_range(int64_t L) {
    return L == 227 * (int64_t)kClkSysPerColourClock || L == 228 * (int64_t)kClkSysPerColourClock;
}

// Every measured line length is 227 or 228 colour clocks. Delta is the one
// value PT-3 measured; the caller grades this only when PT-3 holds.
bool pt5_line_lengths(const TAnalysis& a, int64_t delta) {
    for (int k = 0; k + 1 < kT_K; ++k) {
        std::vector<int64_t> lens;
        if (!line_lengths(a, k, delta, lens)) return false;
        for (int64_t L : lens) if (!length_in_range(L)) return false;
    }
    return true;
}

bool p2(const TAnalysis& a) {
    if (!a.matched) return false;
    for (int k = 0; k < kT_K; ++k) {
        if (a.fields[(size_t)k].N != 262 && a.fields[(size_t)k].N != 263) return false;
    }
    for (int k = 0; k + 1 < kT_K; ++k)
        if (a.fields[(size_t)k].N == 263 && a.fields[(size_t)k + 1].N == 263) return false;
    return true;
}

bool pt6_gate(const TAnalysis& a) {
    if (!a.matched) return false;
    for (int k = 0; k + 1 < kT_K; ++k) if (a.fields[(size_t)k].N == a.fields[(size_t)k + 1].N) return false;
    return true;
}

// The four-field pattern at one phase over fields 0..11; the caller grades
// this only when PT-3 holds (delta) and the gate PT-6 is open.
bool p3(const TAnalysis& a, int64_t delta, int* phase_out = nullptr) {
    // Q[phase] = (isLong, lastLineLen)
    static const bool Qlong[4]  = {false, true, false, true};
    static const int  Qlen[4]   = {227, 228, 228, 227};
    for (int phase = 0; phase < 4; ++phase) {
        bool allMatch = true;
        for (int k = 0; k <= 11 && allMatch; ++k) {
            if (k + 1 >= kT_K) { allMatch = false; break; }
            bool isLong = a.fields[(size_t)k].N == 263;
            std::vector<int64_t> lens;
            if (!line_lengths(a, k, delta, lens) || lens.empty()) { allMatch = false; break; }
            int64_t xk = lens.back();
            int slot = (k + phase) % 4;
            if (isLong != Qlong[slot] || xk != (int64_t)Qlen[slot] * (int64_t)kClkSysPerColourClock) allMatch = false;
        }
        if (allMatch) { if (phase_out) *phase_out = phase; return true; }
    }
    return false;
}

// The preconditions of Program T on one run. A precondition that reads the M1
// records (the field lengths N_k) is graded only where they are valid (PT-1);
// one that reads the restart frame only where every M1 field is matched to a
// restart; PT-3 only where PT-2 shows the bitplane marks it reads are there;
// PT-5 only where PT-3 and the gate PT-4 hold. PT-4 and PT-6 are gates and
// print as measured states.
struct TPre {
    Pv pt1, pt2, pt3, pt5, pt7;
    bool pt4 = false, pt6 = false;
    int64_t delta = 0;
};
TPre t_pre(const TAnalysis& a) {
    TPre p;
    const std::string noM1 = "the M1 records are not valid (PT-1), so the field lengths N_k are not known";
    p.pt1 = graded(pt1_valid(a));
    bool frame = p.pt1.holds() && a.matched;
    std::string noFrame = p.pt1.holds() ? "no M1 field is matched to a restart: the run has no restart frame" : noM1;
    p.pt2 = frame ? graded(pt2_groups(a)) : not_graded(noFrame);
    p.pt3 = p.pt2.holds() ? graded(pt3_delta(a, &p.delta)) : not_graded("PT-2 does not hold: the bitplane marks of the fields are not there");
    p.pt4 = pt4_restart_wait(a);
    p.pt5 = (p.pt3.holds() && p.pt4) ? graded(pt5_line_lengths(a, p.delta))
                                     : not_graded("PT-3 (the offset Delta the last-line length needs) or the gate PT-4 (which anchors the line-0 marks) did not hold");
    p.pt6 = p.pt1.holds() && pt6_gate(a);
    p.pt7 = p.pt1.holds() ? graded(pt7_anchor(a)) : not_graded(noM1);
    return p;
}

// ---- Program T assertions: one function per label, used by the normal
// run and by that label's mutation run alike ----------------------------

void g_FIX_T(const TAnalysis& a) {
    checks.expect(fix_t(a),
                  "FIX-T: Program T stores and delivers a known constant (BEEF) before M1 records anything");
}
void g_PT1(const Pv& v) {
    pre_line(v,
        "PT-1: Program T's fixture -- the completion word was written and all 13 M1 records are valid");
}
void g_PT2(const Pv& v) {
    pre_line(v,
        "PT-2: in every field, the bitplane fetches are exactly N_k - 44 groups of 20 consecutive ascending "
        "words each, with no fetch left over, independently confirming N_k from the bitplane fetches");
}
void g_PT3(const Pv& v) {
    pre_line(v,
        "PT-3: the Copper-mark-to-bitplane-mark offset Delta has one single value on every line 44..254 "
        "of every field");
}
// PT-4 is a gate, not a graded line: p25 leaves open whether the restart lies
// at the start or at the end of vertical blanking, and an implementation that
// restarts at the end of blanking satisfies the (0,$E0) WAIT at once. The gate
// state is printed; PT-5 and P3 are NOT GRADED when it is closed.
void g_PT4(const TAnalysis& a) {
    bool open = pt4_restart_wait(a);
    int64_t lo = INT64_MAX, hi = INT64_MIN;
    int have = 0;
    if (a.matched) {
        for (int k = 0; k < kT_K; ++k) {
            auto mc0 = copper_mark(a, k, 0);
            auto mc1 = copper_mark(a, k, 1);
            if (!mc0 || !mc1) continue;
            int64_t d = (int64_t)*mc1 - (int64_t)*mc0;
            lo = std::min(lo, d); hi = std::max(hi, d); ++have;
        }
    }
    std::string m = std::string("PT-4 gate of PT-5 and P3 (in every field the Copper marks of line 0 and line 1 "
                                "are 227 or 228 colour clocks apart): ") + (open ? "open" : "closed") +
                    "; line 0-to-1 measured in " + std::to_string(have) + " of " + std::to_string(kT_K) + " fields";
    if (have) m += ", " + std::to_string(lo) + " to " + std::to_string(hi) + " clk_sys";
    checks.note(m);
}
void g_PT5(const TPre& t) {
    if (t.pt5.ng()) { checks.record("PT-5: NOT GRADED, a precondition (PT-3, the offset Delta the last-line length needs, or PT-4, the gate that anchors the line-0 marks) did not hold on this run"); return; }
    checks.expect(t.pt5.holds(),
        "PT-5: every measured line length (section 2.7) is 227 or 228 colour clocks");
}
void g_PT7(const TAnalysis& a, const Pv& v) {
    pre_line(v,
        "PT-7: each of the 13 recorded M1 fields holds exactly one read of LISTT's first word between the "
        "writes that open and close it, the restart intervals the fields are matched to follow each other "
        "one by one in field order, and the Copper's last read of LISTT's words up to its terminal "
        "$FFFF,$FFFE wait before each such read is a read of that wait (section 2.6)");
    if (!v.ng()) checks.note("PT-7 measured: " + anchor_text(a.anchor));
}
void g_P2(const TAnalysis& a, bool pre) {
    if (!pre) { checks.record("P2: NOT GRADED, a precondition (PT-1, PT-2 or PT-7) did not hold on this run"); return; }
    checks.expect(p2(a),
        "P2 p24,p34: in all 13 recorded NTSC fields, every field has 262 or 263 lines, and no two consecutive "
        "fields are both 263");
}
// PT-6 is a gate, not a graded line: p34 reads the NTSC fields as all 262
// lines, and an implementation that follows p34 closes the gate. P3 reports
// "not graded, gate closed" then; that is not a failure.
void g_PT6(const TAnalysis& a, bool open) {
    std::string lens;
    for (int k = 0; k < kT_K; ++k) lens += (k ? "," : "") + (a.fields[(size_t)k].N < 0 ? std::string("?") : std::to_string(a.fields[(size_t)k].N));
    checks.note(std::string("PT-6 gate of P3 (consecutive field lengths alternate between 262 and 263): ") +
                (open ? "open" : "closed") + "; N_0..N_" + std::to_string(kT_K - 1) + " = " + lens);
}
// What the P3 verdict rests on, computed from this run: Delta, the type f_k and
// the last-line length x_k of every graded field, and how many of the measured
// line lengths of those fields are 227 and 228 colour clocks.
std::string cc_text(int64_t clk) {
    return clk % (int64_t)kClkSysPerColourClock == 0 ? std::to_string(clk / (int64_t)kClkSysPerColourClock)
                                                     : std::to_string(clk) + "clk_sys";
}
void p3_measured(const TAnalysis& a, int64_t delta) {
    checks.note("P3 measured: Delta = " + std::to_string(delta) + " clk_sys (" + cc_text(delta) + " colour clocks)");
    std::string fx;
    size_t n227 = 0, n228 = 0, other = 0, total = 0;
    for (int k = 0; k <= 11; ++k) {
        std::vector<int64_t> lens;
        std::string f = a.fields[(size_t)k].N == 262 ? "short" : a.fields[(size_t)k].N == 263 ? "long"
                        : "N=" + std::to_string(a.fields[(size_t)k].N);
        if (!line_lengths(a, k, delta, lens) || lens.empty()) { fx += (k ? " " : "") + std::string("(") + f + ",?)"; continue; }
        fx += (k ? " " : "") + std::string("(") + f + "," + cc_text(lens.back()) + ")";
        for (int64_t L : lens) {
            ++total;
            if (L == 227 * (int64_t)kClkSysPerColourClock) ++n227;
            else if (L == 228 * (int64_t)kClkSysPerColourClock) ++n228;
            else ++other;
        }
    }
    checks.note("P3 measured: (f_k, x_k in colour clocks) for k = 0..11 = " + fx);
    checks.note("P3 measured: of " + std::to_string(total) + " line lengths in fields 0..11, " + std::to_string(n227) +
                " are 227 colour clocks, " + std::to_string(n228) + " are 228 and " + std::to_string(other) + " are neither");
}
void g_P3(const TAnalysis& a, const TPre& t) {
    if (!t.pt6) { checks.record("P3: not graded, gate closed (PT-6 did not hold on this run)"); return; }
    if (!(t.pt1.holds() && t.pt2.holds() && t.pt3.holds() && t.pt4 && t.pt5.holds() && t.pt7.holds())) {
        checks.record("P3: NOT GRADED, a precondition (PT-1 to PT-5 or PT-7) did not hold on this run");
        return;
    }
    p3_measured(a, t.delta);
    int phase = -1;
    bool p3ok = p3(a, t.delta, &phase);
    checks.expect(p3ok,
        "P3 p24 ch2-vertical-beam-position.c15 ch2-vertical-beam-position.c16 "
        "ch2-vertical-beam-position.c17 ch2-vertical-beam-position.c18 ch2-vertical-beam-position.c19 "
        "COP2-vertical-beam-position.c17 COP2-vertical-beam-position.c18 COP2-vertical-beam-position.c19 "
        "COP2-vertical-beam-position.c20 COP2-vertical-beam-position.c21 COP2-vertical-beam-position.c22: "
        "the NTSC 4-field pattern (short/short-line, long/long-line, short/long-line, long/short-line) "
        "repeats for 3 whole cycles at one fixed phase");
    if (p3ok) checks.note("P3 phase = " + std::to_string(phase));
}

void run_program_t_assertions(const std::string& dir) {
    TAnalysis a = analyze_t(dir, "prog_t", /*ntsc=*/true);
    TPre t = t_pre(a);

    g_FIX_T(a);
    g_PT1(t.pt1);
    g_PT2(t.pt2);
    g_PT3(t.pt3);
    g_PT4(a);
    g_PT5(t);
    g_PT7(a, t.pt7);
    g_P2(a, t.pt1.holds() && t.pt2.holds() && t.pt7.holds());
    checks.record("P2: COP2-vertical-beam-position.c13 (\"the first field length ... is 262\") is not graded -- "
        "no page fixes which of the recorded fields is the first of the alternation (the capture opens at a wrap "
        "that no page ties to a field type), so the claim has no field to be graded on; P2 allows 262 for every field "
        "and requires no order");
    checks.record("P2: COP2-vertical-beam-position.c14 (\"the next field length ... is 263\"), "
        "COP2-using-the-copper-in-interlaced-mode.c3 (\"an interlaced NTSC display has 524 lines\") and "
        "COP2-using-the-copper-in-interlaced-mode.c7 (\"262 lines at a time\") are not graded -- the pages disagree: "
        "p24 gives \"262, then 263 lines\" (525 in all, as do Table 3-13 on p79 and p304) against .c3's 524 and .c7's 262 "
        "at a time, and p34's \"262 lines at a time\" against COP2-vertical-beam-position.c14; only the part both "
        "readings agree on is graded, in P2: every field has 262 or 263 lines and no two consecutive fields are both 263");
    g_PT6(a, t.pt6);
    g_P3(a, t);
}

struct TMutSpec { std::string prog; bool ntsc; };

// Every precondition of the run as a control, except those in `skip` (the line
// the mutation targets prints through its own assertion); the gates PT-4 and
// PT-6 print as their measured states.
void t_dump(const std::string& label, const TAnalysis& a, const TPre& t, const std::set<std::string>& skip) {
    if (!skip.count("PT-1")) pre_ctl(label, "PT-1", t.pt1);
    if (!skip.count("PT-2")) pre_ctl(label, "PT-2", t.pt2);
    if (!skip.count("PT-3")) pre_ctl(label, "PT-3", t.pt3);
    if (!skip.count("PT-4")) g_PT4(a);
    if (!skip.count("PT-5")) pre_ctl(label, "PT-5", t.pt5);
    if (!skip.count("PT-6")) g_PT6(a, t.pt6);
    if (!skip.count("PT-7")) pre_ctl(label, "PT-7", t.pt7);
}

// The graded lines that read the preconditions of the mutated run, each
// through its normal gate.
void t_downstream(const TAnalysis& a, const TPre& t, const std::set<std::string>& skip) {
    if (!skip.count("P2")) g_P2(a, t.pt1.holds() && t.pt2.holds() && t.pt7.holds());
    if (!skip.count("P3")) g_P3(a, t);
}

// Returns false if `label` names no Program-T mutation.
bool run_t_mutation(const std::string& dir, const std::string& label) {
    static const std::map<std::string, TMutSpec> kProg = {
        {"P2", {"prog_t", false}},
        {"P3", {"prog_t", false}},
        {"PT-1", {"prog_t_f", true}},
        {"PT-2", {"prog_t_e", true}},
        {"PT-3", {"prog_t_d", true}},
        {"PT-4", {"prog_t_c", true}},
        {"PT-5", {"prog_t_g", true}},
        {"PT-6", {"prog_t_a", true}},
        {"PT-7", {"prog_t_h", true}},
        {"PT-7(long)", {"prog_t_k", true}},
        {"SELFTEST-PT-7-ANCHOR", {"prog_t_j", true}},
        {"FIX-T", {"prog_t_i", true}},
    };
    auto it = kProg.find(label);
    if (it == kProg.end()) return false;
    TAnalysis a = analyze_t(dir, it->second.prog, it->second.ntsc);
    TPre t = t_pre(a);
    if (label == "P2") {
        t_dump(label, a, t, {});
        g_P2(a, t.pt1.holds() && t.pt2.holds() && t.pt7.holds());
    } else if (label == "P3") {
        // P3 is already red in the normal run. The comparator is shown with
        // every precondition live, in PAL, where p23 gives every line as 227
        // colour clocks and no field has 263 lines.
        t_dump(label, a, t, {"PT-6"});
        ctl(label, "PT-6 (the gate; PAL fields alternate 312, 313)", t.pt6);
        g_P3(a, t);
        bool anyLongField = false;
        for (const auto& f : a.fields) if (f.N == 263) anyLongField = true;
        checks.note(label + " mutation measured: any field with N=263? " + (anyLongField ? "yes" : "no"));
        checks.record(label + " mutation: no input built in this package turns P3's own comparator green "
            "-- unfalsifiable at this boundary while NTSC never produces a long line and PAL never "
            "produces a 263-line field");
    } else if (label == "PT-1") {
        g_PT1(t.pt1);
        t_dump(label, a, t, {"PT-1"});
        t_downstream(a, t, {});
        if (a.m1[0].present) checks.note(label + " mutation measured: M1 record 0's first V = " + std::to_string(a.m1[0].firstV));
    } else if (label == "PT-2") {
        g_PT2(t.pt2);
        t_dump(label, a, t, {"PT-2"});
        t_downstream(a, t, {});
    } else if (label == "PT-3") {
        g_PT3(t.pt3);
        t_dump(label, a, t, {"PT-3"});
        t_downstream(a, t, {});
    } else if (label == "PT-4") {
        // T-c: the gate closes, so PT-5 and P3 are NOT GRADED.
        t_dump(label, a, t, {"PT-4"});
        g_PT4(a);
        ctl(label, "the gate PT-4 is closed", !t.pt4);
        t_downstream(a, t, {});
    } else if (label == "PT-5") {
        // T-g: line 10's WAIT uses H=$40, so every Copper mark stays live and
        // the length of line 9 and of line 10 change. Line 10 lies below line
        // 44, so Delta (lines 44..254) is not touched.
        bool allMarks = t.pt3.holds();
        size_t outside = 0, measured = 0;
        std::string first;
        for (int k = 0; k + 1 < kT_K && t.pt3.holds(); ++k) {
            std::vector<int64_t> lens;
            if (!line_lengths(a, k, t.delta, lens)) { allMarks = false; continue; }
            for (size_t v = 0; v < lens.size(); ++v) {
                ++measured;
                if (!length_in_range(lens[v])) {
                    ++outside;
                    if (first.empty())
                        first = "field " + std::to_string(k) + " line " + std::to_string(v) + " = " +
                                std::to_string(lens[v]) + " clk_sys (" + std::to_string(lens[v] / (int64_t)kClkSysPerColourClock) +
                                " colour clocks)";
                }
            }
        }
        t_dump(label, a, t, {"PT-5"});
        ctl(label, "every mark the line lengths need is present", allMarks);
        g_PT5(t);
        checks.note(label + " mutation measured: " + std::to_string(outside) + " of " + std::to_string(measured) +
                    " line lengths outside 227 and 228 colour clocks; first: " + (first.empty() ? "none" : first));
        t_downstream(a, t, {});
    } else if (label == "PT-6") {
        // T-a: the gate closes, so P3 reports "not graded, gate closed". P2 is not
        // printed: a non-interlaced display is not the interlaced display it grades.
        t_dump(label, a, t, {"PT-6"});
        g_PT6(a, t.pt6);
        ctl(label, "the gate PT-6 is closed", !t.pt6);
        checks.record(label + "/P2: not printed in this run, LACE is clear, so the display is not the interlaced display P2 grades");
        t_downstream(a, t, {"P2"});
    } else if (label == "PT-7" || label == "PT-7(long)") {
        g_PT7(a, t.pt7);
        t_dump(label, a, t, {"PT-7"});
        t_downstream(a, t, {});
    } else if (label == "SELFTEST-PT-7-ANCHOR") {
        // T-j: a self-test of the third clause of pt7_anchor, not a page-predicted
        // contrast (as I-s): LISTT ends in a MOVE the Copper cannot execute in
        // front of the terminal wait.
        ctl(label, "every M1 field holds exactly one restart", a.anchor.listsOk && a.anchor.oneRestart == a.anchor.fields);
        ctl(label, "the matched restarts follow each other one by one", a.anchor.ordered);
        t_dump(label, a, t, {"PT-7"});
        checks.expect(t.pt7.holds(),
            "SELFTEST-PT-7-ANCHOR: PT-7's comparator on T-j -- not page-predicted: on this design the Copper "
            "fetches the first word of the MOVE it cannot write and nothing more of the list, so each field "
            "holds one restart and none is preceded by a read of a terminal wait; a self-test of the third clause");
        checks.note("SELFTEST-PT-7-ANCHOR measured: " + anchor_text(a.anchor));
        t_downstream(a, t, {});
    } else if (label == "FIX-T") {
        g_FIX_T(a);
    }
    return true;
}

} // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));

    if (!g_mutate) {
        run_program_i_assertions(dir);
        run_program_t_assertions(dir);
    } else {
        std::string label = g_mutate;
        if (!run_i_mutation(dir, label) && !run_t_mutation(dir, label))
            checks.record("NMH_MUTATE=" + label + " names no mutation of this package");
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
