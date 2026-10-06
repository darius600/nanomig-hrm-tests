// s7_f1.h -- F1, F2, F3, F8: the blitter row functions and the blitter's source DMA (SPECIFICATION.md section 3).
#pragma once

#include "s7_c12.h"

namespace s7 {

constexpr uint16_t kDmaBlt = 0x0240;   // DMACONR bits DMAEN and BLTEN

// The common head of an F run: prologue, the DMACON value of the proposition, DMA_CHECK, an optional wait that
// ends by its timeout (the contrast of the timeout precondition).
struct FHead { RunSpec rs; int dma_idx = -1; };

inline FHead f_head(const std::string& id, const std::string& sel, uint16_t dmacon, bool nodma_mut_off = false) {
    FHead h;
    h.rs.bin = sel == id + "-FIXTURE" ? "prog_run_nofix.bin" : "prog_run.bin";
    poke_words(h.rs.pokes, kIdle, idle_list());
    prologue(h.rs.sc);
    h.rs.sc.w16(DMACON, sel == id + "-DMA" && !nodma_mut_off ? uint16_t(dmacon & ~0x0040 & ~0x0010 & ~0x0020 & ~0x0001 & ~0x0002 & ~0x0004 & ~0x0008) : dmacon);
    if (sel == id + "-TMO") h.rs.sc.waitb(DMACONR, 0x8000, 50);
    h.dma_idx = h.rs.sc.rd(DMACONR);
    return h;
}

inline bool f_fixture(const Cap& c) { return c.loaded && c.released && c.fixture_ok && c.done; }

// every non-program write outside the windows [a, b]
inline bool writes_only_in(const std::vector<Acc>& acc, const std::vector<std::pair<uint64_t, uint64_t>>& wins) {
    for (auto& a : acc) {
        if (!a.write || is_cpu(a.addr)) continue;
        bool in = false;
        for (auto& w : wins) if (a.t > w.first && a.t < w.second) in = true;
        if (!in) return false;
    }
    return true;
}

inline std::string list_str(const Obs& o, bool wide = true) { return obs_str(o, wide); }

// ---- F1 and F2: BLTSIZE and BLTSIZV/BLTSIZH -------------------------------------------
struct SizeCase { unsigned w, h; unsigned ww, wh; bool v_written = true; unsigned expect_total = 0; };   // nominal and written size

inline void run_f1_f2(Checks& ck, bool f2, const std::string& sel) {
    const std::string id = f2 ? "F2" : "F1";
    FHead hd = f_head(id, sel, 0x8240);
    Script& s = hd.rs.sc;
    s.w16(BLTCON0, 0x01FF); s.w16(BLTCON1, 0); set_masks(s); s.w16(BLTDMOD, 0);
    std::vector<SizeCase> cases;
    if (!f2) {
        const unsigned lst[][2] = {{1, 1}, {2, 1}, {4, 1}, {8, 1}, {16, 1}, {32, 1}, {63, 1}, {1, 2}, {1, 4}, {1, 8}, {1, 16}, {1, 32}, {1, 64},
                                   {1, 128}, {1, 256}, {1, 512}, {1, 1023}, {5, 3}};
        for (auto& e : lst) cases.push_back({e[0], e[1], e[0], e[1]});
        if (sel == "F1") cases.back().ww = 6;   // contrast: (6,3) written, (5,3) expected
    } else {
        for (unsigned j = 0; j < 15; ++j) cases.push_back({1, 1u << j, 1, 1u << j});
        for (unsigned i = 0; i < 11; ++i) cases.push_back({1u << i, 1, 1u << i, 1});
        cases.push_back({1, 0x7FFF, 1, 0x7FFF});
        cases.push_back({0x7FF, 1, 0x7FF, 1});
        cases.push_back({2, 3, 2, 3});
        SizeCase reuse{5, 3, 5, 3}; reuse.v_written = false;
        cases.push_back(reuse);
        if (sel == "F2") cases[3].wh = 9;   // contrast: v = 9 written for the nominal v = 8
    }
    struct Ids { int ms, mg, me; };
    std::vector<Ids> ids;
    const uint32_t dst = kDst2;
    for (size_t i = 0; i < cases.size(); ++i) {
        auto& cs = cases[i];
        Ids d{};
        s.w32(BLTDPTH, dst);
        d.ms = s.mark();
        if (!f2) {
            s.lines(2);
            d.mg = s.mark();
            s.w16(BLTSIZE, uint16_t((cs.wh << 6) | cs.ww));
        } else {
            if (cs.v_written) s.w16(BLTSIZV, uint16_t(cs.wh));
            s.lines(2);
            d.mg = s.mark();
            s.w16(BLTSIZH, uint16_t(cs.ww));
        }
        s.blitdone();
        d.me = s.mark();
        ids.push_back(d);
    }
    if (sel == id + "-IDLE") { if (!f2) s.w16(BLTSIZE, 0x0041); else s.w16(BLTSIZH, 1); }   // contrast: a blit after the last case, not awaited
    s.end();
    hd.rs.cycles = 80'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    Ctx cx{&c, &acc};
    bool fixture = f_fixture(c);
    bool dma = (logv(c, hd.dma_idx) & kDmaBlt) == kDmaBlt;
    bool tmo = c.timeouts == 0;
    std::vector<std::pair<uint64_t, uint64_t>> wins;
    bool ok = true;
    size_t total = 0;
    for (size_t i = 0; i < cases.size(); ++i) {
        uint64_t ms = marker(c, ids[i].ms), mg = marker(c, ids[i].mg), me = marker(c, ids[i].me);
        wins.push_back({ms, me});
        Obs pre = wr_obs(cx, ms, mg, {});
        Obs w = wr_obs(cx, mg, me, {});
        size_t nom = size_t(cases[i].w) * cases[i].h;
        if (f2 && !cases[i].v_written) nom = size_t(cases[i - 1].h) * cases[i].w;   // the reuse case keeps the height of the case before
        bool cok = pre.empty() && w.size() == nom;
        for (size_t k = 0; cok && k < w.size(); ++k) cok = w[k] == ((uint64_t(dst + 2 * k) << 16) | 0xFFFF);
        ok = ok && cok;
        total += w.size();
        char buf[160];
        std::snprintf(buf, sizeof buf, "%s case %zu: nominal w=%u h=%u (written %u,%u): %zu writes before the start, %zu writes after, %zu expected%s",
                      id.c_str(), i, cases[i].w, cases[i].h, cases[i].ww, cases[i].wh, pre.size(), w.size(), nom, cok ? "" : " (differs)");
        ck.note(buf);
    }
    bool idle = writes_only_in(acc, wins);
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: DMACONR reads DMAEN and BLTEN set before the first case (AA p.22)");
    ck.expect(tmo, id + "-TMO: no wait of the program ended by its timeout");
    ck.expect(idle, id + "-IDLE: no blitter write lies outside the windows of the cases, so every blit finished inside its case");
    if (!(fixture && dma && tmo && idle)) { not_graded(ck, id, "fixture, DMA, TMO or IDLE"); return; }
    if (!f2)
        ck.expect(ok, "F1: a write to BLTSIZE starts the blitter, and the blit covers w words (bits 5-0) on each of h lines (bits 15-6): no blitter write before the write, exactly w*h writes after it at DST+2i with data $FFFF, in all 18 cases (AA p.14 'Writing to this register will start the Blitter', 'h=Height=Vertical lines', 'w=Width =Horiz pixels (6 bits=64 words=1024 pixels max)'; AA p.7 row function 'Blitter start and size (window width, height)') claims opus:AA-bltsize.c5 sol:AA-bltsize.c5");
    else
        ck.expect(ok, "F2: with the AA chip set selected, BLTSIZV holds a 15-bit height and keeps it for later blits, BLTSIZH holds an 11-bit width and a write to it starts a blit of w words on h lines, a write to BLTSIZV alone starts nothing: no blitter write before the BLTSIZH write, exactly v*w writes after it at DST+2i with data $FFFF, in every case, and 6 then 15 writes in the reuse cases (AA p.14 'BLTSIZH H 05E W A Blitter H size & start (11 bit width)', 'BLTSIZV H 05C W A Blitter V size (15 bit height)', 'BLTSIZV need not be rewritten for subsequent blits if the vertical size is the same'; AA p.7 mark h) claims opus:AA-bltsizv.c4 sol:AA-bltsizv.c4 opus:AA-bltsizh.c4 sol:AA-bltsizh.c4");
    (void)total;
}

// ---- F3: BLTCON0L ----------------------------------------------------------------------
inline void run_f3(Checks& ck, const std::string& sel) {
    const std::string id = "F3";
    FHead hd = f_head(id, sel, 0x8240);
    Script& s = hd.rs.sc;
    poke_words(hd.rs.pokes, kAsrc, {0x00FF, 0x0F0F});
    s.w16(BLTCON1, 0); set_masks(s); s.w16(BLTAMOD, 0); s.w16(BLTBMOD, 0); s.w16(BLTCMOD, 0); s.w16(BLTDMOD, 0);
    s.w32(BLTAPTH, kAsrc); s.w32(BLTBPTH, kBsrc); s.w32(BLTCPTH, kCsrc); s.w32(BLTDPTH, kDst);
    s.w16(BLTCON0, 0x09F0);
    if (sel == "F3") s.w16(BLTCON0, 0xFF0F); else s.w16(BLTCON0L, 0xFF0F);   // contrast: the full register written with the same word
    int ms = s.mark();
    s.w16(BLTSIZE, 0x0042);
    s.blitdone();
    int me = s.mark();
    if (sel == "F3-IDLE") s.w16(BLTSIZE, 0x0042);   // contrast: a blit after the window, not awaited
    s.end();
    hd.rs.cycles = 20'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    Ctx cx{&c, &acc};
    uint64_t t0 = marker(c, ms), t1 = marker(c, me);
    bool fixture = f_fixture(c), dma = (logv(c, hd.dma_idx) & kDmaBlt) == kDmaBlt, tmo = c.timeouts == 0;
    size_t ra = rd_count(cx, t0, t1, {{kAsrc, kAsrc + 4}}), rb = rd_count(cx, t0, t1, {{kBsrc, kBsrc + 4}}), rc = rd_count(cx, t0, t1, {{kCsrc, kCsrc + 4}});
    Obs w = wr_obs(cx, t0, t1, {});
    Obs want = wr_expect(kDst, {0xFF00, 0xF0F0});
    // the reads of A at ASRC and ASRC+2, in that order
    std::vector<uint32_t> ar;
    for (auto& a : acc) if (!a.write && !a.refresh && a.t > t0 && a.t < t1 && a.addr >= kAsrc && a.addr < kAsrc + 4) ar.push_back(a.addr);
    bool reads_ok = ar.size() == 2 && ar[0] == kAsrc && ar[1] == kAsrc + 2 && rb == 0 && rc == 0;
    bool wok = w == want;
    ck.note("F3: reads of ASRC " + dec((long long)ra) + ", of BSRC " + dec((long long)rb) + ", of CSRC " + dec((long long)rc) + "; writes " + list_str(w));
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: DMACONR reads DMAEN and BLTEN set (AA p.22)");
    ck.expect(tmo, id + "-TMO: no wait of the program ended by its timeout");
    std::vector<std::pair<uint64_t, uint64_t>> wins = {{t0, t1}};
    bool idle = writes_only_in(acc, wins);
    ck.expect(idle, id + "-IDLE: no blitter write lies outside the window of the blit");
    if (!(fixture && dma && tmo && idle)) { not_graded(ck, id, "fixture, DMA, TMO or IDLE"); return; }
    ck.expect(reads_ok && wok, "F3: with the AA chip set selected, a write of $FF0F to BLTCON0L replaces LF7-LF0 of BLTCON0 and leaves bits 15-8 unchanged: the blit after BLTCON0 = $09F0 reads A only (two reads, ASRC and ASRC+2, none in BSRC or CSRC) and writes $FF00 and $F0F0 at DST and DST+2 (AA p.13 'BLTCON0L H 05A W A Blitter control register 0 (write lower 8 bits only) This is to speed up software-- the upper bits are often the same.'; AA p.7 mark h; AA p.14 BLTCON0 bits) claims opus:AA-bltcon0l.c4 sol:AA-bltcon0l.c4");
}

// ---- F8: BLTxDAT loaded by the blitter's source DMA ---------------------------------------
inline void run_f8(Checks& ck, const std::string& sel) {
    const std::string id = "F8";
    FHead hd = f_head(id, sel, 0x8240);
    Script& s = hd.rs.sc;
    const uint16_t pat[4] = {0x8001, 0x4002, 0x2004, 0x1008};
    const uint16_t con0[3] = {0x09F0, 0x05CC, 0x03AA};
    const uint16_t ptr[3] = {BLTAPTH, BLTBPTH, BLTCPTH};
    uint32_t base[3][3];   // [x under test][source]
    for (int x = 0; x < 3; ++x)
        for (int k = 0; k < 3; ++k) {
            base[x][k] = 0x0B3000 + 0x100u * uint32_t(3 * x + k);
            std::vector<uint16_t> ws = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
            if (k == x) { ws.assign(pat, pat + 4); if (sel == id) ws[2] = 0x0000; }   // contrast: the third source word is $0000
            poke_words(hd.rs.pokes, base[x][k], ws);
        }
    s.w16(BLTCON1, 0); set_masks(s); s.w16(BLTAMOD, 0); s.w16(BLTBMOD, 0); s.w16(BLTCMOD, 0); s.w16(BLTDMOD, 0);
    int ms[3], me[3];
    for (int x = 0; x < 3; ++x) {
        uint16_t cw = con0[x];
        if (sel == id + "-READS") cw = uint16_t(cw & ~(0x0800 >> x));   // contrast: USEx cleared
        s.w16(BLTCON0, cw);
        for (int k = 0; k < 3; ++k) s.w32(ptr[k], base[x][k]);
        s.w32(BLTDPTH, kDst);
        ms[x] = s.mark();
        s.w16(BLTSIZE, 0x0044);
        s.blitdone();
        me[x] = s.mark();
    }
    if (sel == id + "-AFTER") s.w16(BLTSIZE, 0x0044);   // contrast: a second blit after M_end of the last case, not awaited
    s.end();
    hd.rs.cycles = 30'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    Ctx cx{&c, &acc};
    bool fixture = f_fixture(c), dma = (logv(c, hd.dma_idx) & kDmaBlt) == kDmaBlt, tmo = c.timeouts == 0;
    bool reads = true, ok = true, after = true;
    std::vector<std::pair<uint64_t, uint64_t>> wins;
    const char* nm[3] = {"A", "B", "C"};
    for (int x = 0; x < 3; ++x) {
        uint64_t t0 = marker(c, ms[x]), t1 = marker(c, me[x]);
        wins.push_back({t0, t1});
        size_t n = rd_count(cx, t0, t1, {{base[x][x], base[x][x] + 8}});
        reads = reads && n == 4;
        Obs w = wr_obs(cx, t0, t1, {});
        Obs want = wr_expect(kDst, {0x8001, 0x4002, 0x2004, 0x1008});
        // no other write to [DST, DST+8): every write of the window is one of the four
        bool wok = w == want;
        ok = ok && wok;
        ck.note(std::string("F8 source ") + nm[x] + ": " + dec((long long)n) + " blitter reads of SRC_" + nm[x] + "; writes " + list_str(w) + (wok ? "" : " (differs)"));
    }
    after = writes_only_in(acc, wins);
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: DMACONR reads DMAEN and BLTEN set (AA p.22)");
    ck.expect(tmo, id + "-TMO: no wait of the program ended by its timeout");
    ck.expect(reads, id + "-READS: each of the three blits made exactly four blitter reads in its source region SRC_x");
    ck.expect(after, id + "-AFTER: no blitter write lies outside the windows of the three blits, so no blitter access follows the end of a case");
    if (!(fixture && dma && tmo && reads && after)) { not_graded(ck, id, "a precondition"); return; }
    ck.expect(ok, "F8: with source x enabled the blitter loads BLTADAT, BLTBDAT or BLTCDAT from the source pointer's RAM words and computes D from them, and the destination words reach RAM as blitter writes: four writes at DST to DST+6 with data $8001, $4002, $2004, $1008 in that order and no other write, for each of A, B, C (AA p.13 'It is normally loaded by the Blitter DMA channel', 'The transfer is automatic during Blitter operation'; AA p.6 ER)");
}

} // namespace s7
