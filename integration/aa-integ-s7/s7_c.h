// s7_c.h -- the Copper-case procedure of SPECIFICATION.md 1.9 and the propositions C1-C12.
#pragma once

#include "s7_core.h"

namespace s7 {

// ---- memory map of the propositions (the build section of SPECIFICATION.md) ------
constexpr uint32_t kIdle  = 0x090000;   // {0xFFFF, 0xFFFE}
constexpr uint32_t kCase  = 0x090100;   // the case list
constexpr uint32_t kDst   = 0x1B0200;   // blitter destination (C7-C12, F3, F8), in the top of Chip memory away from RES, the stack and the S regions
constexpr uint32_t kDst2  = 0x1A0000;   // F1, F2: at least 32767 words
constexpr uint32_t kAsrc  = 0x0B2000;
constexpr uint32_t kBsrc  = 0x0B2100;
constexpr uint32_t kCsrc  = 0x0B2200;
constexpr uint32_t kPx    = 0x0B2300;   // C12 preload source: [PX, PX+4)
constexpr uint32_t kDst3  = 0x1B0400;   // C12 preload destination
constexpr uint32_t kRd    = 0x0E0000;   // C2 disk regions: A16 = 0, (Rd & 0xFFFF) <= 0xFEF8
constexpr uint32_t kRe    = kRd + 0x10000;
constexpr uint32_t kRe2   = kRd + 0x0100;
constexpr unsigned kC2Lines = 8;        // the LINES wait that closes every window of C2

// An observation: a list of values; a blitter write is (address << 16) | data, a register read its value.
using Obs = std::vector<uint64_t>;

// True while the script of a frame B's observation is being built: the population contrasts apply to frame B only,
// so that the calibration of frame A (PC4) stays as it is.
inline bool g_frame_b = false;

inline std::string obs_str(const Obs& o, bool wide = false) {
    std::string s = "{";
    for (size_t i = 0; i < o.size() && i < 12; ++i) {
        if (i) s += ",";
        s += wide ? hex6(unsigned(o[i] >> 16)) + ":" + hex4(unsigned(o[i])) : (o[i] > 0xFFFF ? "$" + hex6(unsigned(o[i])) : "$" + hex4(unsigned(o[i])));
    }
    if (o.size() > 12) s += ",..." + dec((long long)o.size()) + " in all";
    return s + "}";
}

struct Ctx { const Cap* cap; const std::vector<Acc>* acc; };

struct CalA   { int ms = 0, me = 0; std::vector<int> idx; };

// One register case of a C proposition: what V0, V1 and the observation are.
struct Variant {
    std::string name;
    std::vector<uint16_t> caselist;                    // the words of the case list
    uint16_t reg = 0, v1 = 0;
    std::function<void(Script&)> v0, v1cpu;            // CPU writes establishing V0, and writing V1
    std::function<std::vector<int>(Script&)> obs;      // step 8: stimulus; returns the log indexes it created
    std::function<Obs(const Ctx&, uint64_t, uint64_t, const std::vector<int>&)> ex;   // observation in a window
    Obs U, W;
    bool cal1_mark_before_v1 = false;                  // C10: the CPU write itself starts the blit
    bool b_window_from_start = false;                  // C10: the window of frame B is (M_start, M_end)
    bool obs_replaces_step6 = false;                   // C4
    bool blitter = false;                              // PC5 applies; blitter regions are `chan`
    bool wide = false;                                 // print observations as address:data
    std::vector<Region> chan;                          // regions of the channel
    std::function<bool(const Ctx&, uint64_t, uint64_t, const Obs&)> pc6;   // population precondition on one observation, null if none
    std::function<void(Script&)> after;                // after M_end of a frame B and after each calibration observation
    std::function<void(Script&, CalA&, CalA&)> cal;    // replaces the default frame A (C4)
    Obs U_cal0;                                        // when set, R_cal0 is compared with this and not with U
    bool has_ucal0 = false;
    std::function<bool(const Ctx&, uint64_t, uint64_t, const std::vector<int>&)> runs_ok;   // C2: the channel read at all
    std::function<bool(const Cap&, const std::vector<int>&)> run_pre;                       // C2: the precondition of the run (DMAON)
    std::shared_ptr<std::vector<int>> inject_marks;    // C6: markers after which the PC7 contrast moves the mouse
};

struct FrameB { uint16_t c = 0; int ms = 0, mw = 0, me = 0, f_vb = -1; std::vector<int> idx; };
struct VarPlan { Variant v; CalA c1, c0; std::vector<FrameB> b; int dma_idx = -1; uint32_t case_addr = kCase; };

struct Prop {
    std::string id;                        // "C1"
    std::vector<Variant> variants;
    std::vector<uint16_t> cs = {0x0000, 0xFFFD, 0x0002};
    uint16_t dmacon_once = 0;              // DMACON SET bits written once before frame A (0 = none)
    uint16_t adkcon_once = 0;
    uint16_t dma_enable_bit = 0;           // the DMACON enable bit of the channel for DMA_CHECK
    std::function<void(Script&)> extra_once;
    std::vector<Poke> pokes;               // memory the regions hold before the run
    unsigned batch = 0;                    // 0: one register case per frame (the procedure of SPECIFICATION.md 1.9); n > 0: the cases run n at a time (1.9, walking values)
    std::string pcx = "PC6";               // the label of the population precondition
    bool refresh_pc = false;               // the refresh precondition of 1.6 applies
    bool mouse_contrast = false;           // C6: the population contrast moves the mouse between the two reads
};

inline void prologue(Script& s) {
    s.w16(INTENA, 0x7FFF); s.w16(INTREQ, 0x7FFF); s.w16(DMACON, 0x7FFF);
    s.w16(ADKCON, 0x7FFF);
    s.w16(COPCON, 0); s.w16(FMODE, 0); s.w16(BPLCON0, 0); s.w16(BPLCON1, 0); s.w16(BPLCON2, 0);
    s.w16(BPLCON3, 0x0C00); s.w16(BPLCON4, 0x0011);
    s.w16(POTGO, 0);
    for (int x = 0; x < 4; ++x) s.w16(audvol(x), 0);
    s.w32(COP1LCH, kIdle);
    s.w16(COPJMP1, 0);
    s.w16(DMACON, 0x8280);
}

inline std::vector<uint16_t> idle_list() { return {0xFFFF, 0xFFFE}; }

inline std::vector<uint16_t> case_list(uint16_t reg, uint16_t v1) {
    return {0x2001, 0xFFFE, reg, v1, 0xFFFF, 0xFFFE};
}

struct CRun {
    Cap cap;
    std::vector<VarPlan> plan;
    int dma_idx = -1;
    std::vector<Acc> acc;
};

// Builds and runs the script of a proposition (frame A, then frame B for each COPCON value), per variant.
// `sel` is the mutation selector in force: "" for none.
inline CRun run_c(const Prop& p, const std::string& sel) {
    CRun r;
    RunSpec rs;
    rs.bin = (sel == p.id + "-FIXTURE") ? "prog_run_nofix.bin" : "prog_run.bin";
    Script& s = rs.sc;
    const bool m_assert = sel == p.id, m_pc1 = sel == p.id + "-PC1", m_pc2 = sel == p.id + "-PC2",
               m_pc3 = sel == p.id + "-PC3", m_pc4 = sel == p.id + "-PC4", m_pc5 = sel == p.id + "-PC5";
    poke_words(rs.pokes, kIdle, idle_list());
    for (auto& k : p.pokes) rs.pokes.push_back(k);
    prologue(s);
    if (m_pc3) s.waitb(DMACONR, 0x8000, 50);   // the contrast: a wait that ends by its timeout (DMACONR bit 15 is never set)
    if (p.dmacon_once) s.w16(DMACON, m_pc5 ? uint16_t(p.dmacon_once & ~p.dma_enable_bit) : p.dmacon_once);
    if (p.adkcon_once) s.w16(ADKCON, p.adkcon_once);
    if (p.extra_once) p.extra_once(s);
    if (p.dma_enable_bit) r.dma_idx = s.rd(DMACONR);   // DMA_CHECK
    if (m_pc5 && p.dmacon_once) s.w16(DMACON, p.dmacon_once);   // the contrast of PC5: the enable follows the DMA_CHECK, which has read it clear
    const size_t nvar = g_mutate ? std::min<size_t>(1, p.variants.size()) : p.variants.size();
    if (p.batch > 0) {
        // The cases run n at a time. Each case has its own list (WAIT, MOVE, final WAIT) at kCase + 16 i; the program writes COP1LC
        // and the strobe COPJMP1 for every case (AA p.20 'Coprocessor restart at first location'), so each MOVE starts the Copper
        // anew, whatever the Copper did after the MOVE of the case before. A frame B holds one batch: from line $30 on, the
        // cases one after the other, then COP1LC = IDLE.
        std::vector<VarPlan> plans;
        for (size_t vi = 0; vi < nvar; ++vi) {
            VarPlan vp; vp.v = p.variants[vi];
            vp.case_addr = kCase + 16 * uint32_t(vi);
            poke_words(rs.pokes, vp.case_addr, case_list(vp.v.reg, vp.v.v1));
            plans.push_back(vp);
        }
        for (size_t b0 = 0; b0 < nvar; b0 += p.batch) {
            const size_t b1 = std::min<size_t>(nvar, b0 + p.batch);
            s.vblsync();   // frame A: calibration, the Copper runs IDLE
            for (size_t vi = b0; vi < b1; ++vi) {
                VarPlan& vp = plans[vi]; const Variant& v = vp.v;
                if (v.v0) v.v0(s);
                if (!v.cal1_mark_before_v1 && v.v1cpu && !m_pc4) v.v1cpu(s);
                vp.c1.ms = s.mark();
                if (v.cal1_mark_before_v1 && v.v1cpu && !m_pc4) v.v1cpu(s);
                vp.c1.idx = v.obs(s);
                vp.c1.me = s.mark();
                if (v.after) v.after(s);
                if (v.v0) v.v0(s);
                vp.c0.ms = s.mark();
                vp.c0.idx = v.obs(s);
                vp.c0.me = s.mark();
                if (v.after) v.after(s);
            }
            for (uint16_t c : p.cs) {   // frame B, once for each COPCON value
                uint16_t cw = (m_assert && c == 0x0000) ? uint16_t(0x0002) : c;
                s.w32(COP1LCH, kIdle);
                s.vblsync();
                s.waitline(0x30);
                s.w16(COPCON, cw);
                for (size_t vi = b0; vi < b1; ++vi) {
                    VarPlan& vp = plans[vi]; const Variant& v = vp.v;
                    FrameB fb; fb.c = c;
                    if (v.v0) v.v0(s);
                    s.w32(COP1LCH, vp.case_addr);
                    fb.ms = s.mark();
                    if (!m_pc1) s.w16(COPJMP1, 0);
                    s.lines(2);
                    fb.mw = s.mark();
                    if (m_pc1) s.w16(COPJMP1, 0);   // the contrast of PC1: the strobe follows M_wait, so the Copper reads the MOVE after the window of PC1
                    if (m_pc2 && vi == b0) s.pollb(INTREQR, 0x0020, 30000);   // the contrast: the program waits for the next vertical blank
                    g_frame_b = true;
                    fb.idx = v.obs(s);
                    g_frame_b = false;
                    fb.me = s.mark();
                    if (v.after) v.after(s);
                    vp.b.push_back(fb);
                }
                s.w32(COP1LCH, kIdle);
                s.w16(COPCON, 0);
                const int f_vb = s.rd(INTREQR);
                for (size_t vi = b0; vi < b1; ++vi) plans[vi].b.back().f_vb = f_vb;
            }
        }
        r.plan = plans;
    }
    for (size_t vi = 0; vi < (p.batch > 0 ? 0 : nvar); ++vi) {
        VarPlan vp; vp.v = p.variants[vi];
        const Variant& v = vp.v;
        vp.case_addr = kCase + 0x40 * uint32_t(vi);
        poke_words(rs.pokes, vp.case_addr, v.caselist.empty() ? case_list(v.reg, v.v1) : v.caselist);
        // frame A: calibration, the Copper runs IDLE
        s.vblsync();
        if (v.cal) {
            v.cal(s, vp.c1, vp.c0);
        } else {
            if (v.v0) v.v0(s);
            if (!v.cal1_mark_before_v1 && v.v1cpu && !m_pc4) v.v1cpu(s);
            vp.c1.ms = s.mark();
            if (v.cal1_mark_before_v1 && v.v1cpu && !m_pc4) v.v1cpu(s);
            vp.c1.idx = v.obs(s);
            vp.c1.me = s.mark();
            if (v.after) v.after(s);
            if (v.v0) v.v0(s);
            vp.c0.ms = s.mark();
            vp.c0.idx = v.obs(s);
            vp.c0.me = s.mark();
            if (v.after) v.after(s);
        }
        // frame B, once for each COPCON value
        for (uint16_t c : p.cs) {
            FrameB fb; fb.c = c;
            uint16_t cw = (m_assert && c == 0x0000) ? uint16_t(0x0002) : c;
            s.w32(COP1LCH, vp.case_addr);
            if (m_pc1) s.w16(DMACON, 0x0080);
            s.vblsync();
            s.w16(COPCON, cw);
            if (v.v0) v.v0(s);
            fb.ms = s.mark();
            if (v.obs_replaces_step6) {
                g_frame_b = true;
                fb.idx = v.obs(s);
                g_frame_b = false;
                fb.mw = s.mark();
                if (m_pc2) s.pollb(INTREQR, 0x0020, 30000);
            } else {
                s.waitline(0x22);
                fb.mw = s.mark();
                if (m_pc2) s.pollb(INTREQR, 0x0020, 30000);
                g_frame_b = true;
                fb.idx = v.obs(s);
                g_frame_b = false;
            }
            s.w32(COP1LCH, kIdle);
            s.w16(COPCON, 0);
            fb.f_vb = s.rd(INTREQR);
            fb.me = s.mark();
            if (v.after) v.after(s);
            if (m_pc1) s.w16(DMACON, 0x8080);
            vp.b.push_back(fb);
        }
        r.plan.push_back(vp);
    }
    s.end();
    if (p.mouse_contrast && sel == p.id + "-" + p.pcx) {
        auto marks = r.plan.empty() ? nullptr : r.plan[0].v.inject_marks;
        auto done = std::make_shared<std::set<int>>();
        rs.drive = [marks, done](nmh::NanomigHarness& h, uint64_t, int last) {
            if (!marks) return;
            for (int m : *marks) {
                if (last < m || done->count(m)) continue;
                done->insert(m);   // one movement of 8 counts on the horizontal axis of port 0
                h.ports()->kbd_mouse_type = 0; h.ports()->kbd_mouse_data = 8;
                h.ports()->kbd_mouse_level = !h.ports()->kbd_mouse_level;
            }
        };
    }
    rs.cycles = 40'000'000;
    r.cap = run(rs);
    r.acc = accesses(r.cap);
    return r;
}

} // namespace s7
