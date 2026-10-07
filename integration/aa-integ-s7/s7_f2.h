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
        if (sel == id + "-DMA" && contrast) en = 0x8200;                                       // contrast: the channel's enable is omitted
        if (sel == id + "-ONLY" && contrast) en = uint16_t(en | (1u << (cs.c ^ 1)));           // contrast: the other channel of the pair is enabled too
        if (sel == id + "-QUIET" && contrast) s.w16(DMACON, en);                               // contrast: the enable is written before M_start
        cs.ms = s.mark();
        s.lines(2);
        if (sel == id + "-REF" && contrast) cs.me = s.mark();                                   // contrast: M_end is written before M_go, so the window is empty
        cs.mg = s.mark();
        if (!(sel == id + "-QUIET" && contrast)) s.w16(DMACON, en);
        cs.i_dma = s.rd(DMACONR);
        if (!(sel == id + "-POP" && contrast)) s.lines(20);   // contrast of POP: M_end follows at once
        if (!(sel == id + "-REF" && contrast)) cs.me = s.mark();
        s.w16(DMACON, uint16_t(en & 0x000F));
        s.lines(4);
        regs.push_back({{cs.a, cs.a + 8}});
    }
    s.end();
    hd.rs.cycles = 60'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    const uint64_t K = run_K(acc);
    bool fixture = f_fixture(c), tmo = c.timeouts == 0;
    bool dma = true, only = true, quiet = true, pop = true, ref = true, ok = true;
    size_t first_at = 0, second_at = 0;
    for (size_t k = 0; k < cases.size(); ++k) {
        auto& cs = cases[k];
        uint64_t ms = marker(c, cs.ms), mg = marker(c, cs.mg), me = marker(c, cs.me);
        const uint16_t dm = logv(c, cs.i_dma);
        dma = dma && (dm & (0x0200 | (1u << cs.c))) == (0x0200u | (1u << cs.c));
        only = only && (dm & 0x000F) == (1u << cs.c);
        quiet = quiet && chan_or_stray(acc, ms, mg, regs[k]).empty();
        std::vector<Acc> stray;
        auto rd = chan_or_stray(acc, mg, me, regs[k], &stray);
        pop = pop && rd.size() >= 2;
        ref = ref && refresh_pre(acc, K, mg, me);
        bool first_or_second = rd.size() >= 2 && (rd[0].addr == cs.a || rd[1].addr == cs.a);
        ok = ok && first_or_second && first_read_only_stray(rd, stray);
        first_at += !rd.empty() && rd[0].addr == cs.a;
        second_at += rd.size() > 1 && rd[1].addr == cs.a;
        char buf[240];
        std::snprintf(buf, sizeof buf, "F4 channel %d case %zu: location written $%06X, first read $%06X, second read $%06X, %zu reads outside [a, a+8)", cs.c, k, cs.a,
                      rd.empty() ? 0u : rd[0].addr, rd.size() < 2 ? 0u : rd[1].addr, stray.size());
        ck.note(buf);
    }
    ck.note("F4: " + dec((long long)first_at) + " of " + dec((long long)cases.size()) + " cases read the written location first, " + dec((long long)second_at) + " read it second");
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: in every case DMACONR reads DMAEN and the enable bit of the channel set (AA p.22)");
    ck.expect(only, id + "-ONLY: in every case DMACONR reads no enable bit of another audio channel set, so only channel c runs (AA p.22)");
    ck.expect(quiet, id + "-QUIET: in every case no audio read and no stray DMA read lies between M_start and M_go");
    ck.expect(pop, id + "-POP: in every case at least two audio reads or stray DMA reads lie between M_go and M_end");
    ck.expect(ref, id + "-REF: in every case the refresh-flagged reads between M_go and M_end come in groups of four, one group per line (AA p.29 '4 refresh time slots'); the clause 'groups of four, one group per line' is unfalsifiable at this boundary, and 'at least one whole group lies in the window' is the population clause a stimulus can move");
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
        cs.m0 = s.mark();
        for (int x = 0; x < 8; ++x) {
            uint32_t a = kS[i] ^ (uint32_t(x) << 6);
            uint16_t lo = uint16_t(a & 0xFFFF);
            if (sel == id && i == 1 && x == 3) lo = uint16_t(lo ^ 0x0100);   // contrast: another address in SPR3PTL
            if (sel == id + "-STRAY" && i == 1 && x == 3) lo = uint16_t(lo ^ 0x0400);   // contrast: A10 flipped, outside A8-A6, so the read lies outside all eight regions
            s.w32(uint16_t(SPR0PTH + 4 * x), (a & 0xFFFF0000u) | lo);
        }
        if (sel == id + "-QUIET" && i == 1) s.lines(60);
        if (sel == id + "-REF" && i == 1) cs.m2 = s.mark();   // the contrast: M2 is written before M1, so the window is empty
        cs.m1 = s.mark();
        if (sel == id + "-POP" && i == 1) s.lines(3); else if (sel == id + "-REF" && i == 1) { /* the contrast: M2 is written before M1 */ } else s.vblsync();
        if (!(sel == id + "-REF" && i == 1)) cs.m2 = s.mark();
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
    for (int i = 0; i < nfam; ++i) {
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
    note_past_wait(ck, id, c, acc, {{"IDLE", kIdle + 4}});
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: DMACONR reads DMAEN and SPREN set (AA p.22)");
    ck.expect(quiet, id + "-QUIET: in every case no sprite read and no stray DMA read lies between M0 and M1");
    ck.expect(pop, id + "-POP: in every case at least eight sprite reads or stray DMA reads lie between M1 and M2");
    ck.expect(ref, id + "-REF: in every case the refresh-flagged reads between M1 and M2 come in groups of four, one group per line (AA p.29 '4 refresh time slots'); the clause 'groups of four, one group per line' is unfalsifiable at this boundary, and 'at least one whole group lies in the window' is the population clause a stimulus can move");
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

// F7: the channel's audio DMA writes AUDxDAT with the RAM words, which reach the output as signed bytes. Its
// precondition on the channel's reads allows at most one read outside [AD, AD+8), the channel's first after the
// enable (the pages leave the address of the first read open; see F4).
inline void run_f7(Checks& ck, const std::string& sel) {
    const std::string id = "F7";
    FHead hd = f_head(id, sel, 0x8000);
    Script& s = hd.rs.sc;
    hd.rs.want_audio = true;
    const uint32_t AD = 0x0D8000;
    poke_words(hd.rs.pokes, AD, {0x4040, uint16_t(sel == id ? 0x6060 : 0xC0C0), 0x2020, 0x0000});   // contrast: the second word is +96 in place of -64
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
        if (sel == id + "-REF" && first) cs.me = s.mark();       // contrast: M_end is written before the enable, so the window is empty
        uint16_t en = uint16_t(0x8200 | (1u << c));
        if (sel == id + "-DMA" && first) en = 0x8200;            // contrast: the channel's enable is omitted
        s.w16(DMACON, en);
        cs.i_dma = s.rd(DMACONR);
        if (sel == id + "-POP" && first) s.lines(5);             // contrast: a window of five lines holds fewer than eight reads
        else if (sel == id + "-STRAY" && first) { s.lines(60); s.w16(uint16_t(audlc(c) + 2), 0x8100); s.lines(40); }   // contrast: the location is rewritten to AD+$100, so the next restart reads outside
        else s.lines(100);
        if (!(sel == id + "-REF" && first)) cs.me = s.mark();
        s.w16(DMACON, uint16_t(1u << c)); s.w16(audvol(c), 0);
        cases.push_back(cs);
    }
    s.end();
    hd.rs.cycles = 30'000'000;
    Cap c = run(hd.rs);
    std::vector<Acc> acc = accesses(c);
    const uint64_t K = run_K(acc);
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
        pop = pop && tt.size() >= 8 && cnt[0] >= 2 && cnt[1] >= 2 && cnt[2] >= 2 && cnt[3] >= 2;
        stray_ok = stray_ok && first_read_only_stray(rd, stray);
        ref = ref && refresh_pre(acc, K, t0, t1);
        uint64_t T = tt.size() > 1 ? (tt.back() - tt.front()) / (tt.size() - 1) : 0;
        auto rl = level_runs(sl, c.base, t0, t1, T), rr = level_runs(sr, c.base, t0, t1, T);
        bool pat = has_pattern(rl) || has_pattern(rr);
        ok = ok && pat;
        std::string sa;
        for (auto& a : stray) sa += " $" + hex6(a.addr) + (a.t == (rd.empty() ? 0 : rd[0].t) ? " (the first read)" : "");
        ck.note("F7 channel " + dec(ch) + ": " + dec((long long)tt.size()) + " audio reads in [AD, AD+8), " + dec((long long)stray.size()) + " other reads (at" + sa +
                "); runs of the left output " + dec((long long)rl.size()) + ", of the right output " + dec((long long)rr.size()) + "; the level sequence (L1, L2, L3, L4) three times with L1 > L3 > L4 > L2: " + (pat ? "present" : "absent"));
    }
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: in every case DMACONR reads DMAEN and the enable bit of the channel set (AA p.22)");
    ck.expect(adk, id + "-ADK: ADKCONR & $00FF reads 0 before each case, so no modulation bit is set (AA p.11 USExVy / USExPy clear)");
    ck.expect(pop, id + "-POP: in every case at least eight audio reads lie in the window, each of the four addresses of [AD, AD+8) at least twice");
    ck.expect(stray_ok, id + "-STRAY: in every case at most one read of the window lies outside [AD, AD+8), and it is the channel's first read after the enable");
    ck.expect(ref, id + "-REF: in every case the refresh-flagged reads of the window come in groups of four, one group per line (AA p.29 '4 refresh time slots'); the clause 'groups of four, one group per line' is unfalsifiable at this boundary, and 'at least one whole group lies in the window' is the population clause a stimulus can move");
    ck.expect(tmo, id + "-TMO: no wait of the program ended by its timeout");
    if (!(fixture && dma && adk && pop && stray_ok && ref && tmo)) { not_graded(ck, id, "a precondition"); return; }
    ck.expect(ok, "F7: with channel c's audio DMA on (c = 0-3), the words the channel reads from RAM are written into AUDcDAT and output as two's-complement bytes: on at least one output signal the runs, in time order, contain the sequence (L1, L2, L3, L4) three times in succession with L1 > L3 > L4 > L2, the levels of the bytes +64, -64, +32, 0 (AA p.11 'This register is the Audio channel x (x=0,1,2,3) DMA data buffer. It contains 2 bytes of data (each byte is a twos complement signed integer) that are outputed sequentially (with digital to analog conversion) to the audio output pins.', 'The audio DMA channel controller automatically transfers data to this register from RAM.'; AA p.7, p.8 access W) claims opus:AA-aud0dat.c2 sol:AA-aud0dat.c2 opus:AA-aud1dat.c2 sol:AA-aud1dat.c2 opus:AA-aud2dat.c2 sol:AA-aud2dat.c2 opus:AA-aud3dat.c2 sol:AA-aud3dat.c2");
}

