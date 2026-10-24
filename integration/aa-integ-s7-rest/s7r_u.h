// s7r_u.h -- the UHRES runs (SPECIFICATION.md section 4), the access classification R2 with
// its cross-check against run UR, and the shared preconditions PR-U1 to PR-U5.
#pragma once

#include "s7r_p8.h"

namespace s7r {

// The marker loop of a run: the byte address of its first instruction (`move.w (a2),d0`, `move.b d0,d1`,
// `sub.b d5,d1`, `cmp.b d6,d1`: the words $3012 $1200 $9205 $B206 of s7r.i, MARKLOOP) in the program binary,
// which the harness loads at PROGRAM_ORG ($080400); 0 when none.
inline uint32_t loop_head(const std::string& bin) {
    std::ifstream f(g_dir + "/" + bin, std::ios::binary);
    std::vector<uint8_t> b((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());
    auto w = [&](size_t i) { return uint16_t((b[i] << 8) | b[i + 1]); };
    for (size_t i = 0; i + 7 < b.size(); i += 2)
        if (w(i) == 0x3012 && w(i + 2) == 0x1200 && w(i + 4) == 0x9205 && w(i + 6) == 0xB206) return 0x080400 + uint32_t(i);
    return 0;
}

struct UDef {
    std::string nm;
    int bph = 0, bpl = 0, bmod = 0, bstrt = 0, bstop = 0;
    int sph = 0, spl = 0, sstrt = 0, sstop = 0;
    int vlo = 0, vhi = 0;
    int glo = 0x60, gw = 0x30;      // the marker gate of the program
};

inline UDef udef(const std::string& nm) {
    UDef d; d.nm = nm;
    d.bph = 0x0015; d.bpl = 0xAAAB; d.bmod = 0x0010; d.bstrt = 0x50; d.bstop = 0x70;
    d.sph = 0x0013; d.spl = 0x2469; d.sstrt = 0x58; d.sstop = 0x68; d.vlo = 0x40; d.vhi = 0x80;
    if (nm == "U2") { d.bph = 0x000A; d.bpl = 0x5555; d.bmod = 0xFFF0; d.sph = 0x000C; d.spl = 0xDB97; }
    if (nm == "U3") { d.bph = 0xFFF5; d.sph = 0xFFF3; }
    if (nm == "U4") { d.bstrt = 0xF8; d.bstop = 0x108; d.sstrt = 0x100; d.sstop = 0x110; d.vlo = 0xE8; d.vhi = 0x120; }
    if (nm == "UR") { d.vhi = 0x121; }
    if (nm == "D1" || nm == "D0" || nm == "DH") { d.bmod = 0; d.sstrt = 0xF0; d.sstop = 0xF4; }
    if (nm == "UW") { d.bmod = 0; d.bstop = 0x7A; d.sstrt = 0x50; d.sstop = 0x7A; }
    return d;
}

struct URun {
    UDef d;
    std::string bin;
    Cap cap;
    Markers m;
    MarkerAnchor an;
    uint64_t ts = 0, tsd = 0, tend = 0;    // end of prologue step 1, setup-done record, end marker
    int setup_line = -1;
    uint32_t P = 0, S = 0;
    int bstep = 0;
    uint32_t blo = 0, bhi = 0;
    std::vector<uint64_t> heads;           // port times of the fetches of the marker loop's first instruction
    std::vector<Acc> uh;                   // accesses from the record to the end marker that are not refresh, CPU or zero-block accesses
    std::vector<Acc> bplh, sprh, stray;    // the same up to t(Vhi), sorted by range
    std::map<int, std::vector<Acc>> bl, sl;   // per line V
};

// The accesses of one run, sorted by the classification of R2 and the run's expected ranges.
inline void measure_u(URun& r) {
    const Cap& c = r.cap;
    r.m = markers(c, r.d.vlo, r.d.vhi);
    r.an = anchor_markers(r.m, r.d.glo, r.d.gw);
    r.heads.clear();
    if (const uint32_t ha = loop_head(r.bin))
        for (auto& x : c.acc) if (!x.w && x.a == ha) r.heads.push_back(x.t);
    r.tend = end_time(c);
    r.ts = write_time(c, kFixture + 2, 1);
    r.tsd = write_time(c, kSdRec + 2);
    uint16_t v = word_at(c, kSdRec), h = word_at(c, kSdRec + 2);
    r.setup_line = r.tsd ? int(((v & 7) << 8) | (h >> 8)) : -1;
    int sx = int(int16_t(r.d.bmod));
    r.bstep = 2 + sx;
    r.P = (uint32_t(r.d.bph & 0x1F) << 16) | uint32_t(r.d.bpl & 0xFFFE);
    r.S = (uint32_t(r.d.sph & 0x1F) << 16) | uint32_t(r.d.spl & 0xFFFE);
    int nl = r.d.bstop - r.d.bstrt;
    int64_t lo = r.P, hi = r.P;
    for (int k = 0; k < nl; ++k) { int64_t a = int64_t(r.P) + int64_t(k) * r.bstep; lo = std::min(lo, a); hi = std::max(hi, a); }
    r.blo = uint32_t(std::max<int64_t>(0, lo - 0x100)); r.bhi = uint32_t(hi + 0x100);
    if (!r.tsd || !r.tend) return;
    for (auto& x : c.acc) {
        const Cls k = classify(x);
        if (x.t < r.tsd || x.t > r.tend || (k != C_UH && k != C_COP)) continue;   // not a refresh, CPU or zero-block access
        r.uh.push_back(x);
    }
    const uint64_t tlast = (r.m.have.size() && r.m.have.back()) ? r.m.t.back() : r.tend;
    for (auto& x : r.uh) {
        if (x.t > tlast) continue;
        int line = line_of(r.m, x.t);
        if (x.a >= r.blo && x.a <= r.bhi) { r.bplh.push_back(x); if (line >= 0) r.bl[line].push_back(x); }
        else if (x.a >= r.S && x.a < r.S + 0x1000) { r.sprh.push_back(x); if (line >= 0) r.sl[line].push_back(x); }
        else r.stray.push_back(x);
    }
}

inline URun urun(const std::string& nm, const std::string& bin, uint64_t cycles = 7'000'000) {
    URun r;
    r.d = udef(nm);
    r.bin = bin;
    r.cap = runbin(bin, cycles);
    return r;
}

// ---- R2: the cross-check of the refresh output against run UR -------------------------
// From the end of prologue step 1 to the run's end marker the accesses the refresh output
// marks are exactly the accesses at the port times of UR's non-CPU accesses.
inline bool xcheck_cap(const Cap& c, const URun& ur) {
    const uint64_t ts = write_time(c, kFixture + 2, 1), tend = end_time(c);
    if (!ts || !tend || !ur.ts || !ur.tend || tend > ur.tend) return false;
    std::vector<uint64_t> a, b;
    for (auto& y : c.acc) if (y.t >= ts && y.t <= tend && classify(y) == C_REF) a.push_back(y.t);
    for (auto& y : ur.cap.acc) if (y.t >= ts && y.t <= tend && classify(y) != C_CPU) b.push_back(y.t);
    return !a.empty() && a == b;
}
inline bool xcheck(const URun& x, const URun& ur) { return xcheck_cap(x.cap, ur); }

// The anchor of R2: between every two consecutive markers of UR at least one refresh access,
// and no read inside Z.
inline bool ur_anchor(const URun& ur) {
    if (!ur.tend) return false;
    for (size_t i = 1; i < ur.m.t.size(); ++i) {
        if (!ur.m.have[i] || !ur.m.have[i - 1]) return false;
        bool any = false;
        for (auto& y : ur.cap.acc) if (y.ref && y.t > ur.m.t[i - 1] && y.t < ur.m.t[i]) { any = true; break; }
        if (!any) return false;
    }
    for (auto& y : ur.cap.acc) if (!y.w && y.a >= kZ && y.a < kZEnd) return false;
    return true;
}

// ---- helpers on lines ------------------------------------------------------------
inline uint64_t tmark(const URun& r, int v) {
    int i = v - r.d.vlo;
    return (i >= 0 && size_t(i) < r.m.t.size() && r.m.have[size_t(i)]) ? r.m.t[size_t(i)] : 0;
}
inline const std::vector<Acc>& on_line(const std::map<int, std::vector<Acc>>& m, int v) {
    static const std::vector<Acc> none;
    auto it = m.find(v);
    return it == m.end() ? none : it->second;
}
inline size_t count_before(const std::vector<Acc>& v, uint64_t t) { size_t n = 0; for (auto& x : v) if (x.t < t) ++n; return n; }
inline size_t count_after(const std::vector<Acc>& v, uint64_t t) { size_t n = 0; for (auto& x : v) if (x.t > t) ++n; return n; }

// "Exactly one BPLH access on every line of lo..hi" (E3.1).
inline bool one_per_line(const URun& r, int lo, int hi) {
    for (int v = lo; v <= hi; ++v) if (on_line(r.bl, v).size() != 1) return false;
    return true;
}
inline bool two_per_line(const URun& r, int lo, int hi) {
    for (int v = lo; v <= hi; ++v) if (on_line(r.sl, v).size() != 2) return false;
    return true;
}

inline std::vector<std::pair<uint64_t, uint32_t>> tl(const std::vector<Acc>& v) {
    std::vector<std::pair<uint64_t, uint32_t>> o;
    for (auto& x : v) o.push_back({x.t, x.a});
    return o;
}

// ---- the H counter as the line-start reference (SPECIFICATION.md section 3, R1) -----------------------
// t_s(V): the port time of the last fetch of the marker loop's first instruction before the store of marker V; 0 when none.
inline uint64_t sample_time(const URun& r, int v) {
    const int i = v - r.d.vlo;
    if (i < 0 || size_t(i) >= r.m.t.size() || !r.m.have[size_t(i)]) return 0;
    auto it = std::lower_bound(r.heads.begin(), r.heads.end(), r.m.t[size_t(i)]);
    return it == r.heads.begin() ? 0 : *(it - 1);
}
// h(V): the H field (bits 7-0) the marker loop read for marker V.
inline int marker_h(const URun& r, int v) { return int(r.m.h[size_t(v - r.d.vlo)] & 0xFF); }
// The beam position, in memory cycles from the start of line V, at the port time t (before the sample of marker V).
inline double beam_pos(const URun& r, int v, uint64_t t, double K) {
    return double(marker_h(r, v)) - (double(sample_time(r, v)) - double(t)) / K;
}
// The start of line V in clk_sys cycles, T0(V) = t_s(V) - h(V) K; false when marker V or its sample time is missing.
inline bool line_start(const URun& r, int v, double K, double& t0) {
    const uint64_t ts = sample_time(r, v);
    if (!ts) return false;
    t0 = double(ts) - double(marker_h(r, v)) * K;
    return true;
}
// The horizontal line of an access by the H counter: the marker interval's line V, or V - 1 when the position is negative; -1 when none.
inline int beam_line(const URun& r, const Acc& x, double K) {
    const int v = line_of(r.m, x.t);
    if (v < 0 || !sample_time(r, v)) return -1;
    return beam_pos(r, v, x.t, K) < 0.0 ? v - 1 : v;
}

// The 40 walking cases of run UW (SPECIFICATION.md section 4): case n = 0..19 sets pointer bit n alone, n = 20..39 clears
// pointer bit n - 20 alone; bit j < 15 is DB(j+1) of the low word, bit j >= 15 is DB(j-15) of the high word. Unused bits
// are 0 in the walking-one cases and 1 in the walking-zero cases.
inline void walk_words(int n, uint16_t& pth, uint16_t& ptl) {
    const bool one = n < 20;
    const int j = one ? n : n - 20;
    if (one) {
        ptl = j < 15 ? uint16_t(1u << (j + 1)) : uint16_t(0);
        pth = j >= 15 ? uint16_t(1u << (j - 15)) : uint16_t(0);
    } else {
        ptl = j < 15 ? uint16_t(0xFFFFu & ~(1u << (j + 1))) : uint16_t(0xFFFF);
        pth = j >= 15 ? uint16_t(0xFFFFu & ~(1u << (j - 15))) : uint16_t(0xFFFF);
    }
}
// The address a pointer pair holds: (high word bits 4-0) << 16 | (low word & 0xFFFE) (AA p.6, legend '+').
inline uint32_t walk_addr(int n) {
    uint16_t h, l;
    walk_words(n, h, l);
    return (uint32_t(h & 0x1F) << 16) | uint32_t(l & 0xFFFE);
}
// Every case of one pointer has a UHRES access at its address on its line 0x51 + n (the sprite pointer carries case (n + 20) mod 40).
inline bool walk_ok(const URun& uw, bool sprite, std::string& detail) {
    int bad = 0;
    for (int n = 0; n < 40; ++n) {
        const uint32_t want = walk_addr(sprite ? (n + 20) % 40 : n);
        const int v = 0x51 + n;
        bool found = false;
        std::string seen;
        for (auto& x : uw.uh) {
            if (line_of(uw.m, x.t) != v) continue;
            seen += " $" + hex(x.a, 6);
            if (x.a == want) found = true;
        }
        if (!found) {
            if (bad++ < 3) detail += " line $" + hex(unsigned(v), 3) + ": expected $" + hex(want, 6) + ", seen" + (seen.empty() ? " none" : seen) + ";";
        }
    }
    detail += " cases without their address: " + dec(bad) + " of 40";
    return bad == 0;
}

// PR-1c: the refresh accesses (R2) of a run in each complete line of run UR between t0 and t1; the lines are those of UR
// with their starts read from its markers (R1).
struct RefLines { int lines = 0, minc = 0, maxc = 0; bool ok = false; };
inline RefLines refresh_per_line(const Cap& c, const URun& ur, double K, uint64_t t0, uint64_t t1) {
    RefLines o;
    if (!(K > 0)) return o;
    std::vector<double> rt;
    for (auto& x : c.acc) if (x.ref) rt.push_back(double(x.t));
    o.minc = 1 << 30;
    for (int v = ur.d.vlo; v < ur.d.vhi; ++v) {
        double a, b;
        if (!line_start(ur, v, K, a) || !line_start(ur, v + 1, K, b)) continue;
        if (a < double(t0) || b > double(t1)) continue;
        const int n = int(std::lower_bound(rt.begin(), rt.end(), b) - std::lower_bound(rt.begin(), rt.end(), a));
        ++o.lines;
        o.minc = std::min(o.minc, n);
        o.maxc = std::max(o.maxc, n);
    }
    if (!o.lines) o.minc = 0;
    o.ok = o.lines >= 1 && o.minc >= 1;
    return o;
}

// The shared state of the runs that use R2.
struct Shared {
    HvResult hv;
    URun ur;
    bool ur_ok = false;       // the fixture of UR holds
    bool anchor = false;      // R2's anchor on UR
    bool self = false;        // UR against itself
    bool have = false;
};

inline const char* ur_bin() {
    if (mut("FIXTURE-UR")) return "prog_ur_nofix.bin";
    return (mut("R2-UR") || mut("R2-XCHK-REF") || mut("R2-XCHK-U") || mut("R2-XCHK-D")) ? "prog_ur_dma.bin" : "prog_ur.bin";
}

inline void run_shared(Checks& ck, Shared& s) {
    s.hv = run_hv(ck);
    s.ur = urun("UR", ur_bin());
    measure_u(s.ur);
    s.ur_ok = fixture_ok(s.ur.cap);
    s.anchor = ur_anchor(s.ur);
    s.self = s.ur_ok && xcheck(s.ur, s.ur);
    s.have = true;
    ck.expect(s.ur_ok, "FIXTURE-UR: run UR loaded, booted and stored the constant $BEEF at its fixture address before any access is classified");
    ck.expect(s.ur_ok && s.anchor,
              "R2-UR: between every two consecutive markers of UR there is at least one refresh access, and UR has no read inside the zero block Z (DMACON with no channel enabled, AA p.22 'Enable all DMA below (also UHRES dma)')");
    ck.note("UR: " + dec((long long)s.ur.cap.acc.size()) + " accesses, markers " + dec(s.ur.an.count) + " of " + dec((long long)s.ur.m.have.size()) + " (" + cap_core() + ")");
}

} // namespace s7r
