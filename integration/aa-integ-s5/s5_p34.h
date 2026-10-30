// s5_p34.h -- P3 (two's-complement bytes) and P4 (two bytes in sequence), AA p.11.
#pragma once

#include "s5_audio.h"

namespace s5 {

// One run of channel 0 with a data set of `words`, volume 64, period 300, played for `blocks` blocks.
inline ARun data_run(const std::vector<uint16_t>& words, unsigned blocks, uint16_t vol = 0x0040, uint16_t per = 300, int x = 0, const std::function<void(Script&)>& mid = nullptr) {
    ARun r;
    RunSpec rs; rs.bin = run_bin();
    Script& s = rs.sc;
    r.pre = prefix(s);
    poke_words(rs.pokes, kBuf[x], words);
    s.w32(audlc(x), kBuf[x]);
    s.w16(audlen(x), uint16_t(words.size())); s.w16(audper(x), per); s.w16(audvol(x), vol);
    r.m_start = s.w16(DMACON, start_val(1u << x));
    r.i_dmaconr = r.pre.logn++; s.rd(DMACONR);
    if (mid) mid(s);
    s.bwait(wc(unsigned(2 * per) * unsigned(words.size()) * (blocks + 1)));
    r.m_stop = s.w16(DMACON, uint16_t(1u << x));
    s.dly(200);
    s.end();
    rs.cycles = 4'000'000 + 8ull * (2 * per * words.size() * (blocks + 3));
    r.cap = run(rs);
    finish(r, kBuf[x], x);
    return r;
}

// ---- P3 ---------------------------------------------------------------------

inline void run_p3(Checks& ck) {
    const uint8_t B[9] = {0x00, 0x7F, 0x80, 0x01, 0xFF, 0x60, 0xA0, 0xE0, 0x20};
    std::vector<uint16_t> words;
    for (int i = 0; i < 9; ++i) words.push_back(uint16_t(B[i] * 0x0101));
    if (mut("P3-SWAP")) std::swap(words[1], words[2]);
    if (mut("P3-BYTE")) words[1] = 0x7F20;    // contrast: the second byte of word 2 is 0x20 in place of 0x7F
    ARun r = data_run(words, 4, 0x0040, mut("P3-PER") ? 600 : 300, 0,
                    mut("P3-CHG") ? std::function<void(Script&)>([](Script& sc) {   // contrast: after every block-finished request of the channel the CPU
                        for (int i = 0; i < 4; ++i) {              //   replaces word 5 (0xFF) by 0x4040 and 0xA0A0 in turn, so that
                            sc.waitb(INTREQR, 0x0080, 60000);      //   consecutive passes read different words
                            sc.w16(INTREQ, 0x0080);
                            sc.wm(kBuf[0] + 8, (i & 1) ? 0xA0A0 : 0x4040);
                        }
                    }) : std::function<void(Script&)>());
    PreAgg agg; agg.add(r.st);
    note_gc(ck, "P3", r.st);
    uint16_t d = logv(r.cap, r.i_dmaconr);
    bool gd = (d & 0x200) && (d & 1);
    auto ret = returns_idx(r.f, kBuf[0]);
    bool gf = ret.size() >= 4;
    bool gw = true, gp = true, ok_pass = true;
    double L1[2][9] = {}, L2[2][9] = {};   // level of the first and of the second byte position of each word, per pass
    uint64_t T0[2] = {0, 0};
    const uint64_t WL = 600 * r.K;         // one word: two samples of the period 300
    for (int p = 2; p <= 3; ++p) {
        Pass ps = pass_of(r, kBuf[0], p);
        if (ps.fi < 0 || ps.r0 < 0 || int(ret.size()) <= p) { ok_pass = false; continue; }
        gw &= ps.g_w;
        T0[p - 2] = r.runs[size_t(ps.r0)].t0;
        if (T0[p - 2] + 9 * WL > r.cap.base + r.cap.audio.size()) { ok_pass = false; continue; }
        // word j of the pass occupies [T0 + j WL, T0 + (j+1) WL); its byte positions are read in the middle of each half
        for (int j = 0; j < 9; ++j) {
            L1[p - 2][j] = double(out_at(r.cap, T0[p - 2] + uint64_t(j) * WL + WL / 4)) - r.Z;
            L2[p - 2][j] = double(out_at(r.cap, T0[p - 2] + uint64_t(j) * WL + 3 * WL / 4)) - r.Z;
        }
        std::string lv;
        for (int j = 0; j < 9; ++j) lv += num(L1[p - 2][j]) + "/" + num(L2[p - 2][j]) + " ";
        ck.note("P3 pass " + dec(p) + " levels by word, first byte position/second (bytes 00 7F 80 01 FF 60 A0 E0 20): " + lv);
    }
    // G-P: a pass of nine words of two samples of the period 300 lasts 9 * 600 colour clocks, from one pass anchor to the next
    if (ok_pass) gp = T0[1] > T0[0] && T0[1] - T0[0] == 9 * WL;
    ck.note("P3: pass-3 anchor run starts " + dec(ok_pass ? (long long)(T0[1] - T0[0]) : 0LL) + " cycles after the pass-2 anchor run (nine words of 600 colour clocks: " + dec((long long)(9 * WL)) + ")");
    print_std(ck, "P3", agg, 1);
    ck.expect(gd, "P3-G-D: after the start DMACONR bit 9 and bit 0 read 1 (AA p.22)");
    ck.expect(gf, "P3-G-F: the channel fetches at least four times at the location, so passes 2 and 3 are followed by a next anchor (AA p.22 '00 AUD0EN')");
    ck.expect(ok_pass && gw, "P3-G-W: passes 2 and 3 are located and no run starts in the system clock of either anchor fetch (SPECIFICATION.md 2.10)");
    ck.expect(ok_pass && gp, "P3-G-P: the anchor run of pass 3 starts exactly nine words of 600 colour clocks after the anchor run of pass 2, so each word spans two samples of the period 300 (AA p.11 AUDxPER, AUDxLEN)");
    bool pre = agg.fixture && agg.s0 && agg.gc && gd && gf && ok_pass && gw && gp;
    const std::string un = "fixture, G-S0, G-C, G-D, G-F, G-W or G-P";
    // the order, by word position: 0x80 < 0xA0 < 0xE0 < 0xFF < 0x00 < 0x01 < 0x20 < 0x60 < 0x7F
    const int order[9] = {2, 6, 7, 4, 0, 3, 8, 5, 1};   // word indexes in increasing level
    bool ord = true, same = true, bytes = true;
    for (int p = 0; p < 2; ++p)
        for (int k = 0; k + 1 < 9; ++k) ord &= L1[p][order[k + 1]] > L1[p][order[k]] && L2[p][order[k + 1]] > L2[p][order[k]];
    for (int j = 0; j < 9; ++j) same &= L1[0][j] == L1[1][j] && L2[0][j] == L2[1][j];
    for (int p = 0; p < 2; ++p)
        for (int j = 0; j < 9; ++j) bytes &= L1[p][j] == L2[p][j];
    graded(ck, pre, ord, "P3-ORDER: in passes 2 and 3 the levels are strictly ordered L(0x80) < L(0xA0) < L(0xE0) < L(0xFF) < L(0x00) < L(0x01) < L(0x20) < L(0x60) < L(0x7F), in the first byte position and in the second alike", un);
    graded(ck, pre, same, "P3-SAME: the same byte gives the same level in pass 2 and in pass 3 (equal), in each byte position", un);
    graded(ck, pre, bytes, "P3-BYTES: in passes 2 and 3 the two byte positions of every word, whose two bytes are equal, give the same level (AA p.11 'It contains 2 bytes of data (each byte is a twos complement signed integer)')", un);
    graded(ck, pre, ord && same && bytes, "P3 sol:AA-audxdat.c6: each byte of AUDxDAT, in either byte position, is a two's-complement signed integer: the output level increases strictly with the byte's two's-complement value (AA p.11 'It contains 2 bytes of data (each byte is a twos complement signed integer)')", un);
}

// ---- P4 ---------------------------------------------------------------------

inline void run_p4(Checks& ck) {
    const uint8_t a = 0x20, b = 0x60, c = 0xA0, d = 0xE0;
    const uint8_t W[10][2] = {{a, a}, {b, c}, {d, d}, {c, b}, {a, a}, {b, d}, {c, c}, {d, b}, {c, c}, {b, b}};
    std::vector<uint16_t> words;
    for (int i = 0; i < 10; ++i) words.push_back(uint16_t((W[i][0] << 8) | W[i][1]));
    if (mut("P4-SAME")) words[1] = uint16_t((b << 8) | b);
    if (mut("P4-LV")) words[1] = uint16_t((b << 8) | 0x40);   // contrast: the second byte of word 2 is 0x40, a level that is not calibrated
    if (mut("P4-CAL")) words[6] = words[8] = uint16_t((b << 8) | b);   // contrast: words 7 and 9 are (b,b) in place of (c,c), so L(c) is not calibrated apart from L(b)
    uint8_t Wl[10][2]; std::memcpy(Wl, W, sizeof Wl);
    ARun r = data_run(words, 4, 0x0040, mut("P4-PER") ? 600 : 300);
    PreAgg agg; agg.add(r.st);
    note_gc(ck, "P4", r.st);
    uint16_t dm = logv(r.cap, r.i_dmaconr);
    bool gd = (dm & 0x200) && (dm & 1);
    auto ret = returns_idx(r.f, kBuf[0]);
    bool gf = ret.size() >= 4;
    struct PassRes { bool located = false, gw = false, gp = false, cal = false, two = true, lens = true, lvl = true, other = true; std::string order; };
    PassRes pr[2];
    for (int p = 2; p <= 3; ++p) {
        PassRes& q = pr[p - 2];
        Pass ps = pass_of(r, kBuf[0], p);
        if (ps.fi < 0 || ps.r0 < 0 || int(ret.size()) <= p) continue;
        q.located = true;
        q.gw = ps.g_w;
        uint64_t T0 = r.runs[size_t(ps.r0)].t0, WL = 600 * r.K;
        if (std::getenv("NMH_DEBUG")) {
            std::string dbg;
            for (int k = std::max(0, ps.r0 - 3); k < std::min<int>(int(r.runs.size()), ps.r0 + 16); ++k)
                dbg += "[" + dec((long long)(r.runs[size_t(k)].t0) - (long long)ps.ta) + "+" + dec((long long)r.runs[size_t(k)].len()) + " v=" + dec(r.runs[size_t(k)].v) + "] ";
            ck.note("P4 debug pass " + dec(p) + " (run start relative to the anchor fetch, length, level): " + dbg);
        }
        // runs of the pass: those whose midpoint lies in [T0, T0 + 10 WL)
        std::vector<std::vector<size_t>> by(10);
        for (size_t i = size_t(ps.r0); i < r.runs.size(); ++i) {
            uint64_t mid2 = r.runs[i].t0 + r.runs[i].t1;   // twice the midpoint
            if (mid2 < 2 * T0) continue;
            uint64_t n = (mid2 / 2 - T0) / WL;
            if (n >= 10) break;
            by[n].push_back(i);
        }
        // calibration from the equal-byte words
        std::map<uint8_t, double> cal;
        q.gp = true;
        for (int n = 0; n < 10; ++n) {
            if (Wl[n][0] != Wl[n][1]) continue;
            if (by[size_t(n)].size() != 1) { q.gp = false; continue; }
            const LRun& u = r.runs[by[size_t(n)][0]];
            if (u.len() != WL) q.gp = false;
            cal[Wl[n][0]] = double(u.v) - r.Z;
        }
        q.cal = cal.size() == 4;
        for (auto& x : cal) for (auto& y : cal) if (x.first != y.first && x.second == y.second) q.cal = false;
        if (!(q.gw && q.gp && q.cal)) continue;
        for (int n = 0; n < 10; ++n) {
            if (Wl[n][0] == Wl[n][1]) continue;
            auto& ids = by[size_t(n)];
            if (ids.size() != 2) { q.two = false; continue; }
            for (size_t id : ids) {
                const LRun& u = r.runs[id];
                if (u.len() != WL / 2) q.lens = false;
            }
            double l1 = double(r.runs[ids[0]].v) - r.Z, l2 = double(r.runs[ids[1]].v) - r.Z;
            double e1 = cal[Wl[n][0]], e2 = cal[Wl[n][1]];
            bool fwd = l1 == e1 && l2 == e2, rev = l1 == e2 && l2 == e1;
            if (!(fwd || rev)) q.lvl = false;
            // no run in the interval has any other level
            for (size_t id : ids) {
                double l = double(r.runs[id].v) - r.Z;
                if (!(l == e1 || l == e2)) q.other = false;
            }
            q.order += fwd ? "H" : (rev ? "L" : "?");
        }
    }
    ck.note("P4 order of the two bytes of a word (H: high byte first, L: low byte first), pass 2: " + pr[0].order + " pass 3: " + pr[1].order + " (recorded, not graded)");
    print_std(ck, "P4", agg, 1);
    ck.expect(gd, "P4-G-D: after the start DMACONR bit 9 and bit 0 read 1 (AA p.22)");
    ck.expect(gf, "P4-G-F: the channel fetches at least four times at the location (AA p.22 '00 AUD0EN')");
    bool pre0 = agg.fixture && agg.s0 && agg.gc && gd && gf;
    bool use[2];
    for (int k = 0; k < 2; ++k) {
        const std::string pn = dec(k + 2);
        PassRes& q = pr[k];
        ck.expect(q.located && q.gw, "P4-G-W-" + pn + ": pass " + pn + " is located and no run starts in the system clock of its anchor fetch (SPECIFICATION.md 2.10)");
        ck.expect(q.located && q.gp, "P4-G-P-" + pn + ": each equal-byte word of pass " + pn + " gives one run of exactly 600 colour clocks (AA p.11 AUDxPER)");
        ck.expect(q.located && q.cal, "P4-PRE-CAL-" + pn + ": the four calibration levels L(a), L(b), L(c), L(d) of pass " + pn + " are pairwise distinct");
        use[k] = pre0 && q.located && q.gw && q.gp && q.cal;
    }
    const std::string un = "fixture, G-S0, G-C, G-D, G-F, G-W, G-P or the calibration of the pass";
    bool two = true, lens = true, lvl = true, any = false;
    for (int k = 0; k < 2; ++k) {
        const std::string pn = dec(k + 2);
        PassRes& q = pr[k];
        graded(ck, use[k], q.two, "P4-TWO-" + pn + ": in pass " + pn + " each word whose two bytes differ occupies exactly two runs in its 600-colour-clock interval (AA p.11 'that are outputed sequentially')", un);
        graded(ck, use[k], q.lens, "P4-LEN-" + pn + ": in pass " + pn + " each of those runs lasts exactly 300 colour clocks, one period per byte (AA p.11 AUDxPER)", un);
        graded(ck, use[k], q.lvl && q.other, "P4-LEVEL-" + pn + ": in pass " + pn + " one run's level equals L(b1), the other's equals L(b2), calibrated in the same pass, and no run has any other level (such as a sum or an average)", un);
        if (use[k]) { any = true; two &= q.two; lens &= q.lens; lvl &= q.lvl && q.other; }
    }
    graded(ck, pre0 && any, two && lens && lvl, "P4 sol:AA-audxdat.c7: the two bytes of AUDxDAT are output one after the other as two consecutive samples, one period each (AA p.11 'that are outputed sequentially (with digital to analog conversion) to the audio output pins'), in every pass whose preconditions hold", un);
}

} // namespace s5