// ---- F9: DSKDAT fed by the disk DMA ------------------------------------------------------------
inline void run_f9(Checks& ck, const std::string& sel) {
    const std::string id = "F9";
    FHead hd = f_head(id, sel, sel == id + "-DMA" ? 0x8200 : 0x8210);
    Script& s = hd.rs.sc;
    const uint32_t DB = 0x0D2000;
    poke_words(hd.rs.pokes, DB, {0x1111, 0x2222, 0x3333, 0x4444, 0x5555, 0x6666});
    s.w16(ADKCON, 0x9100); s.w16(INTREQ, 0x0002);
    s.w32(DSKPTH, sel == id + "-STRAY" ? DB + 0x1000 : DB);   // contrast: DSKPT outside [DB, DB+12)
    int me = 0;
    if (sel == id + "-REF") me = s.mark();   // the contrast: M_end is written before M_start, so the window is empty
    int ms = s.mark();
    uint16_t len = sel == id ? 0xC003 : 0xC004;
    s.w16(DSKLEN, 0x4000);
    if (sel == id + "-DMAON") { s.w16(DSKLEN, len); } else { s.w16(DSKLEN, len); s.w16(DSKLEN, len); }
    int i_on = s.rd(DSKBYTR);
    int i_blk = -1, mb = 0;
    {
        i_blk = s.pollb(INTREQR, 0x0002, 6000);
        mb = s.mark();
        s.w16(DSKLEN, 0x4000);
        s.lines(4);
        if (sel != id + "-REF") me = s.mark();
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
    note_past_wait(ck, id, c, acc, {{"IDLE", kIdle + 4}});
    ck.expect(fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(dma, id + "-DMA: DMACONR reads DMAEN and DSKEN set (AA p.22)");
    ck.expect(dmaon, id + "-DMAON: DSKBYTR bit 14 DMAON reads 1 after the DSKLEN sequence (AA p.23 'DMAEN(DSKLEN) & DMAEN(DMACON) & DSKEN(DMACON)'; HRM p.247 'the DMAEN bit in the DSKLEN register must be turned on twice in order to actually enable the disk DMA hardware'; HRM p.248 DMAON 'Indicates when DMA is actually enabled')");
    ck.expect(ref, id + "-REF: the refresh-flagged reads of the window come in groups of four, one group per line (AA p.29 '4 refresh time slots'); the clause 'groups of four, one group per line' is unfalsifiable at this boundary, and 'at least one whole group lies in the window' is the population clause a stimulus can move");
    ck.expect(tmo, id + "-TMO: no wait of the program ended by its timeout");
    if (!(fixture && dma && dmaon && ref && tmo)) { not_graded(ck, id, "a precondition"); return; }
    ck.expect(reads_ok && blk, "F9: a disk write DMA of LENGTH = 4 words reads exactly four consecutive words from RAM starting at DSKPT, none of the guard words after them and no other word, and then raises the disk block request: INTREQR bit 1 is set after the fourth read (AA p.23 DSKLEN '13-0 LENGTH Length (# of words) of DMA data'; DSKDAT 'when the DMA data is finished (Length=0) it causes a Disk Block Interrupt.'; AA p.25 bit 1 DSKBLK 'Disk Block finished')");
}

} // namespace s7
