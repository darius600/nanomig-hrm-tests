// s3_pre.h -- the common preconditions of SPECIFICATION.md 2.12 and the
// common stimulus changes that make them fail.

#pragma once

#include "s3_check.h"

namespace s3 {

// chip line period measured from the output: twice the median spacing of the
// output lines (the line doubler shows each chip line twice, 2.11)
inline uint64_t chip_line_period(const RunData& r) {
    std::vector<uint64_t> d;
    for (size_t i = 1; i + 1 < r.lines.size() && d.size() < 4000; ++i)
        d.push_back(r.lines[i + 1].begin - r.lines[i].begin);
    if (d.empty()) return 0;
    std::sort(d.begin(), d.end());
    return 2 * d[d.size() / 2];
}

inline uint64_t iabs(int64_t v) { return uint64_t(v < 0 ? -v : v); }

// builds the spec of a run: applies the stimulus changes of the common
// preconditions that the mutation named, then the standard setup
inline RunSpec base_spec(Std o, const std::string& pfx) {
    if (mutating((pfx + "-PC-FRAME").c_str())) {
        if (o.lf) o.beamcon0 = 0x0820;          // VARBEAMEN omitted
        else o.dmacon &= ~0x0080;               // COPEN not set
    }
    if (mutating((pfx + "-PC-PIXEL").c_str())) { o.lf = true; o.htotal = 0x00E3; o.vtotal = 0x0138; o.beamcon0 = 0x08A0; }
    RunSpec s;
    s.setup = std_setup(o);
    if (mutating((pfx + "-PC-SETUP").c_str())) s.no_marker = true;
    if (mutating((pfx + "-PC-TAG").c_str())) s.no_tag_moves = true;
    return s;
}

// The common preconditions accumulated over every run of one proposition
struct Pre {
    std::string pfx;
    bool setup = true, frame = true, tag = true, pixel = true;
    bool used_tag = false, used_pixel = false;
    uint64_t u = 0;
    long lf_need = 0;                  // the least field length the long-frame runs of the proposition ask for, in lines
    std::vector<long> lf_field;        // the measured field length of each long-frame run, in lines

    explicit Pre(const std::string& p) : pfx(p) {}

    // frames first..last (1-based) are used; lf_lines > 0: a long-frame run whose field lasts at least that many lines
    void check(const RunData& r, int first, int last, int lf_lines, bool need_tag, bool need_pixel) {
        used_tag = used_tag || need_tag;
        used_pixel = used_pixel || need_pixel;
        if (!r.loaded || !r.released || !r.done) { setup = frame = tag = pixel = false; return; }
        if (!r.marker_seen) { setup = false; frame = false; return; }
        // PC-FRAME
        uint64_t Pc = chip_line_period(r);
        if (r.tF.size() < size_t(last) + 1 || Pc == 0) frame = false;
        else {
            std::vector<uint64_t> sp;
            for (int i = first; i <= last; ++i) sp.push_back(r.tF[i] - r.tF[i - 1]);
            for (auto d : sp) if (iabs(int64_t(d) - int64_t(sp[0])) > Pc / 8) frame = false;
            if (lf_lines > 0) {
                lf_need = std::max<long>(lf_need, lf_lines);
                lf_field.push_back(std::lround(double(sp[0]) / double(Pc)));
                if (sp[0] + Pc / 8 < uint64_t(lf_lines) * Pc) frame = false;
            }
        }
        if (!need_tag && !need_pixel) return;
        if (r.tF.size() < size_t(last) + 1) { tag = false; pixel = false; return; }
        for (int i = first; i <= last; ++i) {
            Frame f = get_frame(r, i);
            bool ok = f.ok && f.tag.size() == size_t(r.spec.tag_hi - r.spec.tag_lo + 1);
            if (ok) {
                uint64_t P = line_period(f);
                int prev = -1;
                for (const auto& kv : f.tag) {
                    if (prev >= 0 && iabs(int64_t(kv.second) - int64_t(f.tag.at(prev)) - int64_t(P)) > P / 32) ok = false;
                    prev = kv.first;
                }
                if (need_tag) {
                    int prevL = -1;
                    for (int n = r.spec.tag_lo; n <= r.spec.tag_hi && ok; ++n) {
                        int L = out_line(r, f, n);
                        if (L < 0 || L <= prevL) ok = false;
                        prevL = L;
                    }
                }
                if (need_pixel && ok) {
                    uint64_t d = f.tag.at(r.spec.tag_hi) - f.tag.at(r.spec.tag_lo);
                    uint64_t unit = uint64_t(r.spec.tag_hi - r.spec.tag_lo) * 1816;
                    if (d % unit != 0 || d / unit < 1) pixel = false; else u = d / unit;
                }
            }
            if (!ok) { if (need_tag) tag = false; if (need_pixel) pixel = false; }
        }
    }

