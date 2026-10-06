// s7_ceval.h -- the preconditions PC1-PC6 and the verdict of a C proposition (SPECIFICATION.md 1.9, 1.7).
#pragma once

#include "s7_c.h"

namespace s7 {

inline void not_graded(Checks& c, const std::string& id, const std::string& unmet) {
    c.record("NOT GRADED " + id + ": precondition(s) not met: " + unmet);
}

inline std::string cname(uint16_t c) { return "$" + hex4(c); }

struct CEval {
    bool fixture = false, pc1 = true, pc2 = true, pc3 = false, pc4 = true, pc5 = true, pc6 = true, assert_ok = true;
    bool have_pc5 = false, have_pc6 = false, runs = true, pcr = true, have_pcr = false, pc4_ng = false;
    std::vector<Region> copper;
};

inline const std::vector<Region>& copper_regions() {
    static const std::vector<Region> r = {{kIdle, kIdle + 0x10}, {kCase, kCase + 0x10}, {kCase + 0x40, kCase + 0x50}, {kCase + 0x80, kCase + 0x90}, {kCase + 0xC0, kCase + 0xD0}};
    return r;
}

// Evaluates every variant of a proposition and prints the notes; the caller prints the verdict lines.
inline CEval eval_c(Checks& ck, const Prop& p, const CRun& r) {
    CEval e;
    const Cap& c = r.cap;
    e.fixture = c.loaded && c.released && c.fixture_ok && c.done;
    e.pc3 = c.timeouts == 0;
    e.have_pc5 = p.dma_enable_bit != 0;
    if (e.have_pc5) {
        uint16_t dm = logv(c, r.dma_idx);
        e.pc5 = (dm & (0x0200 | p.dma_enable_bit)) == (0x0200 | p.dma_enable_bit);
    }
    Ctx cx{&c, &r.acc};
    std::vector<std::pair<uint64_t, uint64_t>> all_wins;
    std::vector<Region> all_chan;
    if (getenv("NMH_DEBUG")) {
        int n = 0;
        for (auto& kv : c.mark_t) if (kv.first < 40) ck.note(p.id + " debug: marker " + dec(kv.first) + " at cycle " + dec((long long)kv.second));
        {   // bursts of non-CPU writes: first cycle, first address, count, the last marker before
            uint64_t last = 0, start = 0; uint32_t first = 0; size_t cnt = 0; int bursts = 0;
            auto flush = [&]() {
                if (!cnt || bursts++ > 40) return;
                int mk = 0;
                for (auto& kv : c.mark_t) if (kv.second <= start && kv.first > mk) mk = kv.first;
                ck.note(p.id + " debug: burst of " + dec((long long)cnt) + " writes from $" + hex6(first) + " at cycle " + dec((long long)start) + " after marker " + dec(mk));
            };
            for (auto& a : r.acc) {
                if (!a.write || is_cpu(a.addr)) continue;
                if (cnt && a.t - last > 64) { flush(); cnt = 0; }
                if (!cnt) { start = a.t; first = a.addr; }
                ++cnt; last = a.t;
            }
            flush();
        }
        for (auto& a : r.acc) if (a.write && a.addr >= kResults && a.addr < kResults + 4 && n++ < 8)
            ck.note(p.id + " debug: write to RESULTS+" + dec(a.addr - kResults) + " data $" + hex4(a.data) + " at cycle " + dec((long long)a.t));
    }
    uint64_t K_run = 0;
    {
        std::vector<Acc> rf;
        for (auto& a : r.acc) if (a.refresh) rf.push_back(a);
        K_run = colour_clock(rf);
        ck.note(p.id + " run: " + dec((long long)c.base) + " clk_sys cycles to the release of the CPU, " + dec((long long)(c.end_cycle - c.base)) +
                " cycles run after it, K=" + dec((long long)K_run) + " clk_sys cycles per colour clock, bus period " + dec((long long)c.period) + " clk_sys cycles");
        std::vector<std::pair<std::string, uint32_t>> pw = {{"IDLE", kIdle + 4}};
        for (const auto& vp : r.plan) pw.push_back({vp.v.name + " list", vp.case_addr + 12});
        note_past_wait(ck, p.id, c, r.acc, pw);
    }
    for (const auto& vp : r.plan) {
        const Variant& v = vp.v;
        Obs c1 = v.ex(cx, marker(c, vp.c1.ms), marker(c, vp.c1.me), vp.c1.idx);
        Obs c0 = v.ex(cx, marker(c, vp.c0.ms), marker(c, vp.c0.me), vp.c0.idx);
        bool pc4 = c1 == v.W && c0 == (v.has_ucal0 ? v.U_cal0 : v.U) && v.W != v.U;
        e.pc4 = e.pc4 && pc4;
        ck.note(p.id + " " + v.name + " calibration: R_cal1=" + obs_str(c1, v.wide) + " (W=" + obs_str(v.W, v.wide) +
                ") R_cal0=" + obs_str(c0, v.wide) + " (U=" + obs_str(v.U, v.wide) + ")");
        if (v.pc6 || v.run_pre) e.have_pc6 = true;
        if (v.run_pre) e.pc6 = e.pc6 && v.run_pre(c, vp.c1.idx) && v.run_pre(c, vp.c0.idx);
        if (v.runs_ok) e.runs = e.runs && v.runs_ok(cx, marker(c, vp.c1.ms), marker(c, vp.c1.me), vp.c1.idx) &&
                                v.runs_ok(cx, marker(c, vp.c0.ms), marker(c, vp.c0.me), vp.c0.idx);
        if (v.pc6) e.pc6 = e.pc6 && v.pc6(cx, marker(c, vp.c1.ms), marker(c, vp.c1.me), c1) && v.pc6(cx, marker(c, vp.c0.ms), marker(c, vp.c0.me), c0);
        std::vector<std::pair<uint64_t, uint64_t>> wins = {{marker(c, vp.c1.ms), marker(c, vp.c1.me)}, {marker(c, vp.c0.ms), marker(c, vp.c0.me)}};
        std::vector<uint64_t> win_lo = {wins[0].first, wins[1].first};
        for (const auto& fb : vp.b) {
            uint64_t ms = marker(c, fb.ms), mw = marker(c, fb.mw), me = marker(c, fb.me);
            wins.push_back({ms, me});
            win_lo.push_back(v.b_window_from_start ? ms : mw);
            // PC1: a Copper read of the MOVE's first word (CASE+4) between M_start and M_wait
            std::vector<Region> none;
            Window wpc1 = classify(r.acc, ms, mw, v.chan, copper_regions());
            size_t n_move = 0, n_move2 = 0;
            for (auto& a : wpc1.copper) { if (a.addr == vp.case_addr + 4) ++n_move; if (a.addr == vp.case_addr + 6) ++n_move2; }
            bool pc1 = ms && mw && n_move >= 1;
            bool pc2 = (logv(c, fb.f_vb) & 0x0020) == 0;
            e.pc1 = e.pc1 && pc1;
            e.pc2 = e.pc2 && pc2;
            Obs o = v.ex(cx, v.b_window_from_start ? ms : mw, me, fb.idx);
            if (v.run_pre) e.pc6 = e.pc6 && v.run_pre(c, fb.idx);
            if (v.runs_ok) e.runs = e.runs && v.runs_ok(cx, mw, me, fb.idx);
            if (v.pc6) e.pc6 = e.pc6 && v.pc6(cx, v.b_window_from_start ? ms : mw, me, o);
            bool want_w = fb.c == 0x0002;
            bool ok = o == (want_w ? v.W : v.U);
            e.assert_ok = e.assert_ok && ok;
            ck.note(p.id + " " + v.name + " COPCON=" + cname(fb.c) + ": observation " + obs_str(o, v.wide) + " expected " +
                    (want_w ? "W" : "U") + (ok ? "" : " (differs)") + "; Copper reads of CASE+4: " + dec((long long)n_move) + ", of CASE+6: " +
                    dec((long long)n_move2) + "; F_vb=" + dec(logv(c, fb.f_vb) & 0x20 ? 1 : 0));
        }
        if (p.refresh_pc) {   // the refresh precondition of 1.6, in every window the proposition grades
            e.have_pcr = true;
            for (size_t i = 0; i < wins.size(); ++i) {
                Window wc = classify(r.acc, win_lo[i], wins[i].second, v.chan, copper_regions());
                uint64_t K = K_run;
                RefreshCheck rc = refresh_groups(wc.refresh, win_lo[i], wins[i].second, K);
                uint64_t lines = K && wins[i].second > win_lo[i] ? (wins[i].second - win_lo[i]) / (228 * K) : 0;
                ck.note(p.id + " refresh groups, window " + dec((long long)i) + ": " + dec((long long)lines) + " lines, " + dec((long long)rc.groups) + " groups, " + dec((long long)rc.whole) + " whole");
                if (!rc.ok) e.pcr = false;
                if (rc.whole + 1 < lines) e.pcr = false;
            }
        }
        if (v.blitter) {
            all_wins.insert(all_wins.end(), wins.begin(), wins.end());
            all_chan.insert(all_chan.end(), v.chan.begin(), v.chan.end());
        }
    }
    // PC5: no access of the channel outside the windows of the cases
    for (auto& a : r.acc) {
        if (a.refresh || is_cpu(a.addr)) continue;
        if (!a.write && !in_any(all_chan, a.addr)) continue;
        bool inside = false;
        for (auto& w : all_wins) if (a.t > w.first && a.t < w.second) inside = true;
        if (!inside) e.pc5 = false;
    }
    e.pc4_ng = !e.runs;   // PC4 compares frame A's reads, which a run with DMAON = 1 and no read does not have (C2)
    return e;
}

// Prints the precondition lines and the verdict of a proposition.
inline void print_c(Checks& ck, const Prop& p, const CEval& e, const std::string& claims_and_page, const std::string& what,
                    const std::string& pc_extra = "", const std::string& pc6_text = "") {
    const std::string id = p.id;
    ck.expect(e.fixture, id + "-FIXTURE: the run loaded, booted, stored the $BEEF constant at RESULTS and reached DONE, before any value is read");
    ck.expect(e.pc1, id + "-PC1: in every frame B a Copper read of the MOVE's first word (CASE+4) lies between M_start and M_wait, so the Copper reached the MOVE (AA p.20 MOVE format)");
    ck.expect(e.pc2, id + "-PC2: in every frame B no vertical blank started during the case (INTREQR bit 5 is 0 at the end), so the case list ran once (AA p.21, p.25)");
    ck.expect(e.pc3, id + "-PC3: no wait of the program ended by its timeout");
    if (e.pc4_ng) ck.record("NOT GRADED " + id + "-PC4: a run with DMAON = 1 read nothing, so frame A's reads that the precondition compares do not exist; " + id + " fails through 'Disk DMA runs'");
    else ck.expect(e.pc4, id + "-PC4: the CPU's own write of V1 gives W and V0 alone gives U in frame A, and W differs from U, so the observation tells the two apart");
    if (e.have_pc5) ck.expect(e.pc5, id + "-PC5: " + pc_extra);
    if (e.have_pc6) ck.expect(e.pc6, id + "-" + p.pcx + ": " + pc6_text);
    if (e.have_pcr) ck.expect(e.pcr, id + "-PCR: in every window the refresh-flagged reads come in groups of four, one group per line, so the refresh output marks the refresh reads (AA p.29 '4 refresh time slots'); the clause 'groups of four, one group per line' is unfalsifiable at this boundary, and 'at least one whole group lies in the window' is the population clause a stimulus can move");
    bool pre = e.fixture && e.pc1 && e.pc2 && e.pc3 && (e.pc4 || e.pc4_ng) && e.pc5 && e.pc6 && e.pcr;
    if (!pre) { not_graded(ck, id, "a precondition"); return; }
    ck.expect(e.assert_ok && e.runs, id + ": " + what + " " + claims_and_page);
}

} // namespace s7
