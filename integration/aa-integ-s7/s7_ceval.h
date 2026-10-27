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
    bool any_distinct = false;   // some case has W != U
    Fw fw;
    std::vector<Region> copper;
};

inline const std::vector<Region>& copper_regions() {
    static const std::vector<Region> r = {{kIdle, kIdle + 0x10}, {kCase, kCase + 0x500}};   // IDLE, and the case lists (16 bytes each at kCase + 16 i, or at kCase + 0x40 k)
    return r;
}

// One line of notes for the cases of one register (the cases run n at a time): how many cases there were and in how many each
// observation was as expected, with the first differences.
struct Agg {
    size_t n = 0, cal_ok = 0, move_reads = 0, refresh_windows = 0, refresh_min_seen = ~size_t(0);
    std::map<uint16_t, size_t> ok_by_c;
    std::vector<std::string> bad;
};

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
    {
        ck.note(p.id + " run: " + dec((long long)c.base) + " clk_sys cycles to the release of the CPU, " + dec((long long)(c.end_cycle - c.base)) +
                " cycles run after it, bus period " + dec((long long)c.period) + " clk_sys cycles");
        std::vector<FwList> fl = {{"IDLE", kIdle}};
        for (const auto& vp : r.plan) fl.push_back({vp.v.name + (p.batch ? " lists" : " list"), vp.case_addr + 8});
        e.fw = fw_eval(c, r.acc, fl);
    }
    std::map<std::string, Agg> agg;
    std::vector<std::string> agg_order;
    for (const auto& vp : r.plan) {
        const Variant& v = vp.v;
        Agg& ag = agg[v.name];
        if (!ag.n) agg_order.push_back(v.name);
        ++ag.n;
        Obs c1 = v.ex(cx, marker(c, vp.c1.ms), marker(c, vp.c1.me), vp.c1.idx);
        Obs c0 = v.ex(cx, marker(c, vp.c0.ms), marker(c, vp.c0.me), vp.c0.idx);
        bool pc4 = c1 == v.W && c0 == (v.has_ucal0 ? v.U_cal0 : v.U) && (p.batch > 0 || v.W != v.U);
        if (v.W != v.U) e.any_distinct = true;
        e.pc4 = e.pc4 && pc4;
        if (pc4) ++ag.cal_ok;
        const std::string vname = v.name + " V1=$" + hex4(v.v1);
        if (!pc4 && ag.bad.size() < 6) ag.bad.push_back(vname + " calibration R_cal1=" + obs_str(c1, v.wide) + " (W=" + obs_str(v.W, v.wide) + ") R_cal0=" + obs_str(c0, v.wide) + " (U=" + obs_str(v.U, v.wide) + ")");
        if (p.batch == 0)
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
            if (ok) ++ag.ok_by_c[fb.c]; else ag.ok_by_c[fb.c] += 0;
            ag.move_reads += n_move;
            if (!ok && ag.bad.size() < 6) ag.bad.push_back(vname + " COPCON=" + cname(fb.c) + " observation " + obs_str(o, v.wide) + " expected " + (want_w ? "W=" + obs_str(v.W, v.wide) : "U=" + obs_str(v.U, v.wide)));
            if (p.batch == 0)
                ck.note(p.id + " " + v.name + " COPCON=" + cname(fb.c) + ": observation " + obs_str(o, v.wide) + " expected " +
                        (want_w ? "W" : "U") + (ok ? "" : " (differs)") + "; Copper reads of CASE+4: " + dec((long long)n_move) + ", of CASE+6: " +
                        dec((long long)n_move2) + "; F_vb=" + dec(logv(c, fb.f_vb) & 0x20 ? 1 : 0));
        }
        if (p.refresh_pc) {   // the refresh precondition of 1.6, in every window the proposition grades
            e.have_pcr = true;
            for (size_t i = 0; i < wins.size(); ++i) {
                const size_t n = refresh_reads(r.acc, win_lo[i], wins[i].second);
                ++ag.refresh_windows;
                ag.refresh_min_seen = std::min(ag.refresh_min_seen, n);
                if (p.batch == 0)
                    ck.note(p.id + " refresh reads, window " + dec((long long)i) + ": " + dec((long long)n) + " (at least " + dec((long long)refresh_min(kC2Lines)) + " required)");
                if (n < refresh_min(kC2Lines)) e.pcr = false;
            }
        }
        if (v.blitter) {
            all_wins.insert(all_wins.end(), wins.begin(), wins.end());
            all_chan.insert(all_chan.end(), v.chan.begin(), v.chan.end());
        }
    }
    if (p.batch > 0) {
        for (auto& nm : agg_order) {
            const Agg& ag = agg[nm];
            std::string s = p.id + " " + nm + ": " + dec((long long)ag.n) + " values of V1; calibration as expected in " + dec((long long)ag.cal_ok);
            for (auto& kv : ag.ok_by_c) s += "; COPCON=" + cname(kv.first) + " observation as expected in " + dec((long long)kv.second);
            s += "; Copper reads of the MOVE's first word " + dec((long long)ag.move_reads);
            if (ag.refresh_windows) s += "; " + dec((long long)ag.refresh_windows) + " windows with at least " + dec((long long)ag.refresh_min_seen) + " refresh reads each (" + dec((long long)refresh_min(kC2Lines)) + " required)";
            if (!ag.bad.empty()) { s += "; differences:"; for (auto& b : ag.bad) s += " [" + b + "]"; }
            ck.note(s);
        }
    }
    // PC5: no access of the channel outside the windows of the cases
    std::sort(all_wins.begin(), all_wins.end());
    for (auto& a : r.acc) {
        if (all_wins.empty() || a.t < all_wins.front().first || a.refresh || is_cpu(a.addr)) continue;   // from M_start of the first case on: what the channels did before the first case is not a case's blit or disk transfer
        if (!a.write && !in_any(all_chan, a.addr)) continue;
        bool inside = false;
        auto it = std::upper_bound(all_wins.begin(), all_wins.end(), std::make_pair(a.t, ~uint64_t(0)));
        if (it != all_wins.begin()) {   // the windows of the cases do not overlap: the access lies in the one that starts last before it, or in none
            --it;
            inside = a.t > it->first && a.t < it->second;
        }
        if (!inside) {
            e.pc5 = false;
            static int shown = 0;
            if (getenv("NMH_DEBUG") && shown++ < 12) ck.note(p.id + " debug: PC5 access outside the windows: " + (a.write ? "write" : "read") + " $" + hex6(a.addr) + " at cycle " + dec((long long)a.t));
        }
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
    else ck.expect(e.pc4 && (p.batch == 0 || e.any_distinct), id + "-PC4: the CPU's own write of V1 gives W and V0 alone gives U in frame A" + (p.batch ? ", for every value of V1, and W differs from U for some of them" : ", and W differs from U") + ", so the observation tells the two apart");
    if (e.have_pc5) ck.expect(e.pc5, id + "-PC5: " + pc_extra);
    if (e.have_pc6) ck.expect(e.pc6, id + "-" + p.pcx + ": " + pc6_text);
    if (e.have_pcr) ck.expect(e.pcr, id + "-PCR: every window holds at least 28 reads at whose cycle the refresh output is high, the four refresh reads of each of at least 7 whole lines (AA p.29 '4 refresh time slots'; each window holds the 8 line starts of its LINES(8) wait); that the four reads of a line form one group, and that no line holds more than four, is unfalsifiable at this boundary");
    fw_print(ck, id, e.fw);
    bool pre = e.fixture && e.pc1 && e.pc2 && e.pc3 && (e.pc4 || e.pc4_ng) && e.pc5 && e.pc6 && e.pcr;
    if (!pre) { not_graded(ck, id, "a precondition"); return; }
    ck.expect(e.assert_ok && e.runs, id + ": " + what + " " + claims_and_page);
}

} // namespace s7
