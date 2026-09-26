// test_ch5_audio_output_scanrate.cpp -- see SPECIFICATION.md.
//
// P1-P4: the audio program (prog_audio.s) plays tables T1, T2, T3 in
// sequence; table content is poked directly into chip RAM (the same
// "direct memory preload" mechanism load_program() itself uses), so one
// capture's ram_log()+audio() trace serves all four propositions and
// their shared preconditions A1-A4. P5: prog_adkcon.s. P6/F1/F3:
// prog_diskrate.s.
//
// This design's paula_audio_mixer.v routes channels 0 and 3 to
// ldatasum ("channel 0&3 --> left"), the opposite of p.137's "Channels 0
// and 3 are connected to the right-side output jack." No P1-P6 assertion
// depends on which named wire carries channel 0, only that channel 0's
// own bytes are observed in order, so this test reads audio_left, the
// wire this design actually routes channel 0 to; see SPECIFICATION.md
// section 5.
//
// NMH_MUTATE selects one mutated program or table (or, for FASTF0/
// FASTS1, one capture graded two different ways) in place of its normal
// counterpart; unset, every section runs its normal binary/table and the
// summary counts the fixture, preconditions and assertions only.
//
// MUTATION P1 stimulus red T1's last word $6070 -> $7060, via NMH_MUTATE=P1
// MUTATION P2 stimulus red every word of T1 byte-swapped, via NMH_MUTATE=P2
// MUTATION P3 stimulus red every byte of T2 with bit 7 flipped, via NMH_MUTATE=P3
// MUTATION P4 stimulus red T3's word for code +5 (k=133) $0505 -> $0707, via NMH_MUTATE=P4
// MUTATION A1 for T1 stimulus red DMACON $8200 instead of $8201 (AUD0EN not set), via NMH_MUTATE=A1
// MUTATION A2 stimulus red ADKCON $8001 (USE0V1) set before the ADKCONR read, via NMH_MUTATE=A2
// MUTATION A4 for T1 stimulus red AUD0VOL 0 instead of 64, via NMH_MUTATE=A4
// MUTATION A3 not exercised, by a scope decision: this harness's audio() capture is one sample per
//   clk_sys cycle (about 28.375 MHz effective rate), far above the ~114000 samples/second threshold
//   at which P=124 would fail A3 (SPECIFICATION.md 2.7); no legal period fails A3 on this harness.
// MUTATION P5 stimulus red step 2 writes $8200 instead of $8100, via NMH_MUTATE=P5
// MUTATION C1 stimulus red the store of R2 (step 3's readback) left out, via NMH_MUTATE=C1
// MUTATION G1/B1 stimulus red run F's ADKCON write becomes $0100 instead of $8100 (same capture,
//   graded once as G1's contrasting case and once as B1's), via NMH_MUTATE=FASTF0
// MUTATION G2/G3 stimulus red run S's ADKCON write becomes $8100 instead of $0100 (same capture,
//   graded once as G2's contrasting case and once as G3's), via NMH_MUTATE=FASTS1
// MUTATION B2 stimulus red run F's second (step 6) DSKLEN write left out, via NMH_MUTATE=B2
// MUTATION B3 stimulus red run F's LENGTH shrunk to 100 (DSKLEN=$C064) in steps 5 and 6, via NMH_MUTATE=B3
// MUTATION B4 (F1's own contrasting case) attempted, not reproduced: VTOTAL=524/HTOTAL=113/
//   BEAMCON0 rewritten before run F (ECS only), via NMH_MUTATE=F1, exactly the stimulus
//   SPECIFICATION.md 2.4 states. The refresh-group anchor still measures 227.00 color clocks
//   (unchanged); disk DMA's own timing does measurably change under the same write (confirmed at
//   an untried HTOTAL=20 too), consistent with disk DMA's higher arbitration priority over
//   refresh (agnus.v) starving refresh's grants back down near the original rate. Reported, not
//   worked around; see the RECORDED line this run prints and SPECIFICATION.md section 5.

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cmath>
#include <string>
#include <vector>
#include <optional>
#include <algorithm>
#include <iostream>

#include "nanomig_harness.h"

#ifndef NANOMIG_CORE_NAME
#define NANOMIG_CORE_NAME unknown
#endif
#define NMH_STR(x) #x
#define NMH_XSTR(x) NMH_STR(x)

// ---- the assertion call form (BRIEF.md) --------------------------------
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

