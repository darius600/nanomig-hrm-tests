// s6_base.h -- aa-integ-s6: cases, stimulus, one run of nanomig.v, and the reading of its video output.
// See SPECIFICATION.md. Everything here drives and reads the ports of nanomig.v through nanomig_harness.h.
#pragma once

#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <memory>
#include <optional>
#include <algorithm>
#include <iostream>

#include "nanomig_harness.h"

namespace s6 {

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

inline std::string g_dir = ".";
inline const char* g_mutate = nullptr;   // NMH_MUTATE: unset, the normal run never runs a mutant
inline bool mut(const char* id) { return g_mutate && std::string(g_mutate) == id; }

inline std::string dec(long long v) { return std::to_string(v); }
inline std::string hexs(uint32_t v, int digits = 4) { char b[16]; std::snprintf(b, sizeof b, "%0*X", digits, v); return b; }

// ---- memory layout (SPECIFICATION.md 3.9) ---------------------------------
constexpr uint32_t kProgOrg   = 0x080400;
constexpr uint32_t kCopper    = 0x088000;
constexpr uint32_t kProLst    = 0x088100;
constexpr uint32_t kSprBase   = 0x089000;
constexpr uint32_t kSprStride = 0x400;
constexpr uint32_t kBpl       = 0x090000;
constexpr uint32_t kResults   = 0x0C0000;
constexpr uint32_t kDoneAddr  = 0x0C0FFE;
constexpr uint16_t kDoneVal   = 0x600D;
constexpr uint32_t kRList     = 0x0B8000;   // the register list the program interprets (build addition)
constexpr uint32_t kProEntry  = kProgOrg + 0x40;

inline uint32_t spr_addr(int k) { return kSprBase + kSprStride * uint32_t(k); }

// custom registers (offsets from $DFF000)
enum : uint16_t {
    R_COP1LCH = 0x080, R_COP1LCL = 0x082, R_COPJMP1 = 0x088, R_DIWSTRT = 0x08E, R_DIWSTOP = 0x090,
    R_DDFSTRT = 0x092, R_DDFSTOP = 0x094, R_DMACON = 0x096, R_INTENA = 0x09A, R_BPLCON0 = 0x100,
    R_BPLCON1 = 0x102, R_BPLCON2 = 0x104, R_BPLCON3 = 0x106, R_BPL1MOD = 0x108, R_BPL2MOD = 0x10A,
    R_BPLCON4 = 0x10C, R_COLOR00 = 0x180, R_FMODE = 0x1FC, R_BPL1PTH = 0x0E0, R_BPL1PTL = 0x0E2,
    R_SPR0PTH = 0x120
};

inline uint16_t colour(int i) { return uint16_t(((i >> 4) & 15) << 8 | (i & 15) << 4 | 0xA); }

// ---- sprite data ------------------------------------------------------------
struct Line { uint16_t a[4] = {0, 0, 0, 0}, b[4] = {0, 0, 0, 0}; };   // DATA and DATB words of one fetch
struct Sprite {
    int hstart = 0xA0, vstart = 0, vstop = 0;
    bool att = false;
    std::vector<Line> lines;
};
inline Line line_all(uint16_t a, uint16_t b, int n) { Line l; for (int i = 0; i < n; ++i) { l.a[i] = a; l.b[i] = b; } return l; }

// ---- one case ----------------------------------------------------------------
struct Case {
    std::string name;
    char     res = 'L';                 // display resolution: L, H, S
    uint16_t bplcon0 = 0x1200;
    int      spres = 0;                 // SPRES1,SPRES0
    std::optional<uint16_t> bplcon4 = uint16_t(0x0011);
    uint16_t fmode = 0;
    uint16_t diwstrt = 0x2C81, diwstop = 0x2CC1, ddfstrt = 0x0038, ddfstop = 0x00D0;
    uint16_t dmacon = 0x83A0;
    int      n = 1;                     // words per sprite fetch: 1, 2 or 4
    Sprite   spr[8];
    std::vector<uint16_t> cop_extra;    // case specific part of the Copper list
    int      calib_bytes = 640;
    std::string prog = "prog_main.bin";
    bool     expect_done = true;        // the program writes DONE_VALUE
    uint32_t entry = kProgOrg;          // address of the program's first instruction
    bool     prologue_list = false;     // a one-instruction Copper list at kProLst
    std::vector<std::pair<uint16_t, uint16_t>> extra_regs;   // register-list entries inserted before step 5
    // expected sprite length used by precondition C5 (pixels of c5_ns nanoseconds, from the proposition)
    int c5_px = 16, c5_ns = 140;
    int bands_expected = 8;             // sprite bands below the calibration rows
};

inline uint16_t bplcon0_of(char res) { return res == 'L' ? 0x1200 : res == 'H' ? 0x9200 : 0x1240; }

inline void set_res(Case& c, char res) {
    c.res = res;
    c.bplcon0 = bplcon0_of(res);
    c.ddfstrt = res == 'L' ? 0x0038 : 0x003C;
    c.ddfstop = res == 'L' ? 0x00D0 : 0x00D4;
}

// Sprite pixel width in nanoseconds for a display resolution and SPRES (SPECIFICATION.md 2).
inline int pixel_ns(char res, int spres) {
    switch (spres) {
    case 0: return res == 'S' ? 70 : 140;
    case 1: return 140;
    case 2: return 70;
    default: return 35;
    }
}

// ---- the register list ---------------------------------------------------------
using RegList = std::vector<std::pair<uint16_t, uint16_t>>;

inline RegList make_list(const Case& c) {
    RegList l;
    l.push_back({R_INTENA, 0x7FFF});
    l.push_back({R_DMACON, 0x7FFF});
    l.push_back({R_BPLCON2, 0x0024});
    for (int n = 0; n < 8; ++n) {
        l.push_back({R_BPLCON3, uint16_t((n << 13) | 0x0C00)});
        for (int j = 0; j < 32; ++j) l.push_back({uint16_t(R_COLOR00 + 2 * j), colour(32 * n + j)});
    }
    l.push_back({R_BPLCON0, c.bplcon0});
    l.push_back({R_BPLCON1, 0x0000});
    l.push_back({R_BPLCON3, uint16_t(0x0C00 | (c.spres << 6))});
    if (c.bplcon4) l.push_back({R_BPLCON4, *c.bplcon4});
    l.push_back({R_FMODE, c.fmode});
    l.push_back({R_BPL1MOD, 0});
    l.push_back({R_BPL2MOD, 0});
    l.push_back({R_DIWSTRT, c.diwstrt});
    l.push_back({R_DIWSTOP, c.diwstop});
    l.push_back({R_DDFSTRT, c.ddfstrt});
    l.push_back({R_DDFSTOP, c.ddfstop});
    for (auto& e : c.extra_regs) l.push_back(e);
    l.push_back({R_COP1LCH, uint16_t(kCopper >> 16)});
    l.push_back({R_COP1LCL, uint16_t(kCopper & 0xFFFF)});
    l.push_back({R_COPJMP1, 0});
    l.push_back({R_DMACON, c.dmacon});
    return l;
}

inline std::vector<uint16_t> cop_move(uint16_t reg, uint16_t data) { return {reg, data}; }
inline std::vector<uint16_t> cop_wait(int v, int h) { return {uint16_t(((v & 0xFF) << 8) | (h & 0xFE) | 1), 0xFFFE}; }
inline void append(std::vector<uint16_t>& a, const std::vector<uint16_t>& b) { a.insert(a.end(), b.begin(), b.end()); }

inline std::vector<uint16_t> make_copper(const Case& c) {
    std::vector<uint16_t> w;
    append(w, cop_move(R_BPL1PTH, uint16_t(kBpl >> 16)));
    append(w, cop_move(R_BPL1PTL, uint16_t(kBpl & 0xFFFF)));
    for (int x = 0; x < 8; ++x) {
        append(w, cop_move(uint16_t(R_SPR0PTH + 4 * x), uint16_t(spr_addr(x) >> 16)));
        append(w, cop_move(uint16_t(R_SPR0PTH + 4 * x + 2), uint16_t(spr_addr(x) & 0xFFFF)));
    }
    append(w, c.cop_extra);
    w.push_back(0xFFFF);
    w.push_back(0xFFFE);
    return w;
}

inline void poke_sprite(nmh::NanomigHarness& h, int k, const Sprite& s, int n) {
    uint32_t w = spr_addr(k) / 2;
    uint16_t pos = uint16_t(((s.vstart & 0xFF) << 8) | ((s.hstart >> 1) & 0xFF));
    uint16_t ctl = uint16_t(((s.vstop & 0xFF) << 8) | (s.att ? 0x80 : 0) | (s.hstart & 1));
    for (int i = 0; i < n; ++i) h.poke_word(w++, pos);
    for (int i = 0; i < n; ++i) h.poke_word(w++, ctl);
    for (auto& l : s.lines) {
        for (int i = 0; i < n; ++i) h.poke_word(w++, l.a[i]);
        for (int i = 0; i < n; ++i) h.poke_word(w++, l.b[i]);
    }
    for (int i = 0; i < 2 * n; ++i) h.poke_word(w++, 0);
}

// ---- video samples ----------------------------------------------------------------
using Row = std::vector<uint16_t>;                // one entry per sample: r<<8 | g<<4 | b
inline int idx_of(uint16_t raw) { int r = raw >> 8, g = (raw >> 4) & 15, b = raw & 15; return b == 0xA ? r * 16 + g : -1; }
inline bool opaque(uint16_t raw) { return idx_of(raw) > 0; }

inline std::vector<Row> rows_in(const std::vector<nmh::VideoSample>& v, nmh::Span f) {
    std::vector<size_t> starts;
    bool prev = v[f.begin].hs;
    for (size_t i = f.begin + 1; i < f.end; ++i) {
        if (v[i].hs && !prev) starts.push_back(i);
        prev = v[i].hs;
    }
    std::vector<Row> rows;
    for (size_t k = 0; k + 1 < starts.size(); ++k) {
        Row r;
        r.reserve(starts[k + 1] - starts[k]);
        for (size_t i = starts[k]; i < starts[k + 1]; ++i)
            r.push_back(uint16_t((v[i].r & 15) << 8 | (v[i].g & 15) << 4 | (v[i].b & 15)));
        rows.push_back(std::move(r));
    }
    return rows;
}

// ---- one run -------------------------------------------------------------------------
struct Run {
    std::string name;
    Case c;
    bool loaded = false, released = false, fixture = false, done_seen = false, frames_ok = false;
    uint16_t lisa_id = 0;
    uint64_t done_cycle = 0, gb = 0, ge = 0;
    std::vector<Row> g, p;                          // graded frame and the frame before it
    std::vector<nmh::RamAccess> early;              // the first accesses of the run
    std::vector<nmh::RamAccess> gframe;             // accesses of the graded frame in $088000-$08FFFF
    std::vector<uint16_t> list_mem;                 // the register list as the run ended
    std::vector<uint16_t> cop_mem;                  // the Copper list as the run ended
    size_t frames_total = 0;
};

constexpr uint64_t kBootBudget = 4'000'000;
constexpr uint64_t kDoneBudget = 8'000'000;
constexpr uint64_t kNoDoneCycles = 5'400'000;

inline std::shared_ptr<Run> execute(const Case& c) {
    auto r = std::make_shared<Run>();
    r->name = c.name;
    r->c = c;
    nmh::Config cfg;
    cfg.chipset_config = 0x10;
    nmh::NanomigHarness h(cfg);

    RegList list = make_list(c);
    uint32_t w = kRList / 2;
    for (auto& e : list) { h.poke_word(w++, e.first); h.poke_word(w++, e.second); }
    h.poke_word(w++, 0xFFFF);
    h.poke_word(w++, 0);
    auto cop = make_copper(c);
    for (size_t i = 0; i < cop.size(); ++i) h.poke_word(kCopper / 2 + i, cop[i]);
    if (c.prologue_list) { h.poke_word(kProLst / 2, 0xFFFF); h.poke_word(kProLst / 2 + 1, 0xFFFE); }
    for (int k = 0; k < 8; ++k) poke_sprite(h, k, c.spr[k], c.n);
    for (int i = 0; i < c.calib_bytes / 2; ++i) h.poke_word(kBpl / 2 + i, 0xCCCC);

    r->loaded = h.load_program(g_dir + "/" + c.prog) > 0;
    if (!r->loaded) return r;
    r->released = h.boot(kBootBudget).cpu_released;
    if (!r->released) return r;

    if (c.expect_done) {
        auto d = h.run_until_write(kDoneAddr, kDoneVal, kDoneBudget);
        if (d) {
            r->done_seen = true;
            r->done_cycle = *d;
            const auto& v = h.video();
            bool prev = v[*d].vs;
            size_t pos = *d + 1;
            int edges = 0;
            for (int guard = 0; guard < 400 && edges < 4; ++guard) {
                h.run_cycles(10000);
                const auto& vv = h.video();
                for (; pos < vv.size(); ++pos) {
                    if (vv[pos].vs && !prev) ++edges;
                    prev = vv[pos].vs;
                }
            }
        } else {
            h.run_cycles(kNoDoneCycles);
        }
    } else {
        h.run_cycles(kNoDoneCycles);
    }

    auto fx = h.peek_logical(kResults);
    r->fixture = fx.has_value() && *fx == 0xBEEF;
    auto lid = h.peek_logical(kResults + 4);
    r->lisa_id = lid.value_or(0);

    auto frames = nmh::find_frames(h.video());
    r->frames_total = frames.size();
    if (frames.size() >= 4) {
        nmh::Span gs = frames[frames.size() - 2], ps = frames[frames.size() - 3];
        r->gb = gs.begin; r->ge = gs.end;
        r->g = rows_in(h.video(), gs);
        r->p = rows_in(h.video(), ps);
        r->frames_ok = !r->g.empty() && !r->p.empty();
    }
    const auto& lg = h.ram_log();
    {
        // the accesses from the first read of a reset vector on: the RAM port reads address 0 while the CPU is held in reset
        size_t iv = lg.size();
        for (size_t i = 0; i < lg.size(); ++i)
            if (!lg[i].is_write && lg[i].word_addr >= nmh::kKickShadowWordBase && lg[i].word_addr < nmh::kKickShadowWordBase + 4) { iv = i; break; }
        for (size_t i = iv; i < lg.size() && i < iv + 400; ++i) r->early.push_back(lg[i]);
    }
    for (const auto& a : lg)
        if (a.cycle >= r->gb && a.cycle < r->ge && a.word_addr >= 0x044000 && a.word_addr < 0x048000)
            r->gframe.push_back(a);
    for (int i = 0; i < 700; ++i) r->list_mem.push_back(h.peek_word(kRList / 2 + i));
    for (int i = 0; i < 128; ++i) r->cop_mem.push_back(h.peek_word(kCopper / 2 + i));
    return r;
}

// ---- reading rows ----------------------------------------------------------------------------
struct Seg { int L = 0, len = 0; };

inline std::vector<Seg> segs_of(const Row& row) {
    std::vector<Seg> s;
    int n = int(row.size());
    for (int i = 0; i < n;) {
        if (opaque(row[i])) {
            int j = i;
            while (j < n && opaque(row[j])) ++j;
            s.push_back({i, j - i});
            i = j;
        } else ++i;
    }
    return s;
}

inline bool row_opaque(const Row& row) { for (auto x : row) if (opaque(x)) return true; return false; }

struct Group { size_t first, count; };   // consecutive rows that contain opaque samples

inline std::vector<Group> groups_of(const std::vector<Row>& rows) {
    std::vector<Group> gs;
    for (size_t i = 0; i < rows.size();) {
        if (row_opaque(rows[i])) {
            size_t j = i;
            while (j < rows.size() && row_opaque(rows[j])) ++j;
            gs.push_back({i, j - i});
            i = j;
        } else ++i;
    }
    return gs;
}

}  // namespace s6
