// s6_grade.h -- the comparators of SPECIFICATION.md: calibration rows, preconditions and the per-case checks.
#pragma once
#include "s6_cases.h"

namespace s6 {

struct CellRes {
    bool ok = true;
    std::string why;
    void fail(const std::string& s) { if (ok) { ok = false; why = s; } }
};

// ---- calibration rows (3.7, 3.8 C3 and C7) -----------------------------------------------------------
struct CalRow {
    bool ok = false;
    int first1 = -1, last1 = -1;
    std::vector<std::pair<int, int>> runs;   // (index, length) of every run between the first and the last colour(1) sample
};

inline CalRow cal_row(const Row& row) {
    CalRow c;
    int n = int(row.size());
    for (int i = 0; i < n; ++i) if (idx_of(row[i]) == 1) { if (c.first1 < 0) c.first1 = i; c.last1 = i; }
    if (c.first1 < 0) return c;
    for (int i = c.first1; i <= c.last1; ++i) {
        int id = idx_of(row[i]);
        if (id != 0 && id != 1) return c;
        if (c.runs.empty() || c.runs.back().first != id) c.runs.push_back({id, 1}); else ++c.runs.back().second;
    }
    c.ok = true;
    return c;
}

struct Geom {
    bool ok = false;
    int u = 0, r = 0;
    std::string why;
};

// u and r from the HIRES calibration rows (3.7): the rows number 8r, the interior runs of $CCCC are 2u samples.
inline Geom measure_geom(const Run& h) {
    Geom g;
    if (!h.frames_ok) { g.why = "no graded frame"; return g; }
    auto gs = groups_of(h.g);
    if (gs.empty()) { g.why = "no calibration rows"; return g; }
    size_t n = gs[0].count;
    if (n % 8 != 0 || n == 0) { g.why = "calibration rows are not a multiple of 8: " + dec(n); return g; }
    int run = -1;
    for (size_t i = 0; i < n; ++i) {
        CalRow c = cal_row(h.g[gs[0].first + i]);
        if (!c.ok || c.runs.size() < 3) { g.why = "calibration row unreadable"; return g; }
        for (size_t k = 1; k + 1 < c.runs.size(); ++k) {
            if (run < 0) run = c.runs[k].second;
            else if (c.runs[k].second != run) { g.why = "interior runs differ"; return g; }
        }
    }
    if (run < 2 || run % 2) { g.why = "interior run length " + dec(run) + " is not an even number of samples"; return g; }
    g.u = run / 2;
    g.r = int(n / 8);
    g.ok = true;
    return g;
}

// ---- preconditions (3.8) ---------------------------------------------------------------------------------
struct Pre {
    bool fix = false, c1 = false, c2 = false, c3 = false, c4 = false, c5 = false, c7 = false;
    std::string c3why, c5why, c7why;
    bool all() const { return fix && c1 && c2 && c3 && c4 && c5 && c7; }
};

inline bool read_at(const Run& r, uint32_t byte_addr) {
    for (const auto& a : r.gframe) if (!a.is_write && a.word_addr == byte_addr / 2) return true;
    return false;
}

inline Pre compute_pre(const Run& r, const Geom& G) {
    Pre p;
    p.fix = r.loaded && r.released && r.fixture && r.frames_ok;
    p.c1 = r.done_seen;
    if (!r.frames_ok) return p;
    auto gs = groups_of(r.g);

    // C2: the graded frame and the frame before it agree in every calibration row and every band row.
    bool same = !gs.empty();
    for (auto& g : gs)
        for (size_t i = g.first; i < g.first + g.count && same; ++i)
            if (i >= r.p.size() || r.p[i] != r.g[i]) same = false;
    p.c2 = same;

    // C4: sprite DMA used the prepared structures.
    p.c4 = true;
    for (int k = 0; k < 8; ++k) if (!read_at(r, spr_addr(k))) p.c4 = false;

    if (!G.ok || gs.empty()) { p.c3why = p.c5why = p.c7why = G.ok ? "no calibration rows" : "no u, r"; return p; }
    const int u = G.u;
    const Group& cg = gs[0];
    std::vector<CalRow> cal;
    bool allok = true;
    for (size_t i = 0; i < cg.count; ++i) { cal.push_back(cal_row(r.g[cg.first + i])); if (!cal.back().ok) allok = false; }

    // C3: the display resolution is the case's and the colour path is direct.
    int unit = r.c.res == 'L' ? 4 * u : r.c.res == 'H' ? 2 * u : u;
    bool c3 = allok;
    for (auto& c : cal) {
        if (!c.ok) break;
        if (c.runs.size() < 3 || c.runs.front().first != 1 || c.runs.back().first != 1) { c3 = false; p.c3why = "runs"; break; }
        for (size_t k = 1; k + 1 < c.runs.size(); ++k)
            if (c.runs[k].second != unit || c.runs[k].first != int(k % 2 == 1 ? 0 : 1)) { c3 = false; p.c3why = "run " + dec(c.runs[k].second) + " samples, expected " + dec(unit); break; }
        if (!c3) break;
    }
    p.c3 = c3;

    // C7: the row grid holds.
    bool c7 = allok && cg.count > 0;
    if (c7) {
        for (auto& c : cal) if (c.first1 != cal[0].first1) { c7 = false; p.c7why = "first colour(1) sample moves between calibration rows: " + dec(cal[0].first1) + " and " + dec(c.first1); break; }
    }
    if (c7 && r.c.res == 'H' && cg.count != size_t(8 * G.r)) { c7 = false; p.c7why = "calibration rows " + dec(cg.count) + ", expected " + dec(8 * G.r); }
    if (c7 && r.c.res == 'L' && cg.count != size_t(16 * G.r)) { c7 = false; p.c7why = "calibration rows " + dec(cg.count) + ", expected " + dec(16 * G.r); }
    if (c7 && r.c.res == 'S') {
        // SHRES: the rows that show the marker are consecutive, lie below the calibration rows and number 2r (SPECIFICATION.md 3.8 C7).
        int wl = cal[0].first1;
        for (auto& c : cal) wl = std::max(wl, c.first1);
        const size_t q = size_t(wl + 100);
        std::vector<size_t> mk;
        for (size_t i = 0; i < r.g.size(); ++i) if (q < r.g[i].size() && r.g[i][q] == kMarker) mk.push_back(i);
        if (mk.empty()) { c7 = false; p.c7why = "no row shows the marker"; }
        else if (mk.back() - mk.front() + 1 != mk.size()) { c7 = false; p.c7why = "the marker rows are not consecutive"; }
        else if (mk.front() < cg.first + cg.count) { c7 = false; p.c7why = "the marker rows start at row " + dec(mk.front()) + ", inside the calibration rows"; }
        else if (mk.size() != size_t(2 * G.r)) { c7 = false; p.c7why = "marker rows " + dec(mk.size()) + ", expected " + dec(2 * G.r); }
    }
    p.c7 = c7;

    // C5: no sprite is clipped by the display window. The window is observed in the calibration rows; the sprites'
    // programmed extent is placed against it (SPECIFICATION.md 3.8 C5). The sprites in the output are not read.
    bool c5 = allok && !cal.empty();
    if (c5) {
        int wl = cal[0].first1, wr = cal[0].last1;
        for (auto& c : cal) { wl = std::max(wl, c.first1); wr = std::min(wr, c.last1); }
        const int diw_h = r.c.diwstrt & 0xFF;
        int hmin = 1 << 30, hmax = -1;
        for (int k = 0; k < 8; ++k) { hmin = std::min(hmin, r.c.spr[k].hstart); hmax = std::max(hmax, r.c.spr[k].hstart); }
        const int exp_len = r.c.c5_px * r.c.c5_ns * u / 70;
        const int left = wl + (hmin - diw_h) * 2 * u;
        const int right = wl + (hmax - diw_h) * 2 * u + exp_len;
        if (left < wl) { c5 = false; p.c5why = "the leftmost sprite starts at sample " + dec(left) + ", left of the window's first colour(1) sample " + dec(wl); }
        else if (right > wr) { c5 = false; p.c5why = "the rightmost sprite ends at sample " + dec(right) + ", right of the window's last colour(1) sample " + dec(wr); }
    }
    p.c5 = c5;
    return p;
}

// ---- C6 (B runs): the Copper fetches of all sixteen MOVE instructions, in list order ----------------------
inline bool c6_ok(const Run& r) {
    // the list: the pointer MOVEs, then per band a WAIT and a MOVE of four words; the first band's WAIT is $4007,$FFFE
    auto list = make_copper(r.c);
    size_t w0 = list.size();
    for (size_t i = 0; i + 1 < list.size(); i += 2)
        if (list[i] == 0x4007 && list[i + 1] == 0xFFFE) { w0 = i; break; }
    if (w0 == list.size()) return false;
    uint64_t last = 0;
    for (int b = 0; b < 16; ++b) {
        uint32_t addr = kCopper + 2 * uint32_t(w0 + 4 * b + 2);
        bool found = false;
        for (const auto& a : r.gframe)
            if (!a.is_write && a.word_addr == addr / 2 && a.cycle > last) { last = a.cycle; found = true; break; }
        if (!found) return false;
    }
    return true;
}

// ---- R1 and R2 (case R) -----------------------------------------------------------------------------------------
inline bool in_data_ranges(uint32_t word) {
    uint32_t b = word * 2;
    return (b >= kCopper && b < kCopper + 0x200) || (b >= kSprBase && b < kSprBase + 8 * kSprStride) || (b >= kBpl && b < kBpl + 0xC000);
}

inline bool r1_ok(const Run& r) {
    const uint32_t vec = nmh::kKickShadowWordBase;
    size_t iv = r.early.size();
    for (size_t i = 0; i < r.early.size(); ++i)
        if (!r.early[i].is_write && r.early[i].word_addr >= vec && r.early[i].word_addr < vec + 4) { iv = i; break; }
    if (iv == r.early.size()) return false;
    for (size_t i = iv + 1; i < r.early.size(); ++i) {
        const auto& a = r.early[i];
        if (a.is_write) continue;
        if (a.word_addr >= vec && a.word_addr < vec + 4) continue;
        if (in_data_ranges(a.word_addr)) return false;
        if (a.word_addr >= 0x040000 && a.word_addr < 0x100000) return a.word_addr == r.c.entry / 2;
    }
    return false;
}

inline bool r2_ok(const Run& r) {
    if (r.list_mem.size() < 4) return false;
    int n10c = 0; uint16_t v10c = 0; bool cop1_ok = true, other = false;
    size_t i = 0;
    for (; i + 1 < r.list_mem.size(); i += 2) {
        uint16_t reg = r.list_mem[i], val = r.list_mem[i + 1];
        if (reg == 0xFFFF) break;
        if (reg == R_BPLCON4) { ++n10c; v10c = val; }
        if (reg == R_COP1LCH && val != uint16_t(kCopper >> 16)) cop1_ok = false;
        if (reg == R_COP1LCL && val != uint16_t(kCopper & 0xFFFF)) cop1_ok = false;
        if (reg == 0x084 || reg == 0x086 || reg == 0x08A) other = true;
    }
    if (i + 1 >= r.list_mem.size()) return false;   // no terminator
    bool want_write = r.c.bplcon4.has_value();
    if (n10c != (want_write ? 1 : 0)) return false;
    if (want_write && v10c != *r.c.bplcon4) return false;
    if (!cop1_ok || other) return false;
    if (!r.other_list_reads.empty()) return false;   // the Copper ran a list other than the case's
    // the Copper list as the run ended: no MOVE to BPLCON4
    for (size_t k = 0; k + 1 < r.cop_mem.size();) {
        uint16_t w = r.cop_mem[k];
        if (w == 0xFFFF && r.cop_mem[k + 1] == 0xFFFE) return true;
        if ((w & 1) == 0 && w == R_BPLCON4) return false;
        k += 2;
    }
    return false;
}

// ---- per-case checks ---------------------------------------------------------------------------------------------------
inline bool same_colour(const Row& row, const Seg& s) {
    int c = idx_of(row[s.L]);
    for (int i = 0; i < s.len; ++i) if (idx_of(row[s.L + i]) != c) return false;
    return true;
}

inline std::vector<Group> bands_of(const Run& r) {
    auto gs = groups_of(r.g);
    if (!gs.empty()) gs.erase(gs.begin());
    return gs;
}

// P1, P2, P4, P5 (SPECIFICATION.md 4.1): checks (a) to (d) on all eight bands for a sprite pixel width of ns.
inline CellRes grade_m(const Run& r, int ns, const Geom& G, std::string* rec = nullptr) {
    CellRes res;
    if (!r.frames_ok || !G.ok) { res.fail("no frame or no u, r"); return res; }
    const int u = G.u, rr = G.r;
    auto bs = bands_of(r);
    if (bs.size() != 8) { res.fail(dec(bs.size()) + " bands, expected 8"); return res; }
    const int W = 16 * ns * u / 70;
    int unit = ns == 140 ? 2 * u : ns == 70 ? u : (u == 1 ? 0 : (u % 2 == 0 ? u / 2 : -1));
    if (unit < 0 && rec) *rec = "u is odd and above 1: the 35 ns form of check (d) is recorded, not graded";
    for (size_t k = 0; k < 8; ++k) {
        const Group& b = bs[k];
        if (b.count != size_t(6 * rr)) { res.fail("band " + dec(k) + " has " + dec(b.count) + " rows, expected " + dec(6 * rr)); continue; }
        int Lref = -1, c3 = -2;
        for (size_t i = 0; i < b.count; ++i) {
            const Row& row = r.g[b.first + i];
            auto sg = segs_of(row);
            if (sg.size() != 1) { res.fail("band " + dec(k) + " row " + dec(i) + " has " + dec(sg.size()) + " segments"); continue; }
            const Seg& s = sg[0];
            if (s.len != W) { res.fail("band " + dec(k) + " row " + dec(i) + " segment length " + dec(s.len) + ", expected " + dec(W)); continue; }
            if (i == 0) Lref = s.L; else if (s.L != Lref) res.fail("band " + dec(k) + " rows start at different samples");
            if (i < size_t(3 * rr)) {
                if (!same_colour(row, s)) { res.fail("band " + dec(k) + " solid row " + dec(i) + " has more than one colour"); continue; }
                int c = idx_of(row[s.L]);
                if (i == 0) c3 = c; else if (c != c3) res.fail("band " + dec(k) + " solid rows differ in colour");
            } else {
                if (unit == 0) {
                    if (!same_colour(row, s)) res.fail("band " + dec(k) + " alternating row " + dec(i) + " has more than one colour at 35 ns with u = 1");
                } else if (unit > 0) {
                    int c1 = -2;
                    for (int j = 0; j < 16; ++j) {
                        int x = idx_of(row[s.L + j * unit]);
                        for (int q = 1; q < unit; ++q) if (idx_of(row[s.L + j * unit + q]) != x) { res.fail("band " + dec(k) + " row " + dec(i) + " run " + dec(j) + " is not one colour"); break; }
                        if (j % 2 == 0) { if (x != c3) res.fail("band " + dec(k) + " row " + dec(i) + " run " + dec(j) + " is not c3"); }
                        else { if (c1 == -2) c1 = x; else if (x != c1) res.fail("band " + dec(k) + " row " + dec(i) + " odd runs differ in colour"); if (x == c3) res.fail("band " + dec(k) + " row " + dec(i) + " c1 equals c3"); }
                    }
                }
            }
        }
    }
    return res;
}

// P3: every row of every band is one segment of the given length (in samples).
inline CellRes grade_len(const Run& r, int len, const Geom& G) {
    CellRes res;
    if (!r.frames_ok || !G.ok) { res.fail("no frame or no u, r"); return res; }
    auto bs = bands_of(r);
    if (bs.size() != 8) { res.fail(dec(bs.size()) + " bands, expected 8"); return res; }
    for (size_t k = 0; k < 8; ++k) {
        if (bs[k].count != size_t(6 * G.r)) { res.fail("band " + dec(k) + " has " + dec(bs[k].count) + " rows"); continue; }
        for (size_t i = 0; i < bs[k].count; ++i) {
            auto sg = segs_of(r.g[bs[k].first + i]);
            if (sg.size() != 1 || sg[0].len != len) res.fail("band " + dec(k) + " row " + dec(i) + ": " + (sg.size() == 1 ? "segment length " + dec(sg[0].len) : dec(sg.size()) + " segments") + ", expected " + dec(len));
        }
    }
    return res;
}

// P6: width index e = 0..3 (W-00, W-01, W-10, W-11).
inline CellRes grade_w(const Run& r, int e, const Geom& G, int* width_px = nullptr) {
    CellRes res;
    if (!r.frames_ok || !G.ok) { res.fail("no frame or no u, r"); return res; }
    static const int wpx[4] = {16, 32, 32, 64};
    const int u = G.u, rr = G.r, parts = wpx[e] / 16;
    auto bs = bands_of(r);
    if (bs.size() != 8) { res.fail(dec(bs.size()) + " bands, expected 8"); return res; }
    for (size_t k = 0; k < 8; ++k) {
        const Group& b = bs[k];
        if (b.count != size_t(6 * rr)) { res.fail("band " + dec(k) + " has " + dec(b.count) + " rows"); continue; }
        int Ls = -1;
        for (size_t i = 0; i < b.count; ++i) {
            const Row& row = r.g[b.first + i];
            auto sg = segs_of(row);
            if (sg.size() != 1) { res.fail("band " + dec(k) + " row " + dec(i) + " has " + dec(sg.size()) + " segments"); continue; }
            const Seg& s = sg[0];
            if (i < size_t(3 * rr)) {
                if (width_px && k == 0 && i == 0) *width_px = s.len / (2 * u);
                if (!same_colour(row, s)) res.fail("band " + dec(k) + " solid row " + dec(i) + " has more than one colour");
                if (s.len != wpx[e] * 2 * u) res.fail("band " + dec(k) + " solid row " + dec(i) + " segment length " + dec(s.len) + ", expected " + dec(wpx[e] * 2 * u));
                if (i == 0) Ls = s.L; else if (s.L != Ls) res.fail("band " + dec(k) + " solid rows start at different samples");
            } else {
                if (s.len != 32 * u) res.fail("band " + dec(k) + " pattern row " + dec(i) + " segment length " + dec(s.len) + ", expected " + dec(32 * u));
                if (e == 0) { if (!same_colour(row, s)) res.fail("band " + dec(k) + " pattern row " + dec(i) + " has more than one colour"); }
                else {
                    int d = s.L - Ls;
                    if (d < 0 || d % (32 * u) != 0 || d / (32 * u) >= parts) res.fail("band " + dec(k) + " pattern row " + dec(i) + " starts " + dec(d) + " samples from the solid rows' start");
                }
            }
        }
    }
    return res;
}

// P7 (SPECIFICATION.md 6): attached pairs. G = samples per group of two sprite pixels; parts = 1, 2 or 4 sixteen-pixel parts.
inline CellRes grade_a(const Run& r, int Gs, int parts, const Geom& G, std::string* rec = nullptr) {
    CellRes res;
    (void)rec;
    if (!r.frames_ok || !G.ok) { res.fail("no frame or no u, r"); return res; }
    const int rr = G.r;
    auto bs = bands_of(r);
    if (bs.size() != 4) { res.fail(dec(bs.size()) + " bands, expected 4"); return res; }
    for (size_t p = 0; p < 4; ++p) {
        const Group& b = bs[p];
        if (b.count != size_t(8 * rr)) { res.fail("band " + dec(p) + " has " + dec(b.count) + " rows, expected " + dec(8 * rr)); continue; }
        for (size_t i = 0; i < b.count; ++i) {
            bool typeB = ((i / size_t(rr)) & 1) != 0;
            const Row& row = r.g[b.first + i];
            int L = -1;
            for (size_t x = 0; x < row.size(); ++x) if (opaque(row[x])) { L = int(x); break; }
            if (L < 0) { res.fail("band " + dec(p) + " row " + dec(i) + " has no opaque sample"); continue; }
            std::string obs, exp;
            bool row_ok = true;
            for (int g = 0; g < 8 * parts; ++g) {
                int v = a_value(typeB, g);
                int want = v == 0 ? 0 : 16 + v;
                int got = -2;
                bool uniform = true;
                for (int q = 0; q < Gs; ++q) {
                    size_t x = size_t(L + g * Gs + q);
                    int id = x < row.size() ? idx_of(row[x]) : -3;
                    if (q == 0) got = id; else if (id != got) uniform = false;
                }
                obs += (g ? "," : "") + (uniform ? dec(got) : std::string("mixed"));
                exp += (g ? "," : "") + dec(want);
                if (!uniform || got != want) row_ok = false;
            }
            if (!row_ok) res.fail("band " + dec(p) + " row " + dec(i) + " shows the groups [" + obs + "] where colour indices [" + exp + "] are expected");
            auto sg = segs_of(row);
            size_t want_segs = typeB ? size_t(parts) : 1;
            if (sg.size() != want_segs) res.fail("band " + dec(p) + " row " + dec(i) + " has " + dec(sg.size()) + " segments, expected " + dec(want_segs));
        }
    }
    return res;
}

// P8 (SPECIFICATION.md 6): sixteen colour banks. attached: B-A; the MOVE data of band b is (b<<4)|(15-b).
inline CellRes grade_b(const Run& r, bool attached, const Geom& G) {
    CellRes res;
    if (!r.frames_ok || !G.ok) { res.fail("no frame or no u, r"); return res; }
    const int u = G.u, rr = G.r;
    auto bs = bands_of(r);
    if (bs.size() != 16) { res.fail(dec(bs.size()) + " bands, expected 16"); return res; }
    for (int b = 0; b < 16; ++b) {
        const Group& bd = bs[size_t(b)];
        if (bd.count != size_t(7 * rr)) { res.fail("band " + dec(b) + " has " + dec(bd.count) + " rows, expected " + dec(7 * rr)); continue; }
        for (size_t i = 0; i < bd.count; ++i) {
            const Row& row = r.g[bd.first + i];
            auto sg = segs_of(row);
            size_t nseg = attached ? 4 : 8;
            if (sg.size() != nseg) { res.fail("band " + dec(b) + " row " + dec(i) + " has " + dec(sg.size()) + " segments, expected " + dec(nseg)); continue; }
            bool typeB = ((i / size_t(rr)) & 1) != 0;
            for (size_t k = 0; k < nseg; ++k) {
                if (!attached) {
                    int bank = (k % 2 == 0) ? b : 15 - b;
                    for (int g = 0; g < 3; ++g)
                        for (int q = 0; q < 8 * u; ++q) {
                            size_t x = size_t(sg[k].L + g * 8 * u + q);
                            int want = bank * 16 + int(k / 2) * 4 + (g + 1);
                            if (x >= row.size() || idx_of(row[x]) != want) { res.fail("band " + dec(b) + " row " + dec(i) + " sprite " + dec(k) + " group " + dec(g) + " is not colour(" + dec(want) + ")"); g = 3; break; }
                        }
                } else {
                    for (int g = 0; g < 8; ++g) {
                        int v = a_value(typeB, g);
                        int want = v == 0 ? 0 : (15 - b) * 16 + v;
                        for (int q = 0; q < 4 * u; ++q) {
                            size_t x = size_t(sg[k].L + g * 4 * u + q);
                            if (x >= row.size() || idx_of(row[x]) != want) { res.fail("band " + dec(b) + " row " + dec(i) + " pair " + dec(k) + " group " + dec(g) + " is not colour(" + dec(want) + ")"); g = 8; break; }
                        }
                    }
                }
            }
        }
    }
    return res;
}

// P9 and P10 (case R): bit 4 of the decoded index of every group of the even (odd) sprites.
inline CellRes grade_r(const Run& r, bool odd, const Geom& G, std::string* bits75 = nullptr) {
    CellRes res;
    if (!r.frames_ok || !G.ok) { res.fail("no frame or no u, r"); return res; }
    const int u = G.u, rr = G.r;
    auto bs = bands_of(r);
    if (bs.size() != 1) { res.fail(dec(bs.size()) + " bands, expected 1"); return res; }
    if (bs[0].count != size_t(8 * rr)) { res.fail("band has " + dec(bs[0].count) + " rows, expected " + dec(8 * rr)); return res; }
    int mism75 = 0, checked = 0;
    for (size_t i = 0; i < bs[0].count; ++i) {
        const Row& row = r.g[bs[0].first + i];
        auto sg = segs_of(row);
        if (sg.size() != 8) { res.fail("row " + dec(i) + " has " + dec(sg.size()) + " segments"); continue; }
        for (size_t k = 0; k < 8; ++k) {
            if ((k % 2 == 1) != odd) continue;
            for (int g = 0; g < 3; ++g)
                for (int q = 0; q < 8 * u; ++q) {
                    size_t x = size_t(sg[k].L + g * 8 * u + q);
                    int id = x < row.size() ? idx_of(row[x]) : -1;
                    ++checked;
                    if (id < 0 || ((id >> 4) & 1) != 1) res.fail("row " + dec(i) + " sprite " + dec(k) + " group " + dec(g) + " has index " + dec(id) + ", bit 4 is not 1");
                    if (id != 16 + int(k / 2) * 4 + (g + 1)) ++mism75;
                }
        }
    }
    if (bits75) *bits75 = dec(mism75) + " of " + dec(checked) + " samples differ from index 16 + (k>>1)*4 + v";
    return res;
}

}  // namespace s6