namespace {

// ---- nanomig.i, restated -------------------------------------------------
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneValue = 0x600D;

constexpr uint64_t kBootBudget      = 6'000'000;
constexpr uint64_t kAudioRunBudget  = 8'000'000;
constexpr uint64_t kAdkconRunBudget = 2'000'000;
constexpr uint64_t kDiskRunBudget   = 20'000'000;

// clk_sys cycles per color clock (amiga_clk.v: clk7_cnt is a mod-4
// counter; cck toggles once per clk7_cnt==1, so a full cck period is
// 8 clk_28 cycles, and nanomig.v:1571 ties clk28m to clk_sys).
constexpr uint64_t kCck = 8;
constexpr double   kUsPerCckPal  = 0.281937;   // p.141
constexpr double   kUsPerCckNtsc = 0.279365;   // p.141

constexpr uint32_t T1_ADDR = 0x002000;
constexpr uint32_t T2_ADDR = 0x002100;
constexpr uint32_t T3_ADDR = 0x002200;
constexpr uint32_t DISKBUF_A = 0x003000;
constexpr uint32_t DISKBUF_B = 0x004000;

constexpr int kNT1 = 8, kNT2 = 8, kNT3 = 2;   // N (2.6 table)

// ---- table content (P1-P4's own data mutations live here; see
// prog_audio.s's header) --------------------------------------------------
std::vector<uint16_t> t1_table(const std::string& mv) {
    if (mv == "P1") return {0x0010, 0x2030, 0x4050, 0x7060};
    if (mv == "P2") return {0x1000, 0x3020, 0x5040, 0x7060};
    return {0x0010, 0x2030, 0x4050, 0x6070};
}
std::vector<uint16_t> t2_table(const std::string& mv) {
    if (mv == "P3") return {0x8080, 0x8181, 0xFFFF, 0x0000};
    return {0x0000, 0x0101, 0x7F7F, 0x8080};
}
std::vector<uint16_t> t3_table(const std::string& mv) {
    std::vector<uint16_t> t(256);
    for (int k = 0; k < 256; ++k) {
        uint8_t b = (uint8_t)((k + 0x80) & 0xFF);
        t[k] = (uint16_t)((b << 8) | b);
    }
    if (mv == "P4") t[133] = 0x0707;   // code +5 (k=133): $0505 -> $0707
    return t;
}

std::vector<int> c_up()  { return {0,16,32,48,64,80,96,112}; }
std::vector<int> c_low() { return {16,0,48,32,80,64,112,96}; }
std::vector<int> c2()    { return {0,1,127,-128}; }
std::vector<int> c3() {
    std::vector<int> v(256);
    for (int k = 0; k < 256; ++k) v[k] = k - 128;
    return v;
}

// ---- Config helpers -------------------------------------------------------
nmh::Config cfg_ecs_pal()  { nmh::Config c; c.chipset_config = 0x08; return c; }
nmh::Config cfg_ocs_pal()  { nmh::Config c; c.chipset_config = 0x00; return c; }
nmh::Config cfg_ecs_ntsc() { nmh::Config c; c.chipset_config = 0x0A; return c; }

void poke_table(nmh::NanomigHarness& h, uint32_t byte_addr, const std::vector<uint16_t>& words) {
    for (size_t i = 0; i < words.size(); ++i) h.poke_word(byte_addr / 2 + (uint32_t)i, words[i]);
}
void poke_fill(nmh::NanomigHarness& h, uint32_t byte_addr, uint16_t value, uint32_t count) {
    for (uint32_t i = 0; i < count; ++i) h.poke_word(byte_addr / 2 + i, value);
}

// ---- F2: audio pass boundaries (2.4) -------------------------------------
struct F2Result {
    bool anchor_ok = false;
    std::vector<uint64_t> pass_starts;   // t_1, t_2, ... (cycle of each read of the table's first word)
    std::string diag;   // set only when the anchor breaks, for MEASURED reporting
};

// F2 (2.4): "Each read of the first word EXCEPT t_1 is immediately
// preceded ... by a read of the table's last word." p.164's own pipeline
// throw-away means pass 1 (between t_1 and t_2) can be short or
// irregular; only t_2 onward is validated strictly. Empirically (see
// SPECIFICATION.md's build notes) pass 1 is exactly one word short on
// this RTL: t_1's pass reads lo, lo+1, ..., hi-1 and restarts one word
// early. Since the window graded (F2, 2.6) starts at t_2, pass 1's own
// content is never used and does not need to be well-formed.
F2Result f2_check(const std::vector<nmh::RamAccess>& log, uint32_t lo_word, uint32_t hi_word) {
    F2Result r;
    bool strict = false;   // becomes true once t_2 (the second read of lo_word) is seen
    uint32_t expect = lo_word;
    bool any = false;
    for (const auto& a : log) {
        if (a.is_write || a.word_addr < lo_word || a.word_addr > hi_word) continue;
        any = true;
        if (a.word_addr == lo_word) {
            r.pass_starts.push_back(a.cycle);
            if (r.pass_starts.size() >= 2) strict = true;
            expect = lo_word + 1;
            continue;
        }
        if (strict && a.word_addr != expect) {
            char buf[160];
            std::snprintf(buf, sizeof buf,
                "expected word_addr=0x%X, got 0x%X (data=0x%04X) at cycle %llu, after %zu reads of lo",
                expect, a.word_addr, a.data, (unsigned long long)a.cycle, r.pass_starts.size());
            r.diag = buf;
            r.anchor_ok = false;
            return r;
        }
        expect = a.word_addr + 1;
    }
    r.anchor_ok = any;
    return r;
}

// ---- LEVELS(W) and FIT(L, C) (2.5) ---------------------------------------
// audio_left/audio_right are "15 bits significant" (nanomig_harness.h):
// paula_audio_mixer.v sums two channels' already-signed sample*volume
// values with a one-bit sign extension (ldatasum <= {msample0[13],
// msample0} + {msample3[13], msample3}), so the port's 15-bit value is
// itself two's complement, not an unsigned magnitude. A negative sum
// (any pass containing a code with bit 7 set) otherwise reads back as a
// spuriously huge positive number and breaks every fit.
int32_t signed_sample(uint16_t raw) {
    int32_t v = raw & 0x7FFF;
    if (v & 0x4000) v -= 0x8000;
    return v;
}

struct RawRun { int32_t value; uint64_t len; };

std::vector<RawRun> raw_runs(const std::vector<nmh::AudioSample>& audio, uint64_t begin, uint64_t end) {
    std::vector<RawRun> runs;
    for (uint64_t i = begin; i < end && i < audio.size(); ++i) {
        int32_t v = signed_sample(audio[i].left);
        if (!runs.empty() && runs.back().value == v) ++runs.back().len;
        else runs.push_back({v, 1});
    }
    return runs;
}

double median_run_len(const std::vector<RawRun>& runs) {
    if (runs.empty()) return 0.0;
    std::vector<uint64_t> lens;
    for (auto& r : runs) lens.push_back(r.len);
    std::sort(lens.begin(), lens.end());
    return (double)lens[lens.size() / 2];
}

std::vector<int32_t> levels_from_runs(const std::vector<RawRun>& runs, double median) {
    std::vector<int32_t> merged;
    double half = median / 2.0;
    for (auto& r : runs) {
        if ((double)r.len < half) continue;   // a transition, not a held level
        if (!merged.empty() && merged.back() == r.value) continue;   // merge neighbours
        merged.push_back(r.value);
    }
    if (merged.size() <= 2) return {};
    return std::vector<int32_t>(merged.begin() + 1, merged.end() - 1);   // drop first/last
}

struct FitResult { bool matches = false; double a = 0, b = 0; int rotation = -1; };

// Returns the BEST rotation's (a, b) even when no rotation satisfies the
// quarter-LSB tolerance: A4 (2.7) grades the fitted |b| ("is there a
// resolvable amplitude") independently of whether the pattern matches
// the expected code sequence, so a proposition's own mutated data (which
// legitimately breaks the match) must not also silently break A4.
// `matches` is the actual FIT(L, C) verdict P1-P4 grade.
FitResult fit(const std::vector<int32_t>& L, const std::vector<int>& s) {
    FitResult best;
    double best_ratio = 1e300;
    size_t K = s.size();
    if (L.empty() || K == 0) return best;
    for (size_t r = 0; r < K; ++r) {
        double n = (double)L.size();
        double sx = 0, sy = 0, sxx = 0, sxy = 0;
        for (size_t i = 0; i < L.size(); ++i) {
            double x = (double)s[(i + r) % K];
            double y = (double)L[i];
            sx += x; sy += y; sxx += x * x; sxy += x * y;
        }
        double denom = n * sxx - sx * sx;
        if (std::fabs(denom) < 1e-9) continue;
        double b = (n * sxy - sx * sy) / denom;
        double a = (sy - b * sx) / n;
        double max_resid = 0;
        for (size_t i = 0; i < L.size(); ++i) {
            double x = (double)s[(i + r) % K];
            double resid = std::fabs((double)L[i] - a - b * x);
            if (resid > max_resid) max_resid = resid;
        }
        double ratio = (std::fabs(b) > 1e-9) ? (max_resid / std::fabs(b)) : 1e300;
        if (ratio < best_ratio) {
            best_ratio = ratio;
            best = {ratio <= 0.25, a, b, (int)r};
        }
    }
    return best;
}

// ---- F1: refresh-group time base (2.4), used by P6 -----------------------
struct F1Result { bool anchor_ok = false; double color_clocks = 0.0; size_t groups = 0; };

F1Result f1_check(const std::vector<nmh::RamAccess>& log) {
    F1Result r;
    std::vector<uint64_t> refresh;
    for (const auto& a : log) if (!a.is_write && a.word_addr == 0) refresh.push_back(a.cycle);
    std::vector<uint64_t> groups;
    constexpr uint64_t kGroupGap = 100;   // within-group ~4*kCck=32 cycles; between-group ~227.5*kCck
    for (size_t i = 0; i < refresh.size(); ++i)
        if (i == 0 || refresh[i] - refresh[i - 1] > kGroupGap) groups.push_back(refresh[i]);
    r.groups = groups.size();
    if (groups.size() < 101) return r;
    double total = 0;
    for (size_t i = 0; i < 100; ++i) total += (double)(groups[i + 1] - groups[i]);
    double mean_cycles = total / 100.0;
    r.color_clocks = mean_cycles / (double)kCck;
    r.anchor_ok = (r.color_clocks >= 227.0 && r.color_clocks <= 228.0);
    return r;
}

// ---- F3: disk word index (2.4), used by P6 -------------------------------
struct F3Result { bool anchor_ok = false; std::vector<uint64_t> times; };

F3Result f3_check(const std::vector<nmh::RamAccess>& log, uint32_t base_word, uint32_t max_words) {
    F3Result r;
    uint32_t expect = base_word;
    for (const auto& a : log) {
        if (a.is_write || a.word_addr < base_word || a.word_addr >= base_word + max_words) continue;
        if (a.word_addr != expect) { r.anchor_ok = false; return r; }
        r.times.push_back(a.cycle);
        ++expect;
        if (expect >= base_word + max_words) break;
    }
    r.anchor_ok = !r.times.empty();
    return r;
}

// ---- audio program run ----------------------------------------------------
struct AudioCapture {
    bool loaded = false, released = false;
    std::optional<uint64_t> done;
    std::optional<uint16_t> adkconr;
    std::vector<nmh::RamAccess> log;
    std::vector<nmh::AudioSample> audio;
};

AudioCapture run_audio(const std::string& dir, const std::string& mv, const nmh::Config& cfg) {
    AudioCapture r;
    std::string bin = dir + "/prog_audio.bin";
    if (mv == "A1") bin = dir + "/prog_audio_a1mut.bin";
    else if (mv == "A2") bin = dir + "/prog_audio_a2mut.bin";
    else if (mv == "A4") bin = dir + "/prog_audio_a4mut.bin";

    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;

    poke_table(h, T1_ADDR, t1_table(mv));
    poke_table(h, T2_ADDR, t2_table(mv));
    poke_table(h, T3_ADDR, t3_table(mv));

    r.released = h.boot(kBootBudget).cpu_released;
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kAudioRunBudget);
    r.adkconr = h.peek_logical(kResults);
    r.log = nmh::coalesce(h.ram_log());
    r.audio = h.audio();
    return r;
}

