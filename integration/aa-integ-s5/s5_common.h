// s5_common.h -- the baseline S0, the anchors G-C, G-D, G-F and the fixture
// shared by every proposition (SPECIFICATION.md sections 2.1 to 2.9).
#pragma once

#include "s5_core.h"

namespace s5 {

inline void not_graded(Checks& c, const std::string& id, const std::string& unmet) {
    c.record("NOT GRADED " + id + ": precondition(s) not met: " + unmet);
}

// A graded line: NOT GRADED when its preconditions do not hold, PASS or FAIL otherwise.
inline void graded(Checks& c, bool pre, bool cond, const std::string& label, const std::string& unmet) {
    if (!pre) { not_graded(c, label.substr(0, label.find_first_of(" :")), unmet); return; }
    c.expect(cond, label);
}

inline const char* run_bin() { return mut("FIXTURE") ? "prog_run_nofix.bin" : "prog_run.bin"; }

// The shared contrast of G-BRK: UARTBRK is set after the baseline.
inline std::function<void(Script&)> brk_hook() {
    return mut("GBRK") ? std::function<void(Script&)>([](Script& sc) { sc.w16(ADKCON, 0x8800); }) : std::function<void(Script&)>();
}

// ---- the baseline S0 (2.1), its read-back (2.9) and the G-C poll (2.2) ------
struct Prefix {
    int i_intenar = -1, i_dmaconr = -1, i_adkconr = -1, i_intreqr = -1;   // log indexes of the read-backs after the baseline
    int i_gc = -1, n_gc = 0;                              // log index and length of the VHPOSR poll
    int logn = 0;                                         // log words stored so far
};

inline Prefix prefix(Script& s, bool gc = true, unsigned gc_n = 119, const std::function<void(Script&)>& after_s0 = nullptr) {
    Prefix p;
    const bool g0 = mut("G0");
    if (g0) { s.w16(INTENA, 0x8080); s.w16(ADKCON, 0x8001); }
    if (!g0) s.w16(INTENA, 0x7FFF);
    s.w16(INTREQ, 0x7FFF);
    s.w16(DMACON, 0x7FFF);
    if (!g0) s.w16(ADKCON, 0x08FF);
    for (int x = 0; x < 4; ++x) s.w16(audvol(x), 0);
    if (after_s0) after_s0(s);
    p.i_intreqr = p.logn++; s.rd(INTREQR);
    p.i_intenar = p.logn++; s.rd(INTENAR);
    p.i_dmaconr = p.logn++; s.rd(DMACONR);
    p.i_adkconr = p.logn++; s.rd(ADKCONR);
    if (gc) {
        p.i_gc = p.logn; p.n_gc = int(gc_n) + 1;
        s.poll(mut("GC") ? uint16_t(0x004) : uint16_t(VHPOSR), gc_n);   // contrast: VPOSR, whose low byte is not the colour-clock count
        p.logn += p.n_gc;
    }
    return p;
}

inline uint16_t logv(const Cap& c, int idx) {
    return (idx >= 0 && size_t(idx) < c.log.size()) ? c.log[size_t(idx)].second : 0xDEAD;
}
inline bool log_has(const Cap& c, int idx) { return idx >= 0 && size_t(idx) < c.log.size(); }

// G-S0: INTENAR bits 14..0, DMACONR bits 9..0, ADKCONR bits 11 and 7..0 read 0.
inline bool g_s0(const Cap& c, const Prefix& p) {
    if (!log_has(c, p.i_adkconr)) return false;
    return (logv(c, p.i_intenar) & 0x7FFF) == 0 && (logv(c, p.i_dmaconr) & 0x03FF) == 0 &&
           (logv(c, p.i_adkconr) & 0x08FF) == 0;
}

// G-C: the low byte of VHPOSR counts one per colour clock (K system clocks), the high
// byte does not; DMA reads (refresh and the fetches given) start congruent modulo K.
struct GC { bool ok = false; double slope_k = 0; double hi_slope_k = 0; size_t run = 0; bool phase_ok = false; bool count_ok = false; bool hi_count_ok = false; };

// One count per K system clocks, exactly: the run of reads v_i (i = 0..n-1) stored at t_i was read at some r_i in (t_(i-1), t_i]
// (the read precedes its own store and follows the store before it). A counter that steps once per K system clocks, from a start
// X that is not known, puts read i in the count v_0 + d_i (d_i = v_i - v_0) iff X + K d_i <= r_i < X + K (d_i + 1). Such an X exists
// iff max_i (a_i - K (d_i + 1)) < min_i (b_i - K d_i), a_i = t_(i-1), b_i = t_i. No tolerance enters; the bracket is the store spacing.
inline bool exact_counter(const std::vector<std::pair<uint64_t, uint16_t>>& v, size_t i0, size_t n, bool high, uint64_t K, const std::vector<std::pair<uint64_t, uint16_t>>& all, size_t base_idx) {
    if (n < 2 || i0 + n > v.size() || K == 0) return false;
    auto val = [&](size_t i) -> int64_t { return high ? int64_t(v[i].second >> 8) : int64_t(v[i].second & 0xFF); };
    int64_t lo = INT64_MIN, hi = INT64_MAX;
    for (size_t i = i0; i < i0 + n; ++i) {
        size_t gi = base_idx + i;
        int64_t a = gi > 0 ? int64_t(all[gi - 1].first) : int64_t(v[i].first) - 1, b = int64_t(v[i].first);
        int64_t d = val(i) - val(i0);
        lo = std::max(lo, a - int64_t(K) * (d + 1));
        hi = std::min(hi, b - int64_t(K) * d);
    }
    return lo < hi;
}

inline GC g_c(const Cap& c, const Prefix& p, uint64_t K, const std::vector<Fetch>& dma = {}) {
    GC r;
    if (K == 0 || !log_has(c, p.i_gc + p.n_gc - 1)) return r;
    std::vector<std::pair<uint64_t, uint16_t>> v(c.log.begin() + p.i_gc, c.log.begin() + p.i_gc + p.n_gc);
    // longest run of strictly increasing low bytes: one line, between two line starts
    size_t best0 = 0, best_len = 0, cur0 = 0;
    for (size_t i = 1; i <= v.size(); ++i) {
        if (i == v.size() || (v[i].second & 0xFF) <= (v[i - 1].second & 0xFF)) {
            if (i - cur0 > best_len) { best_len = i - cur0; best0 = cur0; }
            cur0 = i;
        }
    }
    r.run = best_len;
    if (best_len < 8) return r;
    r.count_ok = exact_counter(v, best0, best_len, false, K, c.log, size_t(p.i_gc));
    // the contrast reads the high byte (V7..V0), which counts lines: the same bracket over the same reads
    r.hi_count_ok = exact_counter(v, best0, best_len, true, K, c.log, size_t(p.i_gc));
    auto fit = [&](bool high) {
        double sx = 0, sy = 0, sxx = 0, sxy = 0; double n = double(best_len);
        for (size_t i = best0; i < best0 + best_len; ++i) {
            double x = double(v[i].first - v[best0].first);
            double y = high ? double(v[i].second >> 8) : double(v[i].second & 0xFF);
            sx += x; sy += y; sxx += x * x; sxy += x * y;
        }
        double den = n * sxx - sx * sx;
        return den == 0 ? 0.0 : (n * sxy - sx * sy) / den;
    };
    r.slope_k = fit(false) * double(K);
    r.hi_slope_k = fit(true) * double(K);
    bool slope_ok = r.count_ok;
    // phases
    std::set<uint64_t> ph;
    int nref = 0;
    for (auto& a : c.ram) if (!a.is_write && refresh_at(c, a.cycle)) { ph.insert(a.cycle % K); if (++nref >= 200) break; }
    for (auto& f : dma) ph.insert(f.t % K);
    r.phase_ok = ph.size() == 1;
    r.ok = slope_ok && r.phase_ok;
    return r;
}

// Per-run result of the shared preconditions.
struct Std {
    bool fixture = false, s0 = false, gc = false;
    Frame frame;
    GC gcr;
};

inline Std std_pre(const Cap& c, const Prefix& p, const std::vector<Fetch>& dma = {}) {
    Std s;
    s.fixture = c.loaded && c.released && c.fixture_ok && c.done && c.timeouts == 0;
    s.frame = analyse_refresh(c);
    s.s0 = g_s0(c, p);
    s.gcr = g_c(c, p, s.frame.K, dma);
    s.gc = s.frame.ok && s.gcr.ok;
    return s;
}

// Aggregates the shared preconditions over the runs of one proposition and prints them.
struct PreAgg {
    bool fixture = true, s0 = true, gc = true;
    void add(const Std& s) { fixture &= s.fixture; s0 &= s.s0; gc &= s.gc; }
};

inline void print_std(Checks& ck, const std::string& pr, const PreAgg& a, size_t runs) {
    ck.expect(a.fixture, pr + "-FIXTURE: " + dec((long long)runs) + " run(s) loaded, booted, stored the $BEEF constant at RESULTS, reached DONE and ended no wait by its timeout, before any value is read");
    ck.expect(a.s0, pr + "-G-S0: after the baseline S0 INTENAR bits 14..0, DMACONR bits 9..0 and ADKCONR bits 11 and 7..0 read 0 in every run (AA p.10 set/clear registers; AA p.22 DMACON; AA p.25 INTENA)");
    ck.expect(a.gc, pr + "-G-C: in every run the VHPOSR bits 7..0 (H8..H1) count one per colour clock (one count per K system clocks, exactly, bracketed by the read and store times of the poll) and the DMA reads start congruent modulo K (AA p.29 'RESOLUTION=1/160 OF SCREEN WIDTH (280 NS)')");
}

inline void note_gc(Checks& ck, const std::string& tag, const Std& s) {
    ck.note(tag + ": K=" + dec((long long)s.frame.K) + " slope(H8..H1)=" + num2(s.gcr.slope_k) + "/K slope(V7..V0)=" + num2(s.gcr.hi_slope_k) +
            "/K run=" + dec((long long)s.gcr.run) + " count_ok=" + dec(s.gcr.count_ok) + " phase_ok=" + dec(s.gcr.phase_ok) + " refresh_ok=" + dec(s.frame.ok));
}

} // namespace s5
