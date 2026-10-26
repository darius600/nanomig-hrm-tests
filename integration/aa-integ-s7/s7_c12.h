// s7_c12.h -- C12: BLTCDAT, BLTBDAT, BLTADAT with COPCON bit 1 clear (SPECIFICATION.md section 2, C12).
#pragma once

#include "s7_cprops.h"

namespace s7 {

struct C12Src { const char* name; uint16_t datreg, ptreg, use, lf; };
inline const C12Src* c12_sources() {
    static const C12Src s[3] = {{"BLTADAT", BLTADAT, BLTAPTH, 0x0800, 0xF0}, {"BLTBDAT", BLTBDAT, BLTBPTH, 0x0400, 0xCC}, {"BLTCDAT", BLTCDAT, BLTCPTH, 0x0200, 0xAA}};
    return s;
}

struct C12Blit { int ms = 0, me = 0; };
struct C12Frame { uint16_t c = 0; C12Blit pre, obs; int ms = 0, mw = 0, me = 0, f_vb = -1; };
struct C12Plan { C12Src src; C12Blit a_pre0, a_obs0, a_pre1, a_obs1; std::vector<C12Frame> b; };

struct C12Run { Cap cap; std::vector<C12Plan> plan; int dma_idx = -1; std::vector<Acc> acc; };

inline C12Run run_c12(const std::string& sel) {
    C12Run r;
    RunSpec rs;
    const std::string id = "C12";
    rs.bin = sel == id + "-FIXTURE" ? "prog_run_nofix.bin" : "prog_run.bin";
    Script& s = rs.sc;
    const bool m_assert = sel == id, m_pc1 = sel == id + "-PC1", m_pc2 = sel == id + "-PC2", m_pc3 = sel == id + "-PC3",
               m_pc4 = sel == id + "-PC4", m_pc5 = sel == id + "-PC5";
    poke_words(rs.pokes, kIdle, idle_list());
    poke_words(rs.pokes, kPx, {0x3C3C, 0x0000});
    prologue(s);
    if (m_pc3) s.waitb(DMACONR, 0x8000, 50);   // the contrast: a wait that ends by its timeout (DMACONR bit 15 is never set)
    s.w16(DMACON, m_pc5 ? uint16_t(0x8200) : uint16_t(0x8240));
    r.dma_idx = s.rd(DMACONR);
    if (m_pc5) s.w16(DMACON, 0x8240);   // the contrast of PC5: BLTEN is enabled after the DMA_CHECK has read it clear
    s.w16(BLTCON1, 0); set_masks(s);
    s.w16(BLTAMOD, 0); s.w16(BLTBMOD, 0); s.w16(BLTCMOD, 0); s.w16(BLTDMOD, 0);
    const int nsrc = g_mutate ? 1 : 3;   // a mutation run keeps the first source (BLTADAT)
    for (int xi = 0; xi < nsrc; ++xi) {
        C12Plan pl; pl.src = c12_sources()[xi];
        const C12Src& x = pl.src;
        poke_words(rs.pokes, kCase + 0x40 * uint32_t(xi + 1), {0x2001, 0xFFFE, x.datreg, 0x5A5A, 0xFFFF, 0xFFFE});
        const uint32_t casex = kCase + 0x40 * uint32_t(xi + 1);
        auto preload = [&](C12Blit& b) {
            s.w16(BLTCON0, uint16_t(x.use | 0x0100 | x.lf));
            s.w32(x.ptreg, kPx);
            s.w32(BLTDPTH, kDst3);
            b.ms = s.mark();
            s.w16(BLTSIZE, 0x0041);
            s.blitdone();
            b.me = s.mark();
        };
        auto observe = [&](C12Blit& b) {
            s.w16(BLTCON0, uint16_t(0x0100 | x.lf));
            s.w32(BLTDPTH, kDst);
            b.ms = s.mark();
            s.w16(BLTSIZE, 0x0042);
            s.blitdone();
            b.me = s.mark();
        };
        s.vblsync();
        // frame A: the DMA channel loads the register with V0, then with V1
        preload(pl.a_pre0); observe(pl.a_obs0);
        if (!m_pc4) s.wm(kPx, 0x5A5A);
        preload(pl.a_pre1); observe(pl.a_obs1);
        s.wm(kPx, 0x3C3C);
        for (uint16_t c : {uint16_t(0x0000), uint16_t(0xFFFD)}) {
            C12Frame fb; fb.c = c;
            s.w32(COP1LCH, casex);
            if (m_pc1) s.w16(DMACON, 0x0080);
            s.vblsync();
            s.w16(COPCON, c);
            if (m_assert && c == 0x0000) s.wm(kPx, 0x5A5A);   // the contrast: the DMA channel loads V1 as the register's value
            preload(fb.pre);
            if (m_assert && c == 0x0000) s.wm(kPx, 0x3C3C);
            fb.ms = s.mark();
            s.waitline(0x22);
            fb.mw = s.mark();
            if (m_pc2) s.pollb(INTREQR, 0x0020, 30000);
            observe(fb.obs);
            s.w32(COP1LCH, kIdle);
            s.w16(COPCON, 0);
            fb.f_vb = s.rd(INTREQR);
            fb.me = s.mark();
            if (m_pc1) s.w16(DMACON, 0x8080);
            pl.b.push_back(fb);
        }
        r.plan.push_back(pl);
    }
    s.end();
    rs.cycles = 60'000'000;
    r.cap = run(rs);
    r.acc = accesses(r.cap);
    return r;
}

// ---- evaluation -------------------------------------------------------------------
inline void eval_print_c12(Checks& ck, const C12Run& r, const std::string& claims_note) {
    const Cap& c = r.cap;
    const std::string id = "C12";
    bool fixture = c.loaded && c.released && c.fixture_ok && c.done;
    bool pc1 = true, pc2 = true, pc3 = c.timeouts == 0, pc4 = true, pc5 = (logv(c, r.dma_idx) & 0x0240) == 0x0240, assert_ok = true;
    Ctx cx{&c, &r.acc};
    // C12 runs its case lists with COPCON bit 1 clear only; no read of their final WAIT is observed, so only IDLE enters C12-FW.
    const Fw fw = fw_eval(c, r.acc, {{"IDLE", kIdle}});
    const std::vector<Region> px = {{kPx, kPx + 4}}, pxa = {{kPx, kPx + 2}};
    std::vector<Region> chan = {{kDst, kDst + 4}, {kDst3, kDst3 + 2}, {kPx, kPx + 4}};
    std::vector<std::pair<uint64_t, uint64_t>> wins;
    auto blit_ok = [&](const C12Blit& b, bool preload) {   // the reads of the PX words: one for a preload blit, none for an observation blit
        uint64_t t0 = marker(c, b.ms), t1 = marker(c, b.me);
        wins.push_back({t0, t1});
        size_t n_first = rd_count(cx, t0, t1, pxa), n_all = rd_count(cx, t0, t1, px);
        return preload ? (n_first == 1 && n_all == 1) : n_all == 0;
    };
    auto data = [&](const C12Blit& b) { return wr_obs(cx, marker(c, b.ms), marker(c, b.me), {{kDst, kDst + 4}}); };
    for (auto& pl : r.plan) {
        Obs u = wr_expect(kDst, {0x3C3C, 0x3C3C}), wdma = wr_expect(kDst, {0x5A5A, 0x5A5A});
        Obs o0 = data(pl.a_obs0), o1 = data(pl.a_obs1);
        // every blit is evaluated, so that its window enters PC5's list whatever the data say
        const bool b0 = blit_ok(pl.a_pre0, true), b1 = blit_ok(pl.a_obs0, false), b2 = blit_ok(pl.a_pre1, true), b3 = blit_ok(pl.a_obs1, false);
        pc4 = pc4 && o0 == u && o1 == wdma && b0 && b1 && b2 && b3;
        ck.note(id + " " + pl.src.name + " frame A: observation after the preload of V0 " + obs_str(o0, true) + ", after the preload of V1 " + obs_str(o1, true));
        for (auto& fb : pl.b) {
            uint64_t ms = marker(c, fb.ms), mw = marker(c, fb.mw), me = marker(c, fb.me);
            wins.push_back({marker(c, fb.pre.ms), marker(c, fb.pre.me)});
            wins.push_back({mw, me});
            size_t n_move = 0;
            std::vector<Region> cop = {{kIdle, kIdle + 0x10}, {kCase + 0x40, kCase + 0x50}, {kCase + 0x80, kCase + 0x90}, {kCase + 0xC0, kCase + 0xD0}};
            Window w = classify(r.acc, ms, mw, chan, cop);
            for (auto& a : w.copper) if (a.addr == kCase + 0x40 * uint32_t(&pl - &r.plan[0] + 1) + 4) ++n_move;
            pc1 = pc1 && n_move >= 1;
            pc2 = pc2 && (logv(c, fb.f_vb) & 0x20) == 0;
            const bool bp = blit_ok(fb.pre, true), bo = blit_ok(fb.obs, false);
            pc4 = pc4 && bp && bo;
            Obs o = data(C12Blit{fb.obs.ms, fb.obs.me});
            bool ok = o == u;
            assert_ok = assert_ok && ok;
            ck.note(id + " " + pl.src.name + " COPCON=" + cname(fb.c) + ": observation " + obs_str(o, true) + " expected " + obs_str(u, true) + (ok ? "" : " (differs)") +
                    "; Copper reads of the MOVE's first word: " + dec((long long)n_move));
        }
    }
    for (auto& a : r.acc) {
        if (a.refresh || is_cpu(a.addr) || (!a.write && !in_any(chan, a.addr)) || (a.write && a.addr == kPx)) continue;   // the program's own writes of V1 to PX are not blitter accesses
        bool inside = false;
        for (auto& w : wins) if (a.t > w.first && a.t < w.second) inside = true;
        if (!inside) pc5 = false;
    }
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(pc1, id + "-PC1: in every frame B a Copper read of the MOVE's first word lies between M_start and M_wait, so the Copper reached the MOVE (AA p.20 MOVE format)");
    ck.expect(pc2, id + "-PC2: in every frame B no vertical blank started during the case (INTREQR bit 5 is 0 at the end), so the case list ran once (AA p.21, p.25)");
    ck.expect(pc3, id + "-PC3: no wait of the program ended by its timeout");
    ck.expect(pc4, id + "-PC4: frame A: the DMA preload of V0 gives $3C3C twice and of V1 gives $5A5A twice in the observation blit; every preload blit reads [PX, PX+2) once and no other word of [PX, PX+4), every observation blit reads none of them");
    ck.expect(pc5, id + "-PC5: DMACONR reads DMAEN and BLTEN set, and no blitter access lies outside the windows of the blits, so every blit finished inside its window (AA p.22)");
    fw_print(ck, id, fixture, fw);
    if (!(fixture && pc1 && pc2 && pc3 && pc4 && pc5)) { not_graded(ck, id, "a precondition"); return; }
    ck.expect(assert_ok, id + ": with COPCON bit 1 clear (COPCON $0000 and $FFFD) a Copper MOVE of the one value V1 = $5A5A (graded for this value against the preloaded V0 = $3C3C, not for every 16-bit value) to BLTADAT, BLTBDAT or BLTCDAT does not change the data the blitter uses from that source: the observation blit writes $3C3C twice (AA p.6 '~=Address not writable by the Coprocessor unless COPCON bit 1 is set true.'; AA p.19 COPCON bit 01 CDANG; AA p.13 BLTxDAT) " + claims_note);
}

} // namespace s7
