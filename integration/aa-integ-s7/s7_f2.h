// s7_f2.h -- F4, F5, F7, F9: pointer pairs of the audio and sprite DMA, the audio data register's output, the disk DMA
// (SPECIFICATION.md section 3).
#pragma once

#include "s7_f1.h"

namespace s7 {

// the address families S0-S9 (SPECIFICATION.md 1.8)
constexpr uint32_t kS[10] = {0x0EAB64, 0x11549A, 0x173388, 0x08CC76, 0x0C3C0E, 0x13C3F0, 0x103E30, 0x0FC1CE, 0x0BC000, 0x143FFE};

// The refresh precondition (1.6) over one window that holds a LINES(`lines`) wait.
inline bool refresh_pre(const std::vector<Acc>& acc, uint64_t t0, uint64_t t1, unsigned lines) {
    return refresh_reads(acc, t0, t1) >= refresh_min(lines);
}
constexpr unsigned kF4Lines = 20, kF5Lines = 40, kF7Lines = 100, kF9Lines = 4;   // the LINES waits inside the windows of F4, F5, F7, F9

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

// ---- F4: AUDxLCH/AUDxLCL ---------------------------------------------------------------------
// The pages leave the addresses of the channel's first two reads after the enable open: HRM p.164 'Because of
// pipelining in Agnus, the first data word must be thrown away.' places the address of the first read in no
// register, and HRM p.145 ('almost always starts the data output again from the top of the list') and p.148
// ('Copy to "back-up" location register ... Start retrieving audio data') order the first two reads differently.
// The graded form holds under both readings: a = (LCH[4:0] << 16) | (LCL & $FFFE) is the address of the channel's
// first or second read after the enable, and at most one read of the window lies outside [a, a+8), the first.

// The stray reads of a channel's window are acceptable when there is none, or one and it is the channel's first read.
inline bool first_read_only_stray(const std::vector<Acc>& rd, const std::vector<Acc>& stray) {
    if (stray.empty()) return true;
    return stray.size() == 1 && !rd.empty() && rd[0].t == stray[0].t && rd[0].addr == stray[0].addr;
}

inline void run_f4(Checks& ck, const std::string& sel) {
    const std::string id = "F4";
    FHead hd = f_head(id, sel, 0x8000);
    Script& s = hd.rs.sc;
    struct Case { int c; uint32_t a; uint16_t lch; uint16_t lcl; int ms = 0, mg = 0, me = 0, i_dma = -1; };
    std::vector<Case> cases;
    for (int c = 0; c < 2; ++c) {
        for (int i = 0; i < 10; ++i) cases.push_back({c, kS[i], uint16_t(kS[i] >> 16), uint16_t(kS[i] & 0xFFFF)});
        cases.push_back({c, kS[0], uint16_t(0xFFE0 | (kS[0] >> 16)), uint16_t((kS[0] & 0xFFFF) | 1)});
    }
    std::vector<std::vector<Region>> regs;
    for (size_t k = 0; k < cases.size(); ++k) {
        auto& cs = cases[k];
        const bool contrast = k == (sel == id + "-ONLY" ? 12u : 1u);   // the second case of channel 0 (of channel 1 for ONLY) carries the contrast of a precondition selector
        s.w16(audlen(cs.c), 4); s.w16(audper(cs.c), 400); s.w16(audvol(cs.c), 0);
        uint16_t lcl = cs.lcl;
        if (sel == id && contrast) lcl = uint16_t(lcl ^ 0x0010);   // contrast: A4 flipped in the written address
        s.w32(audlc(cs.c), (uint32_t(cs.lch) << 16) | lcl);
        uint16_t en = uint16_t(0x8200 | (1u << cs.c));
        if (sel == id + "-ONLY" && contrast) en = uint16_t(en | (1u << (cs.c ^ 1)));           // contrast: the other channel of the pair is enabled too
        if (sel == id + "-QUIET" && contrast) s.w16(DMACON, en);                               // contrast: the enable is written before M_start
        cs.ms = s.mark();
        s.lines(2);
        cs.mg = s.mark();
        if (sel == id + "-POP" && contrast) s.lines(kF4Lines);                                  // contrast: the enable follows the window's 20 lines, so fewer than two reads fall in it
        if (sel == id + "-DMA" && contrast) cs.i_dma = s.rd(DMACONR);                           // contrast: DMA_CHECK is read before the channel's enable is written
        if (!(sel == id + "-QUIET" && contrast)) s.w16(DMACON, en);
        if (!(sel == id + "-DMA" && contrast)) cs.i_dma = s.rd(DMACONR);
        if (!(sel == id + "-POP" && contrast)) s.lines(sel == id + "-REF" && contrast ? 8 : kF4Lines);   // contrast of REF: a window of 8 lines in place of 20
        cs.me = s.mark();
        s.w16(DMACON, uint16_t(en & 0x000F));
        s.lines(4);
        regs.push_back({{cs.a, cs.a + 8}});
    }
    s.end();
    hd.rs.cycles = 60'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    bool fixture = f_fixture(c), tmo = c.timeouts == 0;
    bool dma = true, only = true, quiet = true, pop = true, ref = true, ok = true;
    size_t first_at = 0, second_at = 0;
    for (size_t k = 0; k < cases.size(); ++k) {
        auto& cs = cases[k];
        uint64_t ms = marker(c, cs.ms), mg = marker(c, cs.mg), me = marker(c, cs.me);
        const uint16_t dm = logv(c, cs.i_dma);
        dma = dma && (dm & (0x0200 | (1u << cs.c))) == (0x0200u | (1u << cs.c));
        only = only && (dm & 0x000F & ~(1u << cs.c)) == 0;
        quiet = quiet && chan_or_stray(acc, ms, mg, regs[k]).empty();
        std::vector<Acc> stray;
        auto rd = chan_or_stray(acc, mg, me, regs[k], &stray);
        pop = pop && rd.size() >= 2;
        const size_t nref = refresh_reads(acc, mg, me);
        ref = ref && refresh_pre(acc, mg, me, kF4Lines);
        bool first_or_second = rd.size() >= 2 && (rd[0].addr == cs.a || rd[1].addr == cs.a);
        ok = ok && first_or_second && first_read_only_stray(rd, stray);
        first_at += !rd.empty() && rd[0].addr == cs.a;
        second_at += rd.size() > 1 && rd[1].addr == cs.a;
        char buf[400];
        std::snprintf(buf, sizeof buf, "F4 channel %d case %zu: location written $%06X, first read $%06X, second read $%06X, %zu reads outside [a, a+8), %zu refresh reads in the window (at least %zu required)", cs.c, k, cs.a,
                      rd.empty() ? 0u : rd[0].addr, rd.size() < 2 ? 0u : rd[1].addr, stray.size(), nref, refresh_min(kF4Lines));
        ck.note(buf);
    }
    ck.note("F4: " + dec((long long)first_at) + " of " + dec((long long)cases.size()) + " cases read the written location first, " + dec((long long)second_at) + " read it second");
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: in every case DMACONR reads DMAEN and the enable bit of the channel set (AA p.22)");
    ck.expect(only, id + "-ONLY: in every case DMACONR reads no enable bit of another audio channel set, so only channel c runs (AA p.22)");
    ck.expect(quiet, id + "-QUIET: in every case no audio read and no stray DMA read lies between M_start and M_go");
    ck.expect(pop, id + "-POP: in every case at least two audio reads or stray DMA reads lie between M_go and M_end");
    ck.expect(ref, id + "-REF: in every case the window between M_go and M_end holds at least 76 reads at whose cycle the refresh output is high, the four refresh reads of each of at least 19 whole lines (AA p.29 '4 refresh time slots'; the window holds the 20 line starts of its LINES(20) wait); that the four reads of a line form one group, and that no line holds more than four, is unfalsifiable at this boundary");
    ck.expect(tmo, id + "-TMO: no wait of the program ended by its timeout");
    if (!(fixture && dma && only && quiet && pop && ref && tmo)) { not_graded(ck, id, "a precondition"); return; }
    ck.expect(ok, "F4: with a = (AUDcLCH[4:0] << 16) | (AUDcLCL & $FFFE), the channel's first or second DMA read after the enable is at a for each of the ten addresses S0-S9 and for the case with the unused bits set, and at most one read of the window lies outside [a, a+8), the first (AA p.7 row functions 'Audio channel 0 location (High 5 bits)' '(Low 15 bits)'; AA p.6 '+=Address register pair.Low word uses DB1-DB15,High word DB0-DB4.', 'LCL,LCH=20 bit Location (starting address) of DMA data'; AA p.11 'This pair of registers contains the 20 bit starting address (location) of Audio channel x (x=0,1,2,3) DMA data.'; HRM p.164 'Because of pipelining in Agnus, the first data word must be thrown away.') claims opus:AA-aud0lch.c5 sol:AA-aud0lch.c5 opus:AA-aud0lcl.c5 sol:AA-aud0lcl.c5 opus:AA-aud1lch.c5 sol:AA-aud1lch.c5 opus:AA-aud1lcl.c5 sol:AA-aud1lcl.c5");
}

// ---- F5: SPRxPTH/SPRxPTL --------------------------------------------------------------------
inline void run_f5(Checks& ck, const std::string& sel) {
    const std::string id = "F5";
    FHead hd = f_head(id, sel, 0x8220);
    Script& s = hd.rs.sc;
    struct Case { int m0, m1, m2; };
    std::vector<Case> cases;
    const int nfam = g_mutate ? 2 : 10;   // a mutation run keeps the first two families
    for (int i = 0; i < nfam; ++i) {
        Case cs{};
        s.vblsync();
        if (sel == id + "-POP" && i == 1) s.lines(30);   // contrast: the pointers are written after the sprite DMA of the frame, so the sprite DMA lies in neither window
        cs.m0 = s.mark();
        for (int x = 0; x < 8; ++x) {
            uint32_t a = kS[i] ^ (uint32_t(x) << 6);
            uint16_t lo = uint16_t(a & 0xFFFF);
            if (sel == id && i == 1 && x == 3) lo = uint16_t(lo ^ 0x0100);   // contrast: another address in SPR3PTL
            if (sel == id + "-STRAY" && i == 1 && x == 3) lo = uint16_t(lo ^ 0x0400);   // contrast: A10 flipped, outside A8-A6, so the read lies outside all eight regions
            s.w32(uint16_t(SPR0PTH + 4 * x), (a & 0xFFFF0000u) | lo);
        }
        if (sel == id + "-QUIET" && i == 1) s.lines(60);   // contrast: the sprite DMA of the frame lies between M0 and M1
        cs.m1 = s.mark();
        if (sel == id + "-REF" && i == 1) s.lines(30);                                        // contrast: a window of 30 lines in place of 40 lines and the vertical blank
        else if (sel == id + "-QUIET" && i == 1) { s.vblsync(); s.lines(30); }              // contrast: the window extends over the sprite DMA of the next frame
        else { s.lines(kF5Lines); s.vblsync(); }
        cs.m2 = s.mark();
        cases.push_back(cs);
    }
    s.end();
    hd.rs.cycles = 80'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    bool fixture = f_fixture(c), tmo = c.timeouts == 0;
    bool dma = (logv(c, hd.dma_idx) & 0x0220) == 0x0220;
    bool quiet = true, pop = true, ref = true, ok = true;
    for (int i = 0; i < nfam; ++i) {
        uint64_t m0 = marker(c, cases[i].m0), m1 = marker(c, cases[i].m1), m2 = marker(c, cases[i].m2);
        std::vector<Region> regs;
        for (int x = 0; x < 8; ++x) { uint32_t a = kS[i] ^ (uint32_t(x) << 6); regs.push_back({a, a + 32}); }
        std::vector<Acc> stray;
        auto pre = chan_or_stray(acc, m0, m1, regs);
        auto run_ = chan_or_stray(acc, m1, m2, regs, &stray);
        quiet = quiet && pre.empty();
        pop = pop && run_.size() >= 8;
        ref = ref && refresh_pre(acc, m1, m2, kF5Lines);
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
        ck.note("F5 family S" + dec(i) + ": " + dec((long long)run_.size()) + " sprite or stray reads, " + dec((long long)stray.size()) + " stray (at" + sa + " cycles after M1), no read at the pointer of" + (miss.empty() ? " none" : miss) +
                "; " + dec((long long)refresh_reads(acc, m1, m2)) + " refresh reads in the window (at least " + dec((long long)refresh_min(kF5Lines)) + " required)");
    }
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: DMACONR reads DMAEN and SPREN set (AA p.22)");
    ck.expect(quiet, id + "-QUIET: in every case no sprite read and no stray DMA read lies between M0 and M1");
    ck.expect(pop, id + "-POP: in every case at least eight sprite reads or stray DMA reads lie between M1 and M2");
    ck.expect(ref, id + "-REF: in every case the window between M1 and M2 holds at least 156 reads at whose cycle the refresh output is high, the four refresh reads of each of at least 39 whole lines (AA p.29 '4 refresh time slots'; the window holds the 40 line starts of its LINES(40) wait); that the four reads of a line form one group, and that no line holds more than four, is unfalsifiable at this boundary");
    ck.expect(tmo, id + "-TMO: no wait of the program ended by its timeout");
    fw_print(ck, id, fixture, fw_eval(c, acc, {{"IDLE", kIdle}}));
    if (!(fixture && dma && quiet && pop && ref && tmo)) { not_graded(ck, id, "a precondition"); return; }
    std::string cl;
    for (int n = 0; n < 8; ++n) cl += " opus:AA-spr" + dec(n) + "pth.c5 opus:AA-spr" + dec(n) + "ptl.c5";
    for (int n = 0; n < 8; ++n) cl += " sol:AA-spr" + dec(n) + "pth.c5 sol:AA-spr" + dec(n) + "ptl.c5";
    ck.expect(ok, "F5: in a frame whose sprite pointers were written in the preceding vertical blank, sprite DMA reads from (SPRxPTH[4:0] << 16) | SPRxPTL[15:1] of each sprite x = 0-7 for each of the ten address families, and reads nothing outside the eight regions (AA p.8 row functions 'Sprite x pointer (High 5 bits)' '(Low 15 bits)'; AA p.6 '+' legend; AA p.28 'This pair of registers contains the 20 bit address of Sprite x (x=0,1,2,3,4,5,6,7) DMA data.') claims" + cl);
}

// ---- F7: AUDxDAT output -----------------------------------------------------------------------
struct Run7 { uint64_t t0, t1; int32_t v; };

// The runs of one output signal in (t0, t1): maximal stretches of equal samples, every one of them (no run is dropped or merged).
inline std::vector<Run7> level_runs(const std::vector<int32_t>& sig, uint64_t base, uint64_t t0, uint64_t t1) {
    std::vector<Run7> raw;
    for (uint64_t t = t0; t < t1; ++t) {
        if (t < base || t - base >= sig.size()) continue;
        int32_t v = sig[t - base];
        if (raw.empty() || raw.back().v != v) raw.push_back({t, t + 1, v}); else raw.back().t1 = t + 1;
    }
    return raw;
}

// The eight bytes of AD..AD+7 in memory order (first byte of a word = bits 15-8), read as two's-complement integers.
// Their order differs from the order of the same bytes read as unsigned, as sign-magnitude or as one's complement
// (sign-magnitude: $FF = -127 < $E0 = -96 < $C0 = -64 < $80 = -0; two's complement: $80 = -128 < $C0 < $E0 < $FF = -1).
constexpr int kF7Bytes[8] = {0x7F, 0x01, 0xFF, 0x80, 0x40, 0xC0, 0x20, 0xE0};
inline int s8(int b) { return b >= 0x80 ? b - 256 : b; }

// True when the eight levels l[0..7] stand in the order of the bytes e[0..7]: every pair is ordered the same way.
inline bool same_order(const int32_t* l, const int* e) {
    for (int p = 0; p < 8; ++p)
        for (int q = p + 1; q < 8; ++q) {
            int a = l[p] < l[q] ? -1 : l[p] > l[q] ? 1 : 0, b = e[p] < e[q] ? -1 : e[p] > e[q] ? 1 : 0;
            if (a != b) return false;
        }
    return true;
}

// Twenty-four consecutive runs that hold the eight levels of the eight bytes three times in succession, the levels in the
// order of the two's-complement values of the bytes. The page leaves open which byte of a word is output first, so the
// order of the bytes within every word is either the memory order or the reverse (the same in all words).
inline bool has_pattern(const std::vector<Run7>& r) {
    int ea[8], eb[8];
    for (int w = 0; w < 4; ++w) {
        ea[2 * w] = s8(kF7Bytes[2 * w]);     ea[2 * w + 1] = s8(kF7Bytes[2 * w + 1]);
        eb[2 * w] = s8(kF7Bytes[2 * w + 1]); eb[2 * w + 1] = s8(kF7Bytes[2 * w]);
    }
    for (size_t i = 0; i + 24 <= r.size(); ++i) {
        bool rep = true;
        for (size_t k = 8; k < 24 && rep; ++k) rep = r[i + k].v == r[i + k - 8].v;
        if (!rep) continue;
        int32_t l[8];
        for (int k = 0; k < 8; ++k) l[k] = r[i + size_t(k)].v;
        if (same_order(l, ea) || same_order(l, eb)) return true;
    }
    return false;
}

// F7: the channel's audio DMA writes AUDxDAT with the RAM words, which reach the output as signed bytes. Its
// precondition on the channel's reads allows at most one read outside [AD, AD+8), the channel's first after the
// enable (the pages leave the address of the first read open; see F4).
inline void run_f7(Checks& ck, const std::string& sel) {
    const std::string id = "F7";
    FHead hd = f_head(id, sel, 0x8000);
    Script& s = hd.rs.sc;
    hd.rs.want_audio = true;
    const uint32_t AD = 0x0D8000;
    // the bytes $7F $01 $FF $80 $40 $C0 $20 $E0 (kF7Bytes); contrast: the byte $C0 (-64) is $60 (+96)
    poke_words(hd.rs.pokes, AD, {0x7F01, 0xFF80, uint16_t(sel == id ? 0x4060 : 0x40C0), 0x20E0});
    struct Case { int ms = 0, me = 0, i_dma = -1, i_adk = -1; };
    std::vector<Case> cases;
    for (int c = 0; c < 4; ++c) {
        Case cs{};
        const bool first = c == 0;   // the first case carries the contrast of a precondition selector
        s.w32(audlc(c), AD); s.w16(audlen(c), 4); s.w16(audper(c), 400); s.w16(audvol(c), 0x0040);
        if (sel == id + "-ADK" && first) s.w16(ADKCON, 0x8001);   // contrast: USE0V1 is set
        cs.i_adk = s.rd(ADKCONR);
        if (sel == id + "-ADK" && first) s.w16(ADKCON, 0x00FF);
        cs.ms = s.mark();
        uint16_t en = uint16_t(0x8200 | (1u << c));
        if (sel == id + "-POP" && first) s.lines(kF7Lines - 5);   // contrast: the enable follows 95 lines of the window of 100, so fewer than eight reads fall in it
        if (sel == id + "-DMA" && first) cs.i_dma = s.rd(DMACONR);   // contrast: DMA_CHECK is read before the channel's enable is written
        s.w16(DMACON, en);
        if (!(sel == id + "-DMA" && first)) cs.i_dma = s.rd(DMACONR);
        if (sel == id + "-POP" && first) s.lines(5);
        else if (sel == id + "-REF" && first) s.lines(40);       // contrast: a window of 40 lines in place of 100
        else if (sel == id + "-STRAY" && first) { s.lines(60); s.w16(uint16_t(audlc(c) + 2), 0x8100); s.lines(40); }   // contrast: the location is rewritten to AD+$100, so the next restart reads outside
        else s.lines(kF7Lines);
        cs.me = s.mark();
        s.w16(DMACON, uint16_t(1u << c)); s.w16(audvol(c), 0);
        cases.push_back(cs);
    }
    s.end();
    hd.rs.cycles = 30'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    bool fixture = f_fixture(c), tmo = c.timeouts == 0;
    bool dma = true, adk = true, pop = true, stray_ok = true, ref = true, ok = true;
    std::vector<Region> chan = {{AD, AD + 8}};
    std::vector<int32_t> sl(c.audio.size()), sr(c.audio.size());
    auto sx = [](uint16_t v) { return int32_t(v & 0x7FFF) - ((v & 0x4000) ? 0x8000 : 0); };
    for (size_t i = 0; i < c.audio.size(); ++i) { sl[i] = sx(c.audio[i].left); sr[i] = sx(c.audio[i].right); }
    for (int ch = 0; ch < 4; ++ch) {
        uint64_t t0 = marker(c, cases[size_t(ch)].ms), t1 = marker(c, cases[size_t(ch)].me);
        const uint16_t dm = logv(c, cases[size_t(ch)].i_dma);
        dma = dma && (dm & (0x0200 | (1u << ch))) == (0x0200u | (1u << ch));
        adk = adk && (logv(c, cases[size_t(ch)].i_adk) & 0x00FF) == 0;
        std::vector<Acc> stray;
        auto rd = chan_or_stray(acc, t0, t1, chan, &stray);
        std::vector<uint64_t> tt;
        size_t cnt[4] = {0, 0, 0, 0};
        for (auto& r : rd) if (r.addr >= AD && r.addr < AD + 8) { tt.push_back(r.t); ++cnt[(r.addr - AD) / 2]; }
        const bool got = tt.size() >= 8 && cnt[0] >= 2 && cnt[1] >= 2 && cnt[2] >= 2 && cnt[3] >= 2;   // the channel fetched the words
        pop = pop && got;
        stray_ok = stray_ok && first_read_only_stray(rd, stray);
        ref = ref && refresh_pre(acc, t0, t1, kF7Lines);
        auto rl = level_runs(sl, c.base, t0, t1), rr = level_runs(sr, c.base, t0, t1);
        bool pat = has_pattern(rl) || has_pattern(rr);
        ok = ok && pat && got;
        std::string sa;
        for (auto& a : stray) sa += " $" + hex6(a.addr) + (a.t == (rd.empty() ? 0 : rd[0].t) ? " (the first read)" : "");
        ck.note("F7 channel " + dec(ch) + ": " + dec((long long)tt.size()) + " audio reads in [AD, AD+8), " + dec((long long)stray.size()) + " other reads (at" + sa +
                "); " + dec((long long)refresh_reads(acc, t0, t1)) + " refresh reads in the window (at least " + dec((long long)refresh_min(kF7Lines)) + " required); runs of the left output " + dec((long long)rl.size()) + ", of the right output " + dec((long long)rr.size()) + "; the eight levels of the eight bytes three times in succession, in the order of the two's-complement values: " + (pat ? "present" : "absent"));
    }
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: in every case DMACONR reads DMAEN and the enable bit of the channel set (AA p.22)");
    ck.expect(adk, id + "-ADK: ADKCONR & $00FF reads 0 before each case, so no modulation bit is set (AA p.11 USExVy / USExPy clear)");
    ck.expect(stray_ok, id + "-STRAY: in every case at most one read of the window lies outside [AD, AD+8), and it is the channel's first read after the enable");
    ck.expect(ref, id + "-REF: in every case the window holds at least 396 reads at whose cycle the refresh output is high, the four refresh reads of each of at least 99 whole lines (AA p.29 '4 refresh time slots'; the window holds the 100 line starts of its LINES waits); that the four reads of a line form one group, and that no line holds more than four, is unfalsifiable at this boundary");
    ck.expect(tmo, id + "-TMO: no wait of the program ended by its timeout");
    (void)pop;
    if (!(fixture && dma && adk && stray_ok && ref && tmo)) { not_graded(ck, id, "a precondition"); return; }
    ck.expect(ok, "F7: with channel c's audio DMA on (c = 0-3), the channel reads the words from RAM (in the window at least eight reads of [AD, AD+8), each of its four words at least twice) and writes them into AUDcDAT, and the two bytes of every word are output as two's-complement bytes, one after the other: on at least one output signal the runs, in time order, contain twenty-four consecutive runs that hold the eight levels of the bytes $7F, $01, $FF, $80, $40, $C0, $20, $E0 three times in succession, the levels in the order of the two's-complement values of the bytes ($80 lowest, $C0, $E0, $FF, $01, $20, $40, $7F highest), the two bytes of every word in either order (AA p.11 'This register is the Audio channel x (x=0,1,2,3) DMA data buffer. It contains 2 bytes of data (each byte is a twos complement signed integer) that are outputed sequentially (with digital to analog conversion) to the audio output pins.', 'The audio DMA channel controller automatically transfers data to this register from RAM.'; AA p.7, p.8 access W) claims opus:AA-aud0dat.c2 sol:AA-aud0dat.c2 opus:AA-aud1dat.c2 sol:AA-aud1dat.c2 opus:AA-aud2dat.c2 sol:AA-aud2dat.c2 opus:AA-aud3dat.c2 sol:AA-aud3dat.c2");
}

// ---- F9: DSKDAT fed by the disk DMA ------------------------------------------------------------
inline void run_f9(Checks& ck, const std::string& sel) {
    const std::string id = "F9";
    FHead hd = f_head(id, sel, 0x8210);
    Script& s = hd.rs.sc;
    // two cases: LENGTH = 4 at DB[0] and LENGTH = 3 at DB[1]; every region holds LENGTH data words and then guard words, 12 bytes in all
    const uint32_t DB[2] = {0x0D2000, 0x0D3000};
    const unsigned LEN[2] = {4, 3};
    poke_words(hd.rs.pokes, DB[0], {0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666});
    poke_words(hd.rs.pokes, DB[1], {0x7111, 0x7222, 0x7333, 0x7444, 0x7555, 0x7666});
    if (sel == id + "-FW") poke_words(hd.rs.pokes, kIdle, {0x0001, 0xFFFE, 0xFFFF, 0xFFFE});   // contrast: the list starts with a WAIT that ends at once, so the word at IDLE+4 is an instruction word of the list
    s.w16(ADKCON, 0x9100);
    struct Case { int ms = 0, me = 0, mb = 0, i_on = -1, i_blk = -1; };
    Case cs[2];
    for (int j = 0; j < 2; ++j) {
        const bool first = j == 0;   // the first case carries the contrast of a selector
        s.w16(INTREQ, 0x0002);
        s.w32(DSKPTH, (sel == id + "-STRAY" && first) ? DB[j] + 0x1000 : DB[j]);   // contrast: DSKPT outside [DB, DB+12)
        cs[j].ms = s.mark();
        uint16_t len = uint16_t(0xC000 | ((sel == id && first) ? 3 : LEN[j]));   // contrast: LENGTH = 3 in the case that expects 4
        s.w16(DSKLEN, 0x4000);
        if (sel == id + "-DMAON" && first) { s.w16(DSKLEN, len); } else { s.w16(DSKLEN, len); s.w16(DSKLEN, len); }
        cs[j].i_on = s.rd(DSKBYTR);
        if (sel == id + "-REF" && first) cs[j].me = s.mark();   // the contrast: M_end follows the DSKBYTR read, so the window holds fewer than 12 refresh reads
        cs[j].i_blk = s.pollb(INTREQR, 0x0002, 220);
        cs[j].mb = s.mark();
        s.w16(DSKLEN, 0x4000);
        s.lines(kF9Lines);
        if (!(sel == id + "-REF" && first)) cs[j].me = s.mark();
    }
    s.w16(INTREQ, 0x0002);
    s.end();
    hd.rs.floppy = 0x00;   // one drive, DF0, as the harness documents for floppy_config 0
    hd.rs.cycles = 40'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    bool fixture = f_fixture(c), tmo = c.timeouts == 0;
    bool dma = (logv(c, hd.dma_idx) & 0x0210) == 0x0210;
    bool dmaon = true, ref = true, ok = true;
    for (int j = 0; j < 2; ++j) {
        uint64_t t0 = marker(c, cs[j].ms), t1 = marker(c, cs[j].me), tb = marker(c, cs[j].mb);
        dmaon = dmaon && (logv(c, cs[j].i_on) & 0x4000) != 0;
        ref = ref && refresh_pre(acc, t0, t1, kF9Lines);
        std::vector<Region> chan = {{DB[j], DB[j] + 12}};
        std::vector<Acc> stray;
        auto rd = chan_or_stray(acc, t0, t1, chan, &stray);
        bool reads_ok = rd.size() == LEN[j] && stray.empty();
        for (size_t i = 0; reads_ok && i < LEN[j]; ++i) reads_ok = rd[i].addr == DB[j] + 2 * uint32_t(i);
        bool blk = cs[j].i_blk >= 0 && (logv(c, cs[j].i_blk) & 0x0002) != 0 && rd.size() >= LEN[j] && rd[LEN[j] - 1].t < tb;
        ok = ok && reads_ok && blk;
        ck.note("F9 LENGTH = " + dec(LEN[j]) + ": " + dec((long long)rd.size()) + " disk or stray reads, " + dec((long long)stray.size()) + " stray; DSKBYTR bit 14 " + dec((logv(c, cs[j].i_on) & 0x4000) ? 1 : 0) +
                "; INTREQR bit 1 " + dec(cs[j].i_blk >= 0 && (logv(c, cs[j].i_blk) & 2) ? 1 : 0) + "; " + dec((long long)refresh_reads(acc, t0, t1)) + " refresh reads in the window (at least " + dec((long long)refresh_min(kF9Lines)) + " required)");
    }
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: DMACONR reads DMAEN and DSKEN set (AA p.22)");
    ck.expect(dmaon, id + "-DMAON: DSKBYTR bit 14 DMAON reads 1 after the DSKLEN sequence of each case (AA p.23 'DMAEN(DSKLEN) & DMAEN(DMACON) & DSKEN(DMACON)'; HRM p.247 'the DMAEN bit in the DSKLEN register must be turned on twice in order to actually enable the disk DMA hardware'; HRM p.248 DMAON 'Indicates when DMA is actually enabled')");
    ck.expect(ref, id + "-REF: the window of each case holds at least 12 reads at whose cycle the refresh output is high, the four refresh reads of each of at least 3 whole lines (AA p.29 '4 refresh time slots'; the window holds the 4 line starts of its LINES(4) wait); that the four reads of a line form one group, and that no line holds more than four, is unfalsifiable at this boundary");
    ck.expect(tmo, id + "-TMO: no wait of the program ended by its timeout");
    fw_print(ck, id, fixture, fw_eval(c, acc, {{"IDLE", kIdle}}));
    if (!(fixture && dma && dmaon && ref && tmo)) { not_graded(ck, id, "a precondition"); return; }
    ck.expect(ok, "F9: a disk write DMA of LENGTH = 4 words and one of LENGTH = 3 words each read exactly LENGTH consecutive words from RAM starting at DSKPT, none of the guard words after them and no other word, and then raise the disk block request: INTREQR bit 1 is set after the last read (AA p.23 DSKLEN '13-0 LENGTH Length (# of words) of DMA data'; DSKDAT 'when the DMA data is finished (Length=0) it causes a Disk Block Interrupt.'; AA p.25 bit 1 DSKBLK 'Disk Block finished')");
}

} // namespace s7