// ---- adkcon program run ----------------------------------------------------
struct AdkconCapture {
    bool loaded = false;
    std::optional<uint64_t> done;
    std::optional<uint16_t> r0, r1, r2, r3, r4;
    std::vector<nmh::RamAccess> log;
};

AdkconCapture run_adkcon(const std::string& dir, const std::string& mv, const nmh::Config& cfg) {
    AdkconCapture r;
    std::string bin = dir + "/prog_adkcon.bin";
    if (mv == "P5") bin = dir + "/prog_adkcon_p5mut.bin";
    else if (mv == "C1") bin = dir + "/prog_adkcon_c1mut.bin";

    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    h.boot(kBootBudget);
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kAdkconRunBudget);
    r.r0 = h.peek_logical(kResults + 0);
    r.r1 = h.peek_logical(kResults + 2);
    r.r2 = h.peek_logical(kResults + 4);
    r.r3 = h.peek_logical(kResults + 6);
    r.r4 = h.peek_logical(kResults + 8);
    r.log = nmh::coalesce(h.ram_log());
    return r;
}

// ---- diskrate program run ----------------------------------------------------
struct DiskCapture {
    bool loaded = false;
    std::optional<uint64_t> done;
    std::optional<uint16_t> b1_f, b2_f, b1_s, b2_s;
    std::vector<nmh::RamAccess> log;       // coalesced: for F3 (each k is its own address)
    std::vector<nmh::RamAccess> log_raw;   // NOT coalesced: for F1 (refresh always reads word_addr
                                            // 0 with the same data, so a long idle stretch of
                                            // identical-looking refresh cycles would otherwise
                                            // collapse into one logical access and hide most lines)
};