    bool ok_setup() const { return setup; }

    void print(const std::string& where) {
        g.expect(setup, pfx + "-PC-SETUP: the setup marker 0x5E7F is seen as a write to 0x0C0000 at the port in " + where + "; the first frame fetch at COP1LC follows it");
        g.expect(frame, pfx + "-PC-FRAME: the Copper fetches at the COP1LC address exist for every frame used in " + where + " and are equally spaced" +
                 (lf_need > 0 ? ", and the field of a long-frame run lasts at least " + dec(lf_need) + " lines" : std::string()));
        if (!lf_field.empty()) {
            long lo = *std::min_element(lf_field.begin(), lf_field.end()), hi = *std::max_element(lf_field.begin(), lf_field.end());
            g.note(pfx + ": the field of the long-frame runs lasts " + dec(lo) + (hi != lo ? ".." + dec(hi) : std::string()) + " lines (p.29, VTOTAL = 0x05FF, LACE clear: 'so there's this many + 1 in a field' = 1536 lines)");
        }
        if (used_tag)
            g.expect(tag, pfx + "-PC-TAG: in every graded frame of " + where + " the tag MOVEs 0x2C-0xFF are fetched with equal spacing and the output shows tag(n) for each n in increasing order");
        if (used_pixel)
            g.expect(pixel, pfx + "-PC-PIXEL: in every graded frame of " + where + " t_tag(0xFF) - t_tag(0x2C) = 211 * 1816 * u clk_sys cycles for a whole number u >= 1 (u = " + dec(long(u)) + ")");
    }
};

inline bool static_no_write(const RunSpec& s, uint16_t lo, uint16_t hi) {
    for (const auto& w : s.setup) if (w.reg >= lo && w.reg <= hi) return false;
    for (const auto& e : s.ev) for (const auto& w : e.wr) if (w.reg >= lo && w.reg <= hi) return false;
    return true;
}

// the data words at addr[i] are fetched in frame f with the intended values
// (the stimulus's own list: vb_intended, or vb when it holds the same)
inline bool moves_fetched(const RunData& r, const Frame& f, const std::vector<Wr>& intended,
                          const std::vector<uint32_t>& addr, size_t from = 0, size_t to = size_t(-1)) {
    if (!f.ok) return false;
    for (size_t i = from; i < addr.size() && i < intended.size() && i < to; ++i) {
        auto it = r.clread.find(addr[i]);
        if (it == r.clread.end()) return false;
        bool seen = false;
        for (const auto& p : it->second)
            if (p.first >= f.t0 && p.first < f.t1) { seen = (p.second == intended[i].val); break; }
        if (!seen) return false;
    }
    return true;
}
inline const std::vector<Wr>& intended_vb(const RunData& r) {
    return r.spec.vb_intended.empty() ? r.spec.vb : r.spec.vb_intended;
}

// first access in [t0,t1) at a byte address in [lo,hi)
inline size_t acc_from(const RunData& r, uint64_t t) {   // index of the first access at or after t
    size_t lo = 0, hi = r.acc.size();
    while (lo < hi) {
        size_t m = (lo + hi) / 2;
        if (r.acc[m].cyc < t) lo = m + 1; else hi = m;
    }
    return lo;
}

inline const Acc* first_in(const RunData& r, uint64_t t0, uint64_t t1, uint32_t lo, uint32_t hi) {
    for (size_t i = acc_from(r, t0); i < r.acc.size(); ++i) {
        const Acc& a = r.acc[i];
        if (a.cyc >= t1) break;
        uint32_t b = a.waddr * 2;
        if (b >= lo && b < hi) return &a;
    }
    return nullptr;
}

// time of the first read of byte address `a` in [t0,t1)
inline std::optional<uint64_t> first_read_at(const RunData& r, uint32_t a, uint64_t t0, uint64_t t1) {
    for (size_t i = acc_from(r, t0); i < r.acc.size(); ++i) {
        const Acc& x = r.acc[i];
        if (x.cyc >= t1) break;
        if (!x.wr && x.waddr * 2 == a) return x.cyc;
    }
    return std::nullopt;
}

// number of accesses in [t0,t1) at byte addresses in [lo,hi), reads only when rd
inline int count_in(const RunData& r, uint64_t t0, uint64_t t1, uint32_t lo, uint32_t hi, int rw = 0 /*0 any,1 read,2 write*/) {
    int n = 0;
    for (size_t i = acc_from(r, t0); i < r.acc.size(); ++i) {
        const Acc& a = r.acc[i];
        if (a.cyc >= t1) break;
        uint32_t b = a.waddr * 2;
        if (b >= lo && b < hi && (rw == 0 || (rw == 1 && !a.wr) || (rw == 2 && a.wr))) ++n;
    }
    return n;
}

}  // namespace s3
