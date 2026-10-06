// s7_f2.h -- F4, F5, F7, F9: pointer pairs of the audio and sprite DMA, the audio data register's output, the disk DMA
// (SPECIFICATION.md section 3).
#pragma once

#include "s7_f1.h"

namespace s7 {

// the address families S0-S9 (SPECIFICATION.md 1.8)
constexpr uint32_t kS[10] = {0x0EAB64, 0x11549A, 0x173388, 0x08CC76, 0x0C3C0E, 0x13C3F0, 0x103E30, 0x0FC1CE, 0x0BC000, 0x143FFE};

struct RefCheck { bool ok = false; };

// The refresh precondition (1.6) over one window.
inline bool refresh_pre(const std::vector<Acc>& acc, uint64_t K, uint64_t t0, uint64_t t1) {
    std::vector<Acc> rf;
    for (auto& a : acc) if (a.refresh && a.t > t0 && a.t < t1) rf.push_back(a);
    return refresh_groups(rf, t0, t1, K).ok;
}
inline uint64_t run_K(const std::vector<Acc>& acc) {
    std::vector<Acc> rf;
    for (auto& a : acc) if (a.refresh) rf.push_back(a);
    return colour_clock(rf);
}

// The reads of the window that are channel reads (regions) or stray, in time order.
inline std::vector<Acc> chan_or_stray(const std::vector<Acc>& acc, uint64_t t0, uint64_t t1, const std::vector<Region>& chan, std::vector<Acc>* stray = nullptr) {
    std::vector<Region> cop = {{kIdle, kIdle + 0x10}, {kCase, kCase + 0x10}};
    Window w = classify(acc, t0, t1, chan, cop);
    std::vector<Acc> all = w.chan;
    all.insert(all.end(), w.stray.begin(), w.stray.end());
    std::sort(all.begin(), all.end(), [](const Acc& a, const Acc& b) { return a.t < b.t; });
    if (stray) *stray = w.stray;
    return all;
}

// ---- F4: AUDxLCH/AUDxLCL: measured, not graded ------------------------------------------------
// The specification expects the first audio DMA read after the enable at the address the location registers hold.
// AA p.11 gives the registers as the 20-bit starting address of the channel's DMA data and says nothing of the
// first read; HRM p.164 says of the start-up 'Because of pipelining in Agnus, the first data word must be thrown
// away.' The address of the thrown-away read is then not a page value, so the proposition is not built as a
// graded line. The same stimulus runs and each case's first and second reads are measured.
inline void run_f4(Checks& ck) {
    FHead hd = f_head("F4", "", 0x8000);
    Script& s = hd.rs.sc;
    struct Case { int c; uint32_t a; uint16_t lch; uint16_t lcl; int ms, mg, me; };
    std::vector<Case> cases;
    for (int c = 0; c < 2; ++c) {
        for (int i = 0; i < 10; ++i) cases.push_back({c, kS[i], uint16_t(kS[i] >> 16), uint16_t(kS[i] & 0xFFFF), 0, 0, 0});
        cases.push_back({c, kS[0], uint16_t(0xFFE0 | (kS[0] >> 16)), uint16_t((kS[0] & 0xFFFF) | 1), 0, 0, 0});
    }
    std::vector<std::vector<Region>> regs;
    for (auto& cs : cases) {
        s.w16(audlen(cs.c), 4); s.w16(audper(cs.c), 400); s.w16(audvol(cs.c), 0);
        s.w32(audlc(cs.c), (uint32_t(cs.lch) << 16) | cs.lcl);
        cs.ms = s.mark();
        s.lines(2);
        cs.mg = s.mark();
        s.w16(DMACON, uint16_t(0x8200 | (1u << cs.c)));
        s.lines(20);
        cs.me = s.mark();
        s.w16(DMACON, uint16_t(1u << cs.c));
        s.lines(4);
        regs.push_back({{cs.a, cs.a + 8}});
    }
    s.end();
    hd.rs.cycles = 60'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    size_t first_at = 0, second_at = 0;
    for (size_t k = 0; k < cases.size(); ++k) {
        auto& cs = cases[k];
        uint64_t mg = marker(c, cs.mg), me = marker(c, cs.me);
        auto rd = chan_or_stray(acc, mg, me, regs[k]);
        bool f = !rd.empty() && rd[0].addr == cs.a, sc = rd.size() > 1 && rd[1].addr == cs.a;
        first_at += f; second_at += sc;
        char buf[200];
        std::snprintf(buf, sizeof buf, "F4 channel %d case %zu: location written $%06X, first read $%06X, second read $%06X", cs.c, k, cs.a,
                      rd.empty() ? 0u : rd[0].addr, rd.size() < 2 ? 0u : rd[1].addr);
        ck.note(buf);
    }
    ck.note("F4: " + dec((long long)first_at) + " of " + dec((long long)cases.size()) + " cases read the written location first, " + dec((long long)second_at) + " read it second");
    if (!g_mutate) ck.record("NOT BUILT F4: the specification expects the first audio read after the enable at the location registers' address; AA p.11 gives the registers as the starting address of the channel's DMA data and HRM p.164 says 'Because of pipelining in Agnus, the first data word must be thrown away', so the address of the first read is not a page value. Claims opus:AA-aud0lch.c5 sol:AA-aud0lch.c5 opus:AA-aud0lcl.c5 sol:AA-aud0lcl.c5 opus:AA-aud1lch.c5 sol:AA-aud1lch.c5 opus:AA-aud1lcl.c5 sol:AA-aud1lcl.c5 are not graded here; the first and second read of each case are measured above");
}

// ---- F5: SPRxPTH/SPRxPTL --------------------------------------------------------------------
inline void run_f5(Checks& ck, const std::string& sel) {
    const std::string id = "F5";
    FHead hd = f_head(id, sel, 0x8220);
    Script& s = hd.rs.sc;
    struct Case { int m0, m1, m2; };
    std::vector<Case> cases;
    for (int i = 0; i < 10; ++i) {
        Case cs{};
        s.vblsync();
        cs.m0 = s.mark();
        for (int x = 0; x < 8; ++x) {
            uint32_t a = kS[i] ^ (uint32_t(x) << 6);
            uint16_t lo = uint16_t(a & 0xFFFF);
            if (sel == id && i == 1 && x == 3) lo = uint16_t(lo ^ 0x0100);   // contrast: another address in SPR3PTL
            s.w32(uint16_t(SPR0PTH + 4 * x), (a & 0xFFFF0000u) | lo);
        }
        if (sel == id + "-QUIET" && i == 1) s.lines(60);
        cs.m1 = s.mark();
        if (sel == id + "-POP" && i == 1) s.lines(3); else if (sel == id + "-REF" && i == 1) s.lines(1); else s.vblsync();
        cs.m2 = s.mark();
        cases.push_back(cs);
    }
    s.end();
    hd.rs.cycles = 80'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    uint64_t K = run_K(acc);
    bool fixture = f_fixture(c), tmo = c.timeouts == 0;
    bool dma = (logv(c, hd.dma_idx) & 0x0220) == 0x0220;
    bool quiet = true, pop = true, ref = true, ok = true;
    for (int i = 0; i < 10; ++i) {
        uint64_t m0 = marker(c, cases[i].m0), m1 = marker(c, cases[i].m1), m2 = marker(c, cases[i].m2);
        std::vector<Region> regs;
        for (int x = 0; x < 8; ++x) { uint32_t a = kS[i] ^ (uint32_t(x) << 6); regs.push_back({a, a + 32}); }
        std::vector<Acc> stray;
        auto pre = chan_or_stray(acc, m0, m1, regs);
        auto run_ = chan_or_stray(acc, m1, m2, regs, &stray);
        quiet = quiet && pre.empty();
        pop = pop && run_.size() >= 8;
        ref = ref && refresh_pre(acc, K, m1, m2);
        bool cok = stray.empty();
        std::string miss;
        for (int x = 0; x < 8; ++x) {
            uint32_t a = kS[i] ^ (uint32_t(x) << 6);
            bool found = false;
            for (auto& r : run_) if (r.addr == a) found = true;
            if (!found) { cok = false; miss += " x=" + dec(x); }
        }
        ok = ok && cok;
        std::string sa;
        for (auto& a : stray) sa += " $" + hex6(a.addr) + "@" + dec((long long)(a.t - m1));
        ck.note("F5 family S" + dec(i) + ": " + dec((long long)run_.size()) + " sprite or stray reads, " + dec((long long)stray.size()) + " stray (at" + sa + " cycles after M1), no read at the pointer of" + (miss.empty() ? " none" : miss));
    }
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: DMACONR reads DMAEN and SPREN set (AA p.22)");
    ck.expect(quiet, id + "-QUIET: in every case no sprite read and no stray DMA read lies between M0 and M1");
    ck.expect(pop, id + "-POP: in every case at least eight sprite reads or stray DMA reads lie between M1 and M2");
    ck.expect(ref, id + "-REF: in every case the refresh-flagged reads between M1 and M2 come in groups of four, one group per line (AA p.29 '4 refresh time slots')");
    ck.expect(tmo, id + "-TMO: no wait of the program ended by its timeout");
    if (!(fixture && dma && quiet && pop && ref && tmo)) { not_graded(ck, id, "a precondition"); return; }
    std::string cl;
    for (int n = 0; n < 8; ++n) cl += " opus:AA-spr" + dec(n) + "pth.c5 opus:AA-spr" + dec(n) + "ptl.c5";
    for (int n = 0; n < 8; ++n) cl += " sol:AA-spr" + dec(n) + "pth.c5 sol:AA-spr" + dec(n) + "ptl.c5";
    ck.expect(ok, "F5: in a frame whose sprite pointers were written in the preceding vertical blank, sprite DMA reads from (SPRxPTH[4:0] << 16) | SPRxPTL[15:1] of each sprite x = 0-7 for each of the ten address families, and reads nothing outside the eight regions (AA p.8 row functions 'Sprite x pointer (High 5 bits)' '(Low 15 bits)'; AA p.6 '+' legend; AA p.28 'This pair of registers contains the 20 bit address of Sprite x (x=0,1,2,3,4,5,6,7) DMA data.') claims" + cl);
}

// ---- F7: AUDxDAT output -----------------------------------------------------------------------
struct Run7 { uint64_t t0, t1; int32_t v; };

inline std::vector<Run7> level_runs(const std::vector<int32_t>& sig, uint64_t base, uint64_t t0, uint64_t t1, uint64_t T) {
    std::vector<Run7> raw;
    for (uint64_t t = t0; t < t1; ++t) {
        if (t < base || t - base >= sig.size()) continue;
        int32_t v = sig[t - base];
        if (raw.empty() || raw.back().v != v) raw.push_back({t, t + 1, v}); else raw.back().t1 = t + 1;
    }
    std::vector<Run7> keep;
    for (auto& r : raw) if (r.t1 - r.t0 >= T / 4) keep.push_back(r);
    std::vector<Run7> out;
    for (auto& r : keep) {
        if (!out.empty() && out.back().v == r.v) out.back().t1 = r.t1; else out.push_back(r);
    }
    return out;
}

// twelve consecutive runs that hold (L1, L2, L3, L4) three times with L1 > L3 > L4 > L2
inline bool has_pattern(const std::vector<Run7>& r) {
    for (size_t i = 0; i + 12 <= r.size(); ++i) {
        bool ok = true;
        for (size_t k = 4; k < 12 && ok; ++k) ok = r[i + k].v == r[i + k - 4].v;
        int32_t l1 = r[i].v, l2 = r[i + 1].v, l3 = r[i + 2].v, l4 = r[i + 3].v;
        if (ok && l1 > l3 && l3 > l4 && l4 > l2) return true;
    }
    return false;
}

// F7 is measured, not graded: the specification's precondition 'no stray DMA read in the window' counts the
// thrown-away first read of the channel (HRM p.164 'Because of pipelining in Agnus, the first data word must be
// thrown away.') as a stray read, so it cannot hold for an implementation that follows the page and leaves the
// address of that read open. The stimulus runs as specified; the reads and the level runs are measured.
inline void run_f7(Checks& ck) {
    FHead hd = f_head("F7", "", 0x8000);
    Script& s = hd.rs.sc;
    hd.rs.want_audio = true;
    const uint32_t AD = 0x0D8000;
    poke_words(hd.rs.pokes, AD, {0x4040, 0xC0C0, 0x2020, 0x0000});
    struct Case { int ms, me; };
    std::vector<Case> cases;
    for (int c = 0; c < 4; ++c) {
        Case cs{};
        s.w32(audlc(c), AD); s.w16(audlen(c), 4); s.w16(audper(c), 400); s.w16(audvol(c), 0x0040);
        cs.ms = s.mark();
        s.w16(DMACON, uint16_t(0x8200 | (1u << c)));
        s.lines(100);
        cs.me = s.mark();
        s.w16(DMACON, uint16_t(1u << c)); s.w16(audvol(c), 0);
        cases.push_back(cs);
    }
    s.end();
    hd.rs.cycles = 30'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    std::vector<Region> chan = {{AD, AD + 8}};
    std::vector<int32_t> sl(c.audio.size()), sr(c.audio.size());
    auto sx = [](uint16_t v) { return int32_t(v & 0x7FFF) - ((v & 0x4000) ? 0x8000 : 0); };
    for (size_t i = 0; i < c.audio.size(); ++i) { sl[i] = sx(c.audio[i].left); sr[i] = sx(c.audio[i].right); }
    for (int ch = 0; ch < 4; ++ch) {
        uint64_t t0 = marker(c, cases[size_t(ch)].ms), t1 = marker(c, cases[size_t(ch)].me);
        std::vector<Acc> stray;
        auto rd = chan_or_stray(acc, t0, t1, chan, &stray);
        std::vector<uint64_t> tt;
        for (auto& r : rd) if (r.addr >= AD && r.addr < AD + 8) tt.push_back(r.t);
        uint64_t T = tt.size() > 1 ? (tt.back() - tt.front()) / (tt.size() - 1) : 0;
        auto rl = level_runs(sl, c.base, t0, t1, T), rr = level_runs(sr, c.base, t0, t1, T);
        bool pat = has_pattern(rl) || has_pattern(rr);
        std::string sa;
        for (auto& a : stray) sa += " $" + hex6(a.addr);
        ck.note("F7 channel " + dec(ch) + ": " + dec((long long)tt.size()) + " audio reads in [AD, AD+8), " + dec((long long)stray.size()) + " other reads (at" + sa +
                "); runs of the left output " + dec((long long)rl.size()) + ", of the right output " + dec((long long)rr.size()) + "; the level sequence (L1, L2, L3, L4) three times with L1 > L3 > L4 > L2: " + (pat ? "present" : "absent"));
    }
    if (!g_mutate) ck.record("NOT BUILT F7: the specification's precondition 'no stray DMA read in the window' counts the channel's thrown-away first read as a stray read, and HRM p.164 'Because of pipelining in Agnus, the first data word must be thrown away.' leaves the address of that read open, so the precondition cannot hold for every implementation that follows the pages. Claims opus:AA-aud0dat.c2 sol:AA-aud0dat.c2 opus:AA-aud1dat.c2 sol:AA-aud1dat.c2 opus:AA-aud2dat.c2 sol:AA-aud2dat.c2 opus:AA-aud3dat.c2 sol:AA-aud3dat.c2 are not graded here; the reads and the level runs are measured above");
}

// ---- F9: DSKDAT fed by the disk DMA ------------------------------------------------------------
inline void run_f9(Checks& ck, const std::string& sel) {
    const std::string id = "F9";
    FHead hd = f_head(id, sel, sel == id + "-DMA" ? 0x8200 : 0x8210);
    Script& s = hd.rs.sc;
    const uint32_t DB = 0x0D2000;
    poke_words(hd.rs.pokes, DB, {0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666});
    s.w16(ADKCON, 0x9100); s.w16(INTREQ, 0x0002);
    s.w32(DSKPTH, DB);
    int ms = s.mark();
    uint16_t len = sel == id ? 0xC003 : 0xC004;
    s.w16(DSKLEN, 0x4000);
    if (sel == id + "-DMAON") { s.w16(DSKLEN, len); } else { s.w16(DSKLEN, len); s.w16(DSKLEN, len); }
    int i_on = s.rd(DSKBYTR);
    int i_blk = -1, mb = 0, me = 0;
    if (sel == id + "-REF") {
        me = s.mark();
        s.w16(DSKLEN, 0x4000);
    } else {
        i_blk = s.pollb(INTREQR, 0x0002, 6000);
        mb = s.mark();
        s.w16(DSKLEN, 0x4000);
        s.lines(4);
        me = s.mark();
    }
    s.w16(INTREQ, 0x0002);
    s.end();
    hd.rs.floppy = 0x00;   // one drive, DF0, as the harness documents for floppy_config 0
    hd.rs.cycles = 30'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    uint64_t K = run_K(acc);
    uint64_t t0 = marker(c, ms), t1 = marker(c, me), tb = marker(c, mb);
    bool fixture = f_fixture(c), tmo = c.timeouts == 0;
    bool dma = (logv(c, hd.dma_idx) & 0x0210) == 0x0210;
    bool dmaon = (logv(c, i_on) & 0x4000) != 0;
    bool ref = refresh_pre(acc, K, t0, t1);
    std::vector<Region> chan = {{DB, DB + 12}};
    std::vector<Acc> stray;
    auto rd = chan_or_stray(acc, t0, t1, chan, &stray);
    bool reads_ok = rd.size() == 4 && stray.empty();
    for (size_t i = 0; reads_ok && i < 4; ++i) reads_ok = rd[i].addr == DB + 2 * uint32_t(i);
    bool blk = i_blk >= 0 && (logv(c, i_blk) & 0x0002) != 0 && rd.size() >= 4 && rd[3].t < tb;
    ck.note("F9: " + dec((long long)rd.size()) + " disk or stray reads, " + dec((long long)stray.size()) + " stray; DSKBYTR bit 14 " + dec(dmaon ? 1 : 0) + "; INTREQR bit 1 " +
            dec(i_blk >= 0 && (logv(c, i_blk) & 2) ? 1 : 0));
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: DMACONR reads DMAEN and DSKEN set (AA p.22)");
    ck.expect(dmaon, id + "-DMAON: DSKBYTR bit 14 DMAON reads 1 after the DSKLEN sequence (AA p.23 'DMAEN(DSKLEN) & DMAEN(DMACON) & DSKEN(DMACON)')");
    ck.expect(ref, id + "-REF: the refresh-flagged reads of the window come in groups of four, one group per line (AA p.29 '4 refresh time slots')");
    ck.expect(tmo, id + "-TMO: no wait of the program ended by its timeout");
    if (!(fixture && dma && dmaon && ref && tmo)) { not_graded(ck, id, "a precondition"); return; }
    ck.expect(reads_ok && blk, "F9: a disk write DMA of LENGTH = 4 words reads exactly four consecutive words from RAM starting at DSKPT, none of the guard words after them and no other word, and then raises the disk block request: INTREQR bit 1 is set after the fourth read (AA p.23 DSKLEN '13-0 LENGTH Length (# of words) of DMA data'; DSKDAT 'when the DMA data is finished (Length=0) it causes a Disk Block Interrupt.'; AA p.25 bit 1 DSKBLK 'Disk Block finished')");
}

} // namespace s7