DiskCapture run_diskrate(const std::string& dir, const std::string& bin_suffix, const nmh::Config& cfg) {
    DiskCapture r;
    std::string bin = dir + "/prog_diskrate" + bin_suffix + ".bin";
    nmh::NanomigHarness h(cfg);
    r.loaded = h.load_program(bin) > 0;
    if (!r.loaded) return r;
    poke_fill(h, DISKBUF_A, 0xAAAA, 544);
    poke_fill(h, DISKBUF_B, 0xAAAA, 544);
    h.boot(kBootBudget);
    r.done = h.run_until_write(kDoneAddr, kDoneValue, kBootBudget + kDiskRunBudget);
    r.b1_f = h.peek_logical(kResults + 0);
    r.b2_f = h.peek_logical(kResults + 2);
    r.b1_s = h.peek_logical(kResults + 6);
    r.b2_s = h.peek_logical(kResults + 8);
    r.log_raw = h.ram_log();
    r.log = nmh::coalesce(r.log_raw);
    return r;
}

std::string fmt(double v, int prec = 2) {
    char buf[64];
    std::snprintf(buf, sizeof buf, "%.*f", prec, v);
    return std::string(buf);
}

}  // namespace

int main(int argc, char** argv) {
    const std::string dir = argc > 1 ? argv[1] : ".";
    std::printf("CORE: %s\n", NMH_XSTR(NANOMIG_CORE_NAME));
    Checks checks;

    const char* mutate_env = std::getenv("NMH_MUTATE");
    const std::string mv = mutate_env ? mutate_env : "";

    const bool audio_mv   = (mv=="P1"||mv=="P2"||mv=="P3"||mv=="P4"||mv=="A1"||mv=="A2"||mv=="A4");
    const bool adkcon_mv  = (mv=="P5"||mv=="C1");
    const bool disk_mv    = (mv=="FASTF0"||mv=="FASTS1"||mv=="B2"||mv=="B3"||mv=="F1");
    const bool need_audio  = mv.empty() || audio_mv;
    const bool need_adkcon = mv.empty() || adkcon_mv;
    const bool need_disk   = mv.empty() || disk_mv;
    const std::string amv  = audio_mv ? mv : "";
    const std::string kmv  = adkcon_mv ? mv : "";

    // =====================================================================
    // P1-P4: audio byte output, order, two's complement, per-LSB step
    // =====================================================================
    if (need_audio) {
        struct CfgSpec { const char* label; nmh::Config cfg; };
        std::vector<CfgSpec> configs = { {"ECS/PAL", cfg_ecs_pal()}, {"OCS/PAL", cfg_ocs_pal()} };

        for (auto& cs : configs) {
            AudioCapture cap = run_audio(dir, amv, cs.cfg);
            bool base_ok = cap.loaded && cap.done.has_value();
            checks.expect(base_ok, std::string("FIXTURE ") + cs.label +
                ": prog_audio reached DONE [not graded -- fixture, no claim uid]");
            if (!base_ok) {
                checks.record(std::string("audio fixture did not complete in ") + cs.label +
                    "; A1-A4, P1-P4 not graded there");
                continue;
            }

            bool a2_ok = cap.adkconr.has_value() && ((*cap.adkconr) & 0x00FF) == 0;
            checks.expect(a2_ok, std::string("A2 ") + cs.label +
                ": ADKCONR bits 7-0 read 0, no attach mode [p.265]");

            struct TableSpec { const char* name; uint32_t addr; int words, N; };
            TableSpec t1{"T1", T1_ADDR, 4, kNT1}, t2{"T2", T2_ADDR, 4, kNT2}, t3{"T3", T3_ADDR, 256, kNT3};

            // A1 (F2's anchor) and A3 (median run length): shared by every
            // proposition on this table. Returns LEVELS(W_T), empty if A1,
            // A2 or A3 failed (in which case that failure has already been
            // printed, or recorded as the reason nothing further is graded).
            auto grade_precond = [&](TableSpec ts, bool a1_mut) {
                uint32_t lo = ts.addr / 2, hi = lo + ts.words - 1;
                F2Result f2 = f2_check(cap.log, lo, hi);
                bool a1 = f2.anchor_ok && (int)f2.pass_starts.size() >= ts.N + 2;
                std::string a1_label = std::string("A1 for ") + ts.name + " " + cs.label +
                    ": audio DMA ran pass after pass, F2 holds with >= " + std::to_string(ts.N + 2) +
                    " reads of " + ts.name + "'s first word";
                if (a1_mut) a1_label += " [NMH_MUTATE demonstrates this precondition red]";
                checks.expect(a1_mut ? !a1 : a1, a1_label);
                if (!f2.diag.empty()) checks.note(std::string(ts.name) + " " + cs.label + " F2 break: " + f2.diag);

                if (!a1 || !a2_ok) {
                    checks.record(std::string(ts.name) + " " + cs.label +
                        ": A3, A4 and its proposition(s) not graded, precondition A1 or A2 failed");
                    return std::vector<int32_t>{};
                }
                uint64_t wb = f2.pass_starts[1], we = f2.pass_starts[ts.N + 1];
                auto runs = raw_runs(cap.audio, wb, we);
                double median = median_run_len(runs);
                bool a3 = median >= 4.0;
                checks.expect(a3, std::string("A3 for ") + ts.name + " " + cs.label +
                    ": the median run in W_" + ts.name + " spans >= 4 output samples (" + fmt(median, 1) + ")");
                if (!a3) {
                    checks.record(std::string(ts.name) + " " + cs.label +
                        ": A4 and its proposition(s) not graded, precondition A3 failed");
                    return std::vector<int32_t>{};
                }
                return levels_from_runs(runs, median);
            };

            // ---- T1: P1, P2 ----
            {
                auto L = grade_precond(t1, mv == "A1");
                if (L.empty()) {
                    checks.record(std::string("P1/P2 ") + cs.label + ": not graded, a precondition failed");
                } else {
                    FitResult fu = fit(L, c_up()), fl = fit(L, c_low());
                    double b = fu.matches ? fu.b : fl.b;
                    bool a4 = L.size() >= (size_t)(2 * t1.words) && std::fabs(b) >= 4.0;
                    std::string a4_label = std::string("A4 for T1 ") + cs.label +
                        ": L has >= " + std::to_string(2 * t1.words) + " entries (" + std::to_string(L.size()) +
                        ") and the fitted |b| is >= 4 steps (" + fmt(std::fabs(b), 1) + ")";
                    if (mv == "A4") a4_label += " [NMH_MUTATE demonstrates this precondition red]";
                    checks.expect(mv == "A4" ? !a4 : a4, a4_label);

                    if (!a4) {
                        checks.record(std::string("P1/P2 ") + cs.label + ": not graded, precondition A4 failed");
                    } else {
                        bool p1 = fu.matches || fl.matches;
                        std::string p1_label = std::string("P1 ") + cs.label +
                            " [p.265] 5-appA-p265.c42: both bytes of every audio data word are output, "
                            "one after the other (FIT(L,C_up) or FIT(L,C_low) matches, all 4 words of "
                            "every pass in W_T1)";
                        if (mv == "P1") p1_label += " [NMH_MUTATE demonstrates this proposition red]";
                        checks.expect(mv == "P1" ? !p1 : p1, p1_label);

                        if (p1) {
                            bool p2 = fu.matches;
                            std::string p2_label = std::string("P2 ") + cs.label +
                                " [p.265,p.164,p.138]: the upper byte is output first (FIT(L,C_up) matches)";
                            if (mv == "P2") p2_label += " [NMH_MUTATE demonstrates this proposition red]";
                            checks.expect(mv == "P2" ? !p2 : p2, p2_label);
                        } else {
                            checks.record(std::string("P2 ") + cs.label + ": not graded, P1 is red");
                        }
                    }
                }
            }

            // ---- T2: P3 ----
            {
                auto L = grade_precond(t2, false);
                if (L.empty()) {
                    checks.record(std::string("P3 ") + cs.label + ": not graded, a precondition failed");
                } else {
                    FitResult f2c = fit(L, c2());
                    bool a4 = L.size() >= (size_t)(2 * t2.words) && std::fabs(f2c.b) >= 4.0;
                    checks.expect(a4, std::string("A4 for T2 ") + cs.label +
                        ": L has >= " + std::to_string(2 * t2.words) + " entries (" + std::to_string(L.size()) +
                        ") and the fitted |b| is >= 4 steps (" + fmt(std::fabs(f2c.b), 1) + ")");
                    if (!a4) {
                        checks.record(std::string("P3 ") + cs.label + ": not graded, precondition A4 failed");
                    } else {
                        bool p3 = f2c.matches;
                        std::string p3_label = std::string("P3 ") + cs.label +
                            " [p.265,p.138] 5-appA-p265.c42,A-audxdat.c5: each byte is output as a "
                            "two's-complement value (FIT(L,C2) matches, s=0,1,127,-128)";
                        if (mv == "P3") p3_label += " [NMH_MUTATE demonstrates this proposition red]";
                        checks.expect(mv == "P3" ? !p3 : p3, p3_label);
                    }
                }
            }

            // ---- T3: P4 ----
            {
                auto L = grade_precond(t3, false);
                if (L.empty()) {
                    checks.record(std::string("P4 ") + cs.label + ": not graded, a precondition failed");
                } else {
                    FitResult f3c = fit(L, c3());
                    bool a4 = L.size() >= (size_t)(t3.words + 1) && std::fabs(f3c.b) >= 4.0;
                    checks.expect(a4, std::string("A4 for T3 ") + cs.label +
                        ": L has >= " + std::to_string(t3.words + 1) + " entries (" + std::to_string(L.size()) +
                        ") and the fitted |b| is >= 4 steps (" + fmt(std::fabs(f3c.b), 1) + ")");
                    if (!a4) {
                        checks.record(std::string("P4 ") + cs.label + ": not graded, precondition A4 failed");
                    } else {
                        bool p4 = f3c.matches;
                        std::string p4_label = std::string("P4 ") + cs.label +
                            " [p.265] A-audxdat.c5: every one-LSB step of a data byte moves the audio "
                            "output by the same amount (FIT(L,C3) matches, all 255 adjacent steps, s=-128..127)";
                        if (mv == "P4") p4_label += " [NMH_MUTATE demonstrates this proposition red]";
                        checks.expect(mv == "P4" ? !p4 : p4, p4_label);
                    }
                }
            }
        }
        checks.record("A-audxdat.c5's 3 mV magnitude is not observable at these ports (samples, not "
            "volts); only the implied uniform one-LSB step is graded, in P4 above (SPECIFICATION.md 1.2)");
    }

    // =====================================================================
    // P5: ADKCON bits 8 (FAST) and 9 (MSBSYNC) read back through ADKCONR
    // =====================================================================
    if (need_adkcon) {
        std::vector<std::pair<const char*, nmh::Config>> configs = {
            {"ECS/PAL", cfg_ecs_pal()}, {"OCS/PAL", cfg_ocs_pal()} };
        for (auto& [label, cfg] : configs) {
            AdkconCapture cap = run_adkcon(dir, kmv, cfg);
            bool base_ok = cap.loaded && cap.done.has_value() &&
                cap.r0 && cap.r1 && cap.r3 && cap.r4 && (kmv == "C1" || cap.r2);
            checks.expect(base_ok, std::string("FIXTURE ") + label +
                ": prog_adkcon reached DONE and stored its result slots [not graded -- fixture, no claim uid]");
            if (!base_ok) { checks.record(std::string("P5/C1 ") + label + ": not graded, fixture failed"); continue; }

            auto w_range = [&](uint32_t off) {
                uint32_t w = (kResults + off) / 2;
                std::vector<uint64_t> v;
                for (auto& a : cap.log) if (a.is_write && a.word_addr == w) v.push_back(a.cycle);
                return v;
            };
            auto w0 = w_range(0), w1 = w_range(2), w2 = w_range(4), w3 = w_range(6), w4 = w_range(8);
            // The precondition's own, unconditional truth: exactly one write to
            // each of R0-R4, in step order. NMH_MUTATE=C1 leaves out the R2
            // store (prog_adkcon_c1mut.s), so w2 is genuinely empty there and
            // c1 is genuinely false; the print below flips the expected
            // polarity, it does not change what c1 measures.
            bool c1 = !w0.empty() && !w1.empty() && !w2.empty() && !w3.empty() && !w4.empty() &&
                      w0[0] < w1[0] && w1[0] < w2[0] && w2[0] < w3[0] && w3[0] < w4[0];
            std::string c1_label = std::string("C1 ") + label +
                ": the Chip-memory port shows one CPU write to each of R0-R4, in step order";
            if (kmv == "C1") c1_label += " [NMH_MUTATE demonstrates this precondition red]";
            checks.expect(kmv == "C1" ? !c1 : c1, c1_label);

            if (kmv == "C1") { continue; }   // P5 itself not graded under the C1 demonstration

            auto bits = [&](std::optional<uint16_t> v) {
                return std::make_pair(v ? ((*v >> 9) & 1) : -1, v ? ((*v >> 8) & 1) : -1);
            };
            auto [b9_0, b8_0] = bits(cap.r0);
            auto [b9_1, b8_1] = bits(cap.r1);
            auto [b9_2, b8_2] = bits(cap.r2);
            auto [b9_3, b8_3] = bits(cap.r3);
            auto [b9_4, b8_4] = bits(cap.r4);
            bool p5;
            if (kmv == "P5") {
                // step 2 wrote $8200 (sets bit 9, not bit 8): predicted R1=(1,0)
                p5 = (b9_0==0&&b8_0==0) && (b9_1==1&&b8_1==0) && (b9_2==1&&b8_2==1) &&
                     (b9_3==1&&b8_3==0) && (b9_4==0&&b8_4==0);
            } else {
                p5 = (b9_0==0&&b8_0==0) && (b9_1==0&&b8_1==1) && (b9_2==1&&b8_2==1) &&
                     (b9_3==1&&b8_3==0) && (b9_4==0&&b8_4==0);
            }
            std::string p5_label = std::string("P5 ") + label +
                " [p.265] A-adkcon.b08,5-appA-p265.c21,A-adkcon.b09,5-appA-p265.c20: bits 8 (FAST) and "
                "9 (MSBSYNC) of ADKCON are separate and ADKCONR reads back each state "
                "(R0=(0,0),R1=(0,1),R2=(1,1),R3=(1,0),R4=(0,0), bit9/bit8) -- register part only, "
                "what the bits do to disk data is not tested (SPECIFICATION.md 1.2)";
            if (kmv == "P5") p5_label += " [NMH_MUTATE demonstrates this proposition red]";
            checks.expect(c1 && (kmv == "P5" ? !p5 : p5), p5_label);
        }
        checks.record("A-adkcon.b09/5-appA-p265.c20's disk-read-synchronizing function is not tested: "
            "the harness serves no disk image and a test cannot drive the disk read-data input (SPECIFICATION.md 1.2)");
        checks.record("A-adkcon.b08/5-appA-p265.c21's \"(fast for MFM, slow for MFM or GCR)\" advisory "
            "is not graded (advice on use, SPECIFICATION.md 1.2); the rate itself is graded in P6");
    }

    // =====================================================================
    // P6 / F1 / F3: disk write DMA bit-cell rate, FAST = ADKCON bit 8
    //
    // G1/G2/G3 are expected to, and do, come out red: ADKCON bit 8 is
    // read back correctly (B1 passes both runs) but is not wired to
    // anything that paces disk write DMA on this RTL. Grep confirms it:
    // paula.v only routes adkcon[10] (wordsync) and adkcon[11] (uartbrk)
    // out of the register; paula_floppy.v and agnus_diskdma.v never
    // reference adkcon or a FAST/bit-cell signal at all. Measured D_F
    // equals D_S exactly regardless of which run's ADKCON gets FAST=1
    // (see the MEASURED lines below) -- the Chip-memory read rate is
    // paced by disk DMA slot availability alone, not by the bit-cell
    // clock the page describes. This is a genuine RTL/page disagreement,
    // not a test defect: a red result here is the finished outcome.
    // =====================================================================
    if (need_disk) {
        std::string suffix = "";
        if (mv == "FASTF0") suffix = "_fastf0";
        else if (mv == "FASTS1") suffix = "_fasts1";
        else if (mv == "B2") suffix = "_b2";
        else if (mv == "B3") suffix = "_b3";
        else if (mv == "F1") suffix = "_f1mut";

        struct CfgSpec { const char* label; nmh::Config cfg; bool ecs; double us_per_cck; };
        std::vector<CfgSpec> configs = {
            {"ECS/PAL", cfg_ecs_pal(), true, kUsPerCckPal},
            {"OCS/PAL", cfg_ocs_pal(), false, kUsPerCckPal},
            {"ECS/NTSC", cfg_ecs_ntsc(), true, kUsPerCckNtsc},
        };
        for (auto& cs : configs) {
            if (mv == "F1" && !cs.ecs) continue;   // F1/B4's contrasting case is ECS only (2.4)
            DiskCapture cap = run_diskrate(dir, suffix, cs.cfg);
            bool base_ok = cap.loaded && cap.done.has_value() && cap.b1_f && cap.b2_f && cap.b1_s && cap.b2_s;
            checks.expect(base_ok, std::string("FIXTURE ") + cs.label +
                ": prog_diskrate reached DONE and stored B1/B2 for both runs [not graded -- fixture, no claim uid]");
            if (!base_ok) { checks.record(std::string("P6 ") + cs.label + ": not graded, fixture failed"); continue; }

            // ---- preconditions (2.9): each is its own PASS/FAIL line, printed
            // as measured. A mutation deliberately breaks exactly one of these
            // (B1 for run F under FASTF0's [B1 framing], B2/B3/B4 under their
            // own labels); every other precondition is expected to still hold.
            bool fast_f_bit = ((*cap.b1_f >> 8) & 1) != 0;
            bool fast_s_bit = ((*cap.b1_s >> 8) & 1) != 0;
            bool b2_f = ((*cap.b2_f >> 14) & 1) == 1 && ((*cap.b2_f >> 13) & 1) == 1;
            bool b2_s = ((*cap.b2_s >> 14) & 1) == 1 && ((*cap.b2_s >> 13) & 1) == 1;

            F3Result f3f = f3_check(cap.log, DISKBUF_A / 2, 544);
            F3Result f3s = f3_check(cap.log, DISKBUF_B / 2, 544);
            bool b3_f = f3f.anchor_ok && f3f.times.size() >= 529;
            bool b3_s = f3s.anchor_ok && f3s.times.size() >= 529;

            F1Result f1r = f1_check(cap.log_raw);
            bool b4 = f1r.anchor_ok;

            checks.note(std::string("F1 anchor ") + cs.label + ": mean refresh-group spacing " +
                fmt(f1r.color_clocks, 2) + " color clocks over " + std::to_string(f1r.groups) + " groups found");
            checks.expect(b4, std::string("B4 (F1 anchor) ") + cs.label +
                ": refresh groups recur 227-228 color clocks apart");
            if (mv == "F1") {
                // F1/B4's own contrasting case (2.4) did not reproduce on this
                // RTL: agnus.v wires the same hpos to agnus_refresh and
                // agnus_diskdma, and an extreme HTOTAL (tried at HTOTAL=20,
                // not committed) demonstrably changes disk DMA's own timing
                // (D_F fell from 310520 to 28712 cycles) but the refresh
                // anchor still measures 227.00 CCK regardless. Disk DMA has
                // higher arbitration priority than refresh (agnus.v's
                // dma_dsk/dma_ref if-else chain), so once htotal shortens the
                // line, disk's far more frequent requests plausibly starve
                // refresh's own grants back down to about the original rate.
                // Reported, not worked around: the stimulus is HTOTAL=113
                // exactly as SPECIFICATION.md 2.4 states, unchanged.
                checks.record(std::string("B4 (F1 anchor) ") + cs.label +
                    ": NMH_MUTATE=F1 did not turn this red on this RTL -- refresh-group spacing "
                    "stayed at 227.00 CCK even though the same VTOTAL/HTOTAL/BEAMCON0 write "
                    "measurably changed disk DMA's own bit-cell timing (see the disk rate MEASURED "
                    "line below); see SPECIFICATION.md section 5");
            }

            // B1 for run F: FASTF0 shows the same capture graded two ways --
            // as G1's own contrasting case (run F's setting is redefined to
            // FAST=0, so B1 still holds against that redefinition) and as
            // B1's own contrasting case (run F was always meant to be
            // FAST=1, so the same reading now mismatches and B1 goes red).
            if (mv == "FASTF0") {
                checks.expect(!fast_f_bit, std::string("B1 for run F ") + cs.label +
                    " [G1 framing]: ADKCONR bit 8 equals run F's own (redefined) setting (FAST=0)");
                checks.expect(!fast_f_bit, std::string("B1 for run F ") + cs.label +
                    " [B1 framing] [NMH_MUTATE demonstrates this precondition red]: ADKCONR bit 8 "
                    "does not equal run F's intended setting (FAST=1)");
            } else {
                checks.expect(fast_f_bit, std::string("B1 for run F ") + cs.label + ": ADKCONR bit 8 == 1");
            }
            {
                std::string b2f_label = std::string("B2 for run F ") + cs.label +
                    ": DSKBYTR shows DMAON=1, DISKWRITE=1";
                if (mv == "B2") b2f_label += " [NMH_MUTATE demonstrates this precondition red]";
                checks.expect(mv == "B2" ? !b2_f : b2_f, b2f_label);
            }
            {
                std::string b3f_label = std::string("B3 for run F ") + cs.label +
                    ": disk DMA reads of buffer A, k=0..528 or more, one per k, in order (" +
                    std::to_string(f3f.times.size()) + " found)";
                if (mv == "B3") b3f_label += " [NMH_MUTATE demonstrates this precondition red]";
                checks.expect(mv == "B3" ? !b3_f : b3_f, b3f_label);
            }
            checks.expect(!fast_s_bit == (mv != "FASTS1"),
                std::string("B1 for run S ") + cs.label + ": ADKCONR bit 8 == " + (mv=="FASTS1"?"1":"0"));
            checks.expect(b2_s, std::string("B2 for run S ") + cs.label + ": DSKBYTR shows DMAON=1, DISKWRITE=1");
            checks.expect(b3_s, std::string("B3 for run S ") + cs.label +
                ": disk DMA reads of buffer B, k=0..528 or more, one per k, in order (" +
                std::to_string(f3s.times.size()) + " found)");

            if (mv == "B2" || mv == "B3") {
                checks.record(std::string("G1/G2/G3 ") + cs.label +
                    ": not graded under this precondition's own demonstration (B2/B3)");
                continue;
            }

            // ---- G1, G2, G3: same comparators regardless of mutation; only
            // the input capture (selected above by `suffix`) differs. ----
            bool have_df = f3f.times.size() > 528;
            bool have_ds = f3s.times.size() > 528;
            if (!have_df && !have_ds) {
                checks.record(std::string("G1/G2/G3 ") + cs.label + ": not graded, B3 did not hold for either run");
                continue;
            }

            double d_f = have_df ? (double)(f3f.times[528] - f3f.times[16]) : 0.0;
            double d_s = have_ds ? (double)(f3s.times[528] - f3s.times[16]) : 0.0;
            double t_f_us = (d_f / (double)kCck) * cs.us_per_cck / (16.0 * 512.0);
            double t_s_us = (d_s / (double)kCck) * cs.us_per_cck / (16.0 * 512.0);
            checks.note(std::string("disk rate ") + cs.label + ": D_F=" + fmt(d_f, 0) + " cycles, D_S=" +
                fmt(d_s, 0) + " cycles, T_F=" + fmt(t_f_us, 3) + " us, T_S=" + fmt(t_s_us, 3) + " us");

            if (have_df) {
                bool g1 = t_f_us >= 1.5 && t_f_us < 2.5;
                std::string g1_label = std::string("G1 ") + cs.label +
                    " [p.265,p.249] A-adkcon.b08,5-appA-p265.c21: FAST=1 gives a bit-cell time of about "
                    "2 us (1.5 <= T_F < 2.5)";
                if (mv == "FASTF0") g1_label += " [NMH_MUTATE demonstrates this proposition red]";
                checks.expect(mv == "FASTF0" ? !g1 : g1, g1_label);
            } else {
                checks.record(std::string("G1 ") + cs.label + ": not graded, B3 for run F failed");
            }

            if (mv == "FASTF0") continue;   // G2/G3 use run S, unaffected by this mutation

            if (have_ds) {
                bool g2 = t_s_us >= 3.5 && t_s_us < 4.5;
                std::string g2_label = std::string("G2 ") + cs.label +
                    " [p.265,p.249] A-adkcon.b08,5-appA-p265.c21: FAST=0 gives a bit-cell time of about "
                    "4 us (3.5 <= T_S < 4.5)";
                if (mv == "FASTS1") g2_label += " [NMH_MUTATE demonstrates this proposition red]";
                checks.expect(mv == "FASTS1" ? !g2 : g2, g2_label);
            } else {
                checks.record(std::string("G2 ") + cs.label + ": not graded, B3 for run S failed");
            }

            if (have_df && have_ds) {
                bool g3 = std::fabs(d_s - 2.0 * d_f) < 3.0 * 228.0 * (double)kCck;
                std::string g3_label = std::string("G3 ") + cs.label +
                    " [p.265,p.249]: the two settings differ by a factor of two (|D_S - 2*D_F| < 3 lines)";
                if (mv == "FASTS1") g3_label += " [NMH_MUTATE demonstrates this proposition red, framed as G3]";
                checks.expect(mv == "FASTS1" ? !g3 : g3, g3_label);
            } else {
                checks.record(std::string("G3 ") + cs.label + ": not graded, B3 failed for a run");
            }
        }
    }

    // ---- claims not turned into a proposition (SPECIFICATION.md 1.1) -----
    if (mv.empty()) {
        checks.record("5-appC-p310.c5,c6,c7,c8,C-interp.per.c1,c2,c3,c4 (p.310, AUDxPER and the "
            "programmable scan rate): no value or rule the page gives for any scan rate's maximum "
            "period, the one directional statement is hedged (\"Generally\"), AUDxPER is write-only "
            "on every page that lists it so \"the maximum value read from this register\" cannot be "
            "read, and c3 adds a sentence (\"holds for the standard chip set only\") that appears on "
            "neither p.310 nor p.140 -- not testable at these ports (SPECIFICATION.md 1.1)");
        checks.record("5-p158.c37,5-scale.c13 (p.158, the 7 KHz cut-off filter): pp.155/156/158 give "
            "three different frequencies for the same low-pass filter and no attenuation at any stated "
            "frequency, and the filter sits outside the chips whose output this test samples "
            "(SPECIFICATION.md 1.1) -- not testable at these ports");
    }

    std::printf("%d passed, %d failed\n", checks.passed, checks.failed);
    return checks.failed == 0 ? 0 : 1;
}
