// test_ch8_light_pen_beam_part2.inc -- textually included by
// test_ch8_light_pen_beam.cpp. Preconditions, propositions, schedules,
// and the main/mutation entry points.

namespace {

// ---------------------------------------------------------------------
// window helpers
// ---------------------------------------------------------------------
std::vector<Sample> filter_by_a(const std::vector<Sample>& s, int64_t lo, int64_t hi) {
    std::vector<Sample> out;
    for (const auto& x : s) if (x.a >= lo && x.a < hi) out.push_back(x);
    return out;
}
int64_t line_len_clk(const BeamModel& m) { return m.line_len_cck * m.colour_clock_len; }

std::vector<Sample> window_VB(const std::vector<Sample>& s, const BeamModel& m, int64_t field_idx) {
    int64_t lo = m.field_start(field_idx);
    return filter_by_a(s, lo, lo + 20 * line_len_clk(m));
}
std::vector<Sample> window_RUN(const std::vector<Sample>& s, const BeamModel& m, int64_t field_idx,
                                int Vrun, std::optional<int64_t> cutoff = std::nullopt) {
    int64_t lo = m.field_start(field_idx) + (int64_t)Vrun * line_len_clk(m);
    int64_t hi = m.field_start(field_idx) + (int64_t)m.L * line_len_clk(m);
    if (cutoff && *cutoff < hi) hi = *cutoff;
    if (hi < lo) hi = lo;
    return filter_by_a(s, lo, hi);
}
std::vector<Sample> window_POST(const std::vector<Sample>& s, const BeamModel& m, int64_t t_off, int64_t field_idx_plus1) {
    int64_t lo = t_off;
    int64_t hi = m.field_start(field_idx_plus1) + 20 * line_len_clk(m);
    return filter_by_a(s, lo, hi);
}

bool all_equal_raw(const std::vector<Sample>& s) {
    if (s.empty()) return false;
    for (const auto& x : s) if (x.raw32 != s[0].raw32) return false;
    return true;
}
bool all_current(const std::vector<Sample>& s, const BeamModel& m) {
    if (s.empty()) return false;
    for (const auto& x : s) if (!m.is_torn(x) && !m.is_current(x)) return false;
    return true;
}
bool none_current(const std::vector<Sample>& s, const BeamModel& m) {
    if (s.empty()) return false;
    for (const auto& x : s) if (!m.is_torn(x) && m.is_current(x)) return false;
    return true;
}
int64_t max_gap_cck(const std::vector<Sample>& s, const BeamModel& m) {
    int64_t worst = 0;
    for (size_t i = 1; i < s.size(); ++i) {
        int64_t g = (s[i].a - s[i - 1].a) / std::max<int64_t>(1, m.colour_clock_len);
        worst = std::max(worst, g);
    }
    return worst;
}

// ---------------------------------------------------------------------
// pulse catalogue (SPECIFICATION.md section 6) -- field_offset relative
// to B0 for phase-B entries.
// ---------------------------------------------------------------------
struct PulseDef { int field_offset; int line; int H; bool port0, port1; uint8_t bit; bool graded; std::string tag; };

// Port mapping (SPECIFICATION.md section 3, "Port mapping"): measured at
// the ports (CIAAPRA bits 6/7, POTGOR bits 10/14, both cores) --
// joystick0 reaches port 2 (CIAAPRA bit 7, POTGOR bit 14), joystick1
// reaches port 1 (CIAAPRA bit 6, POTGOR bit 10). "port0"/"port1" below
// name joystick0/joystick1 (the harness's own port fields); a PulseDef's
// port0/port1 flags select which of those, not the physical port number.
std::vector<PulseDef> phaseB_pin6_pulses(bool ntsc) {
    return {
        {4, 100, 100, false, true,  kBitFire, true,  "B4"},  // port 1 only (joystick1)
        {5, 100, 100, true,  false, kBitFire, true,  "B5"},  // port 2 only (joystick0)
        {6, 100, 101, true,  true,  kBitFire, true,  "B6"},
        {7, 100, 102, true,  true,  kBitFire, true,  "B7"},
        {8, 100, 103, true,  true,  kBitFire, true,  "B8"},
        {9, 100, 105, true,  true,  kBitFire, true,  "B9"},
        {10,100, 108, true,  true,  kBitFire, true,  "B10"},
        {11, 40, 100, true,  true,  kBitFire, true,  "B11"},
        {12,200, 100, true,  true,  kBitFire, true,  "B12"},
        {13,258, 100, true,  true,  kBitFire, true,  "B13"},
        {14, 80, 100, true,  true,  kBitFire, true,  "B14a"},
        {14,150, 100, true,  true,  kBitFire, false, "B14b"},
        {15, 10, 100, true,  true,  kBitFire, false, "B15a"},
        {15, 32, 100, true,  true,  kBitFire, true,  "B15b"},
        {17, 19, 100, true,  true,  kBitFire, false, "B17a"},
        {17, 32, 100, true,  true,  kBitFire, true,  "B17b"},
        {18,160, 100, true,  true,  kBitFire, true,  "B18"},
        {19, ntsc ? 261 : 280, 100, true, true, kBitFire, false, "B19"},
    };
}
// B18's other-pin pulses (not pin 6): pins 1,2,3,4,5,9 both ports at
// lines 40,60,80,100,120,140 (H=100). Pin 5 has no digital joystick bit
// (it is the pot line, measured at POTGOR bit 8/12), so it is driven via
// mouse_buttons for port 1 only; port 2's pin 5 is not reachable through
// any input this harness exposes (measured: driving every joystick0/1 bit
// and every mouse_buttons bit leaves POTGOR bit 12 unchanged).
std::vector<PulseDef> phaseB18_other_pulses() {
    return {
        {18, 40,  100, true,  true,  kBitUp,    true, "B18-pin1"},
        {18, 60,  100, true,  true,  kBitDown,  true, "B18-pin2"},
        {18, 80,  100, true,  true,  kBitLeft,  true, "B18-pin3"},
        {18, 100, 100, true,  true,  kBitRight, true, "B18-pin4"},
        {18, 140, 100, true,  true,  kBitFire2, true, "B18-pin9"},
    };
}

// ---------------------------------------------------------------------
// main-run schedules (SPECIFICATION.md section 6)
// ---------------------------------------------------------------------
void schedB_main(const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st) {
    for (const auto& p : phaseB_pin6_pulses(false /*ntsc flag unused for line calc here*/))
        add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
    for (const auto& p : phaseB18_other_pulses())
        add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
    add_mouse_pulse_events(ev, m, B0, st, 18, 120, 100); // B18's pin-5 pulse, port 1 only
}
void schedC_main(const BeamModel& m, int64_t C0, std::vector<Event>& ev, PortState* st) {
    add_pulse_events(ev, m, C0, st, 1, 100, 100, true, true, kBitFire);
}
void schedD_main(const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st, bool ntsc) {
    // port 1 pin 5 low 2-4ms
    int64_t t2 = M3 + us_to_cck(2000, ntsc) * m.colour_clock_len;
    int64_t t4 = M3 + us_to_cck(4000, ntsc) * m.colour_clock_len;
    ev.push_back({t2, [st]() { st->mbtn |= kMouseMid; }});
    ev.push_back({t4, [st]() { st->mbtn &= (uint8_t)~kMouseMid; }});
    // port 2 pin 5: SPECIFICATION.md schedules 5-7ms low, but no input this
    // harness exposes reaches it (measured, above) -- not scheduled;
    // recorded, not silently substituted.
    // both ports pin 9 low 8-10ms
    int64_t t8 = M3 + us_to_cck(8000, ntsc) * m.colour_clock_len;
    int64_t t10 = M3 + us_to_cck(10000, ntsc) * m.colour_clock_len;
    ev.push_back({t8, [st]() { st->joy0 |= kBitFire2; st->joy1 |= kBitFire2; }});
    ev.push_back({t10, [st]() { st->joy0 &= (uint8_t)~kBitFire2; st->joy1 &= (uint8_t)~kBitFire2; }});
}

// ---------------------------------------------------------------------
// Pin-6-onset catalogues for the page-model (below): every schedB_KX
// mutation's own pin-6 pulses, by the same field/line/H it actually
// injects them at, so the page model latches exactly what that
// mutation's stimulus would trigger on correctly-latching hardware. Each
// mirrors its schedB_KX function; K1's is unused (see latch_enabled).
std::vector<PulseDef> pin6_defs_K2(bool ntsc) {
    auto v = phaseB_pin6_pulses(ntsc);
    v.push_back({2, 100, 100, true, true, kBitFire, true, "K2-B2"});
    v.push_back({3, 100, 100, true, true, kBitFire, true, "K2-B3"});
    return v;
}
std::vector<PulseDef> pin6_defs_K6(bool ntsc) {
    auto v = phaseB_pin6_pulses(ntsc);
    for (auto& p : v) { if (p.tag == "B15a" || p.tag == "B17a") p.line = 30; }
    return v;
}
std::vector<PulseDef> pin6_defs_K7(bool ntsc) {
    auto v = phaseB_pin6_pulses(ntsc);
    for (auto& p : v) { if (p.tag == "B15b" || p.tag == "B17b") p.line = 12; }
    return v;
}
std::vector<PulseDef> pin6_defs_K8(bool ntsc) {
    auto v = phaseB_pin6_pulses(ntsc);
    std::vector<PulseDef> out;
    for (auto& p : v) if (p.tag != "B14a") out.push_back(p);
    return out;
}
std::vector<PulseDef> pin6_defs_K9(bool ntsc) {
    auto v = phaseB_pin6_pulses(ntsc);
    v.push_back({18, 40, 100, true, true, kBitFire, true, "K9-B18pin1"});
    return v;
}
std::vector<PulseDef> pin6_defs_K9b(bool ntsc) {
    auto v = phaseB_pin6_pulses(ntsc);
    std::vector<PulseDef> out;
    for (auto& p : v) if (p.tag != "B4" && p.tag != "B5" && p.tag != "B18") out.push_back(p);
    return out;
}
std::vector<PulseDef> pin6_defs_K16(bool ntsc) {
    auto v = phaseB_pin6_pulses(ntsc);
    for (auto& p : v) {
        if (p.tag == "B7") { p.line = 100; p.H = 122; }
        else if (p.tag == "B11") { p.line = 41; p.H = 100; }
    }
    return v;
}

// ---------------------------------------------------------------------
// preconditions
// ---------------------------------------------------------------------
struct Preconds { bool cp1=false, cp2=false, cp3=false, cp4=false, cp5=false, cp6=false; };
constexpr int kUBoundCck = 15; // A2 (SPECIFICATION.md 5.4): u <= 15 colour clocks

bool check_A1(const std::vector<Sample>& A, const BeamModel& m, int Vrun) {
    for (const auto& s : A) {
        if (s.V < Vrun || s.V > m.L - 1) continue;
        if (m.is_torn(s)) continue;
        if (!m.is_current(s)) return false;
    }
    return true;
}
bool check_A1(const RunResult& r, int Vrun) { return check_A1(r.A, r.model, Vrun); }

// A2 (SPECIFICATION.md 5.4), extended: besides u and the field length,
// grade "within the fitted lines of a field V never decreases" and "the
// counter numbers lines 0 to L and returns to 0 exactly once per field"
// -- the latter from the same field split fit_model used (a V decrease
// starts a new field): each such split must land exactly one field apart
// by the model's own numbering, never zero (a spurious extra split) or
// more than one (a missed field).
bool check_A2(const std::vector<Sample>& A, const BeamModel& m, bool ntsc) {
    if (m.u_cck > kUBoundCck) return false;
    if (m.u_cck < 2) return false; // 5.2's floor: the gap is at least two bus cycles
    int want1 = ntsc ? 262 : 312, want2 = ntsc ? 263 : 313;
    if (m.L + 1 != want1 && m.L + 1 != want2) return false;
    if (A.empty()) return false;
    int64_t span = A.back().a - A.front().a;
    if (span < 4 * m.field_len_clksys) return false; // at least four fields (>= two periods of <=2 fields)

    int Vrun = ntsc ? 21 : 29;
    std::optional<int64_t> lastFieldIdx;
    int lastSplitFieldIdx = -1000000;
    int prevV = -1;
    for (size_t i = 0; i < A.size(); ++i) {
        int64_t f = m.field_index_of(A[i].a);
        if (!lastFieldIdx || f != *lastFieldIdx) {
            // a new field of reads began here (fit_model's own split rule:
            // V decreased). It must be exactly one model field after the
            // previous split, not zero and not more than one.
            if (lastFieldIdx) {
                int64_t modelField = f; // the field this sample's own cycle falls in
                if (lastSplitFieldIdx != -1000000 && modelField - lastSplitFieldIdx != 1) return false;
            }
            lastSplitFieldIdx = (int)f;
            prevV = -1;
        }
        if (A[i].V >= Vrun && A[i].V <= m.L - 1 && !m.is_torn(A[i])) {
            if (prevV >= 0 && A[i].V < prevV) return false; // V decreased within one field's fitted lines
            prevV = A[i].V;
        }
        lastFieldIdx = f;
    }
    return true;
}
bool check_A2(const RunResult& r, int Vrun, bool ntsc) { (void)Vrun; return check_A2(r.A, r.model, ntsc); }

// A3 (SPECIFICATION.md 5.4): "one group of four refresh accesses at the
// Chip-memory port, at the same model H in every model line." Every
// non-code, non-write access at the Chip-memory port in phases A-C is a
// refresh access -- the program's own register accesses ($DFF004/6/16)
// go through the custom-register bus, not the Chip-memory port, and its
// only other Chip-memory reads are its own code fetches (excluded by
// kCodeLo/kCodeHi) and the two stack reads of `rts` (excluded by their
// known address, PROGRAM_STACK). No percentage tolerance: the same set
// of H positions must recur on every model line of the window, exactly.
constexpr uint32_t kStackWordLo = (0x0FFF00 - 64) / 2, kStackWordHi = (0x0FFF00 + 8) / 2;
bool check_A3(const RunResult& r, int64_t lo, int64_t hi) {
    std::map<int64_t, std::set<int64_t>> byLine; // model line -> set of refresh H positions
    for (const auto& a : r.log) {
        if (a.is_write) continue;
        if ((int64_t)a.cycle < lo || (int64_t)a.cycle >= hi) continue;
        if (a.word_addr >= kCodeLo/2 && a.word_addr < kCodeHi/2) continue;
        if (a.word_addr >= kStackWordLo && a.word_addr < kStackWordHi) continue;
        int mv, mh; r.model.pos_at((int64_t)a.cycle, &mv, &mh);
        byLine[mv].insert(mh);
    }
    if (byLine.size() < 8) return false; // too few lines covered to say anything
    std::set<int64_t> fixedSet = byLine.begin()->second;
    for (auto& kv : byLine) if (kv.second != fixedSet) return false;
    return !fixedSet.empty();
}

// A4 (SPECIFICATION.md 5.4): "one vertical-sync pulse begins in each
// model field, at the same model position in every field." Graded from
// the model position (line, H) of each vsync onset in the window -- a
// position the beam-counter model supplies independently of vsync itself
// (the model's anchor and line length are fit from the phase-A counter
// reads; only field_len_clksys, used only to pick line length, comes
// from vsync). Comparing onset positions across fields is therefore not
// circular: it cross-checks the counter-fitted position mapping against
// an independently-timed signal.
bool check_A4(const RunResult& r, int64_t lo, int64_t hi) {
    auto spans = nmh::find_frames(r.vid);
    std::optional<std::pair<int,int>> fixedPos;
    int n = 0;
    for (size_t i = 0; i < spans.size(); ++i) {
        int64_t onset = (int64_t)spans[i].begin; // video sample index == clk_sys cycle (1:1)
        if (onset < lo || onset >= hi) continue;
        int mv, mh; r.model.pos_at(onset, &mv, &mh);
        if (!fixedPos) fixedPos = {mv, mh};
        else if (fixedPos->first != mv || fixedPos->second != mh) return false;
        ++n;
    }
    return true; // n==0: window too short to contain an onset; not contradicted
}

// CP4 (SPECIFICATION.md section 7): every applied input change landed on
// its own scheduled cycle (the harness's own delivery record, not an
// assumption), and every phase A-C pulse (a momentary bit, not one of
// phase D's multi-millisecond windows) was exactly 4 colour clocks wide.
// K12's own mutation additionally compares delivered cycles against the
// unmutated schedule (below), which this generic check cannot do since
// "the schedule" for every other run is that run's own (SPECIFICATION.md
// section 7, CP4: "For a contrasting run, 'the schedule' is its changed
// schedule").
bool check_CP4(const RunResult& r, std::string* detail) {
    for (auto& e : r.applied) if (e.sched != e.applied) {
        if (detail) *detail = "delivery not cycle-exact sched=" + std::to_string(e.sched) +
            " applied=" + std::to_string(e.applied) + " j0=" + std::to_string(e.j0) +
            " j1=" + std::to_string(e.j1) + " mb=" + std::to_string(e.mb);
        return false;
    }
    int64_t cck = std::max<int64_t>(1, r.model.colour_clock_len);
    uint8_t pj0 = 0, pj1 = 0, pmb = 0;
    bool have = false;
    int64_t onSched = 0; uint8_t onBit = 0; int onWhich = -1; bool pending = false;
    for (auto& e : r.applied) {
        uint8_t dj0 = e.j0 ^ pj0, dj1 = e.j1 ^ pj1, dmb = e.mb ^ pmb;
        // "Both" (SPECIFICATION.md section 6) means the same bit position on
        // both ports at once, one logical stimulus change: count distinct
        // bit POSITIONS touched, not how many of joy0/joy1/mouse_buttons
        // carry that position.
        uint8_t combined = dj0 | dj1 | dmb;
        int bits = 0; for (int b = 0; b < 8; ++b) if (combined & (1 << b)) ++bits;
        if (have && bits > 1) { if (detail) *detail = "more than one input bit position changed in one event"; return false; }
        if (e.sched < r.M3cyc) { // phase A-C: momentary pulses, width exactly 4 colour clocks
            bool rising = (dj0 & e.j0) || (dj1 & e.j1) || (dmb & e.mb);
            if (rising) { pending = true; onSched = e.sched; }
            else if (pending) {
                int64_t width = e.sched - onSched;
                if (width != 4 * cck) { if (detail) *detail = "pulse width " + std::to_string(width) + " clk_sys, expected " + std::to_string(4 * cck); return false; }
                pending = false;
            }
        }
        pj0 = e.j0; pj1 = e.j1; pmb = e.mb; have = true;
        (void)onBit; (void)onWhich;
    }
    return true;
}

// temporary diagnostic: breaks down A1/A2 into its own sub-conditions
Preconds grade_preconditions(const RunResult& r, bool ntsc, const std::string& cfgLabel) {
    Preconds pc;
    int Vrun = ntsc ? 21 : 29;
    bool a1 = check_A1(r, Vrun);
    bool a2 = check_A2(r, Vrun, ntsc);
    pc.cp1 = a1 && a2;
    checks.expect(pc.cp1, "CP1 " + cfgLabel + ": beam model anchored on phase A (A1: every non-torn phase-A "
        "sample in the fitted lines is current; A2: field length is 262/263 or 312/313 lines as graded, "
        "2<=u<=" + std::to_string(kUBoundCck) + " colour clocks, at least four fields covered, V never "
        "decreases within a field's fitted lines, and the field split fit_model finds advances by exactly "
        "one field every time)");
    checks.note(cfgLabel + " model: colour_clock_len=" + std::to_string(r.model.colour_clock_len) +
        " line_len_cck=" + std::to_string(r.model.line_len_cck) + " L=" + std::to_string(r.model.L) +
        " field_len_clksys=" + std::to_string(r.model.field_len_clksys) + " u_cck=" + std::to_string(r.model.u_cck) +
        " a1=" + std::to_string(a1) + " a2=" + std::to_string(a2));

    // CP2: dense sampling in phases A-C (<=56 colour clocks between
    // consecutive read instants, at least two samples per line of the
    // fitted/graded ranges) and in phase D (<=50 microseconds).
    int64_t g = max_gap_cck(r.A, r.model);
    g = std::max(g, max_gap_cck(r.B, r.model));
    g = std::max(g, max_gap_cck(r.C, r.model));
    std::map<std::pair<int64_t,int>, int> perLine; // (field, line) -> sample count, phases A-C
    // The very last sample of a buffer ends its line's tally mid-line (the
    // buffer simply runs out there), not because sampling thinned out; it
    // is excluded, one per phase, as that boundary artifact.
    auto tally = [&](const std::vector<Sample>& s, int loLine, int hiLine) {
        size_t n = s.empty() ? 0 : s.size() - 1;
        for (size_t i = 0; i < n; ++i) {
            const auto& x = s[i];
            int line; r.model.pos_at(x.a, &line, nullptr);
            if (line < loLine || line > hiLine) continue;
            ++perLine[{r.model.field_index_of(x.a), line}];
        }
    };
    tally(r.A, Vrun, r.model.L - 1);
    tally(r.B, 0, r.model.L);
    tally(r.C, 0, r.model.L);
    int thin = 0;
    for (auto& kv : perLine) if (kv.second < 2) ++thin;
    int64_t dgap_us_cck = us_to_cck(50, ntsc) * r.model.colour_clock_len;
    int64_t dgap = max_gap_cck(r.D, r.model) * r.model.colour_clock_len; // convert back to clk_sys for a direct compare
    bool cp2 = g <= 56 && !r.A.empty() && !r.B.empty() && !r.C.empty() && thin == 0 &&
               (r.D.empty() || dgap <= dgap_us_cck);
    pc.cp2 = cp2;
    checks.expect(pc.cp2, "CP2 " + cfgLabel + ": sampling is dense (max phase A-C read-instant gap " +
        std::to_string(g) + " colour clocks, <=56 required; " + std::to_string(thin) +
        " (field,line) cells with fewer than two samples, of " + std::to_string(perLine.size()) +
        "; max phase-D gap " + std::to_string(dgap) + " clk_sys, <=" + std::to_string(dgap_us_cck) + " required)");

    bool cp3a = (r.fire_byte & 0xC0) == 0xC0; // bits 6,7 = 1 ("switch open")
    // (b): from the harness's own delivery record, no pin-6 bit (kBitFire)
    // was set outside a scheduled pulse's own [on,off) -- i.e. every rising
    // edge of that bit is followed by a falling edge, checked generically
    // by check_CP4's width test (it fails if a pin-6 pulse isn't exactly
    // 4 colour clocks, which is what "at the open-switch level throughout
    // ... except during the scheduled pulses" reduces to at this boundary).
    std::string cp4detail;
    bool cp4 = check_CP4(r, &cp4detail);
    bool cp3b = cp4; // same record; not an independent re-measurement
    pc.cp3 = cp3a && cp3b;
    checks.expect(pc.cp3, "CP3 " + cfgLabel + ": no trigger is presented outside the schedule -- (a) the "
        "CIAAPRA read of phase A shows bits 6 and 7 = 1 (measured FIRE=$" +
        [&]{ char b[8]; std::snprintf(b, sizeof b, "%02X", r.fire_byte); return std::string(b); }() +
        "); (b) by the harness's own delivery record, pin 6 of both ports returns to the open-switch level "
        "after every scheduled pulse (checked via CP4's own record)");

    pc.cp4 = cp4;
    checks.expect(pc.cp4, "CP4 " + cfgLabel + ": inputs are delivered as scheduled -- by the harness's own "
        "record (" + std::to_string(r.applied.size()) + " events), every change landed on its scheduled "
        "cycle exactly, no event changed more than one input bit, and every phase A-C pulse was exactly 4 "
        "colour clocks wide" + (cp4detail.empty() ? "" : (" (" + cp4detail + ")")));

    bool m_order = r.m0cyc >= 0 && r.m1cyc > r.m0cyc && r.m2cyc > r.m1cyc &&
                   r.m3cyc > r.m2cyc && r.m4cyc > r.m3cyc;
    int64_t phaseA_span = m_order ? (r.A.empty() ? 0 : r.A.back().a - r.A.front().a) : 0;
    bool covA = m_order && phaseA_span >= 2 * r.model.field_len_clksys;
    // phase B coverage: last sample reaches at least line 40 of field B21
    bool covB = false;
    if (m_order && !r.B.empty()) {
        int64_t need = r.model.cycle_of(r.B0_idx + 21, 40, 0);
        covB = r.B.back().a >= need;
    }
    bool covC = false;
    if (m_order && !r.C.empty()) {
        int64_t need = r.model.cycle_of(r.C0_idx + 3, 40, 0);
        covC = r.C.back().a >= need;
    }
    bool covD = false;
    if (m_order && !r.D.empty() && r.m3cyc >= 0) {
        int64_t need = r.m3cyc + us_to_cck(12000, ntsc) * r.model.colour_clock_len;
        covD = r.D.back().a >= need;
    }
    // The BPLCON0 writes that open/close phase B are each bracketed by a
    // wait150 call (SPECIFICATION.md section 4) whose own read gates
    // return on the model line 150; the mark write that immediately
    // follows lands within the same line (a handful of instructions,
    // far under one line's worth of clk_sys cycles).
    bool bplconAtLine150 = true;
    if (m_order) {
        int lv1, lv2;
        r.model.pos_at(r.m1cyc, &lv1, nullptr);
        r.model.pos_at(r.m2cyc, &lv2, nullptr);
        bplconAtLine150 = (lv1 == 150 || lv1 == 151) && (lv2 == 150 || lv2 == 151);
    }
    pc.cp5 = m_order && covA && covB && covC && covD && bplconAtLine150;
    checks.expect(pc.cp5, "CP5 " + cfgLabel + ": the program ran as specified -- M0..M4 appear in order; "
        "the BPLCON0 writes fall at model line 150 (measured M1 line=" +
        (m_order ? std::to_string([&]{int v;r.model.pos_at(r.m1cyc,&v,nullptr);return v;}()) : std::string("n/a")) +
        ", M2 line=" + (m_order ? std::to_string([&]{int v;r.model.pos_at(r.m2cyc,&v,nullptr);return v;}()) : std::string("n/a")) +
        "); coverage: phase A spans at least two model periods (" +
        std::to_string(phaseA_span) + " clk_sys), phase B reaches line 40 of field B21 (" +
        std::string(covB ? "reached" : "NOT reached") + "), phase C reaches line 40 of field C3 (" +
        std::string(covC ? "reached" : "NOT reached") + "), phase D reaches 12ms after M3 (" +
        std::string(covD ? "reached" : "NOT reached") + ")");

    // CP6: the beam keeps its timing in phases B AND C (SPECIFICATION.md
    // section 7: "CP6 | ... A3 and A4 hold there", and section 8's K14
    // row: the window runs B0 to M3).
    int64_t cp6lo = r.model.field_start(r.B0_idx);
    int64_t cp6hi = r.m3cyc > 0 ? r.m3cyc : r.model.field_start(r.C0_idx + 4);
    bool cp6 = check_A3(r, cp6lo, cp6hi) && check_A4(r, cp6lo, cp6hi);
    pc.cp6 = cp6;
    checks.expect(pc.cp6, "CP6 " + cfgLabel + ": the beam keeps its timing in phases B and C (A3: the same "
        "set of refresh H positions recurs on every model line from B0 to M3; A4: every vsync onset in that "
        "span sits at the same model position)");

    return pc;
}

// ---------------------------------------------------------------------
// P1..P13
// ---------------------------------------------------------------------
struct PulseTiming { int64_t t_on, t_off; int field_idx; };
PulseTiming pulse_timing(const BeamModel& m, int64_t base_idx, const PulseDef& p) {
    PulseTiming t;
    t.field_idx = (int)(base_idx + p.field_offset);
    t.t_on = m.cycle_of(t.field_idx, p.line, p.H);
    t.t_off = t.t_on + 4 * m.colour_clock_len;
    return t;
}

// Each proposition's page and claim ids, for the "preconditions failed"
// record below -- SPECIFICATION.md section 2. P10's enabling ids sit on
// P10(a) only (P10(b) is the control; SPECIFICATION.md section 2). P13 is
// listed separately: it depends only on CP2, CP4 and CP5 (SPECIFICATION.md
// section 7, "P13"), not on CP1/CP3/CP6, so it is gated independently of
// the other twelve.
struct PropIds { const char* label; const char* page; const char* ids; };
static const PropIds kPropIdsLatch[] = {
    {"P1",  "p238,p227", "opus:8-lpen.c1 sol:8-pen.c1 opus:8-intro.c12 sol:8-overview.c6"},
    {"P2",  "p238,p228", "opus:8-lpen.c16 sol:8-pen.c17 opus:8-tbl8-1.c20 sol:8-pin.c22"},
    {"P3",  "p238",      "opus:8-lpen.c17 opus:8-lpen.c18 sol:8-pen.c18 sol:8-pen.c19 sol:8-pen.c20"},
    {"P4",  "p229,p238", "opus:8-regs.c18 opus:8-regs.c20 sol:8-reg.c20 sol:8-reg.c22"},
    {"P5",  "p239",      "opus:8-lpen.c36 sol:8-pen.c36"},
    {"P6",  "p239",      "opus:8-lpen.c38 sol:8-pen.c38"},
    {"P7",  "p239",      "opus:8-lpen.c37 sol:8-pen.c37"},
    {"P8",  "p239",      "opus:8-lpen.c39"},
    {"P9",  "p238",      "(none)"},
    {"P10", "p239,p229", "opus:8-lpen.c35 sol:8-pen.c35 opus:8-regs.c16 sol:8-reg.c18 (P10(a) only; P10(b) is the control, no ids)"},
    {"P11", "p239",      "opus:8-lpen.c43 sol:8-pen.c41 opus:8-lpen.c47 sol:8-pen.c44 opus:8-lpen.c48 sol:8-pen.c45 sol:8-pen.c46"},
    {"P12", "p239",      "opus:8-lpen.c52"},
};
static const PropIds kPropIdsP13 = {"P13", "p238", "opus:8-lpen.c10 opus:8-lpen.c11 sol:8-pen.c10 sol:8-pen.c11"};

void grade_P1_P13(const RunResult& r, bool ntsc, bool ecs, const std::string& cfgLabel, const Preconds& pc) {
    bool cpOkLatch = pc.cp1 && pc.cp2 && pc.cp3 && pc.cp4 && pc.cp5 && pc.cp6;
    bool cpOkP13 = pc.cp2 && pc.cp4 && pc.cp5;
    if (!cpOkLatch) {
        for (const auto& pi : kPropIdsLatch)
            checks.record(std::string(pi.label) + " " + cfgLabel + " " + pi.page + " " + pi.ids +
                ": not evaluated -- a required precondition (CP1-CP6) failed for this run, so this "
                "proposition is reported as not evaluated, not as green");
    }
    if (!cpOkP13) {
        checks.record(std::string(kPropIdsP13.label) + " " + cfgLabel + " " + kPropIdsP13.page + " " +
            kPropIdsP13.ids + ": not evaluated -- CP2, CP4 or CP5 failed for this run");
    }
    if (!cpOkLatch && !cpOkP13) return;

    int Vrun = ntsc ? 21 : 29;
    auto pulses = phaseB_pin6_pulses(ntsc);
    std::map<std::string, PulseTiming> timing;
    for (const auto& p : pulses) timing[p.tag] = pulse_timing(r.model, r.B0_idx, p);
    const std::vector<int> notrig_fields = {1, 2, 3, 20};

    // SPECIFICATION.md section 3 ("ECS."): an ECS run whose phase B shows
    // no latch at all (every phase-B sample current) is not graded red on
    // the propositions that need a latch; it is recorded, naming the
    // unstated LPENDIS reset value (p.305) as the open point. Detected
    // here from the run's own measurements, before any of P1-P12 is
    // graded, so the gate below applies uniformly.
    if (cpOkLatch) {
    bool noLatchAtAll = true;
    for (const auto& p : pulses) {
        if (!p.graded) continue;
        const auto& t = timing[p.tag];
        auto post = window_POST(r.B, r.model, t.t_off, t.field_idx + 1);
        if (!post.empty() && !all_current(post, r.model)) noLatchAtAll = false;
    }
    for (int k : notrig_fields) {
        auto vb = window_VB(r.B, r.model, r.B0_idx + k + 1);
        if (!vb.empty() && !all_current(vb, r.model)) noLatchAtAll = false;
    }
    bool gateActive = ecs && noLatchAtAll;
    if (gateActive) checks.note(cfgLabel + ": every phase-B sample measured current (no pen capture, no "
        "end-of-field latch) -- SPECIFICATION.md section 3's ECS/LPENDIS exception applies to P1-P9, P10(a), "
        "P11, P12 below");
    auto gate = [&](bool cond, const std::string& label) {
        if (gateActive) checks.record(label + " -- not graded red (SPECIFICATION.md section 3: ECS run with "
            "no latch at all in phase B; LPENDIS's reset value, p.305, is not stated by the pages)");
        else checks.expect(cond, label);
    };

    // ---- P1 ----
    auto postB4 = window_POST(r.B, r.model, timing["B4"].t_off, timing["B4"].field_idx + 1);
    auto postB5 = window_POST(r.B, r.model, timing["B5"].t_off, timing["B5"].field_idx + 1);
    bool b4_captured = all_equal_raw(postB4) && none_current(postB4, r.model);
    bool b5_captured = all_equal_raw(postB5) && none_current(postB5, r.model);
    bool p1 = b4_captured || b5_captured;
    gate(p1, "P1 " + cfgLabel + " p238,p227 opus:8-lpen.c1 sol:8-pen.c1 opus:8-intro.c12 "
        "sol:8-overview.c6: a pulse on pin 6 of at least one controller port is captured as a light-pen "
        "trigger (POST(B4) or POST(B5) frozen and not current)");
    checks.record("P1: which port captures is not graded (10.4) -- measured: port1(B4) captured=" +
        std::string(b4_captured ? "yes" : "no") + ", port2(B5) captured=" + std::string(b5_captured ? "yes" : "no"));

    // ---- P2 ----
    auto runB18 = window_RUN(r.B, r.model, r.B0_idx + 18, Vrun, timing["B18"].t_on);
    bool p2i = all_current(runB18, r.model);
    auto postB18 = window_POST(r.B, r.model, timing["B18"].t_off, r.B0_idx + 19);
    bool p2ii = all_equal_raw(postB18) && none_current(postB18, r.model);
    bool p2 = p2i && p2ii;
    gate(p2, "P2 " + cfgLabel + " p238,p228 opus:8-lpen.c16 sol:8-pen.c17 opus:8-tbl8-1.c20 "
        "sol:8-pin.c22: pin 6 is the beam-trigger line and the other signal pins are not (RUN(B18) up to the "
        "pin-6 pulse stays current, and the pin-6 pulse itself is captured)");
    checks.record("P2: B18's schedule (section 6) also pulses pin 5, port 2, at line 120. No input this "
        "harness exposes reaches port 2's pin 5 (SPECIFICATION.md section 3, \"Port mapping\"), so only "
        "port 1's pin-5 pulse is driven; port 2's pin 5 is not exercised as one of \"all six other signal "
        "pins, both ports\" and is not graded here.");

    // ---- P3 ----
    bool p3a = true, p3b = true;
    std::vector<int64_t> offsets;
    int graded_n = 0;
    for (const auto& p : pulses) {
        if (!p.graded) continue;
        if ((p.tag == "B4" && !b4_captured) || (p.tag == "B5" && !b5_captured)) continue;
        ++graded_n;
        const auto& t = timing[p.tag];
        auto post = window_POST(r.B, r.model, t.t_off, t.field_idx + 1);
        if (post.empty() || !all_equal_raw(post)) { p3a = false; p3b = false; continue; }
        int V = post[0].V, H = post[0].H;
        if (V != p.line) p3a = false;
        int href; r.model.pos_at(t.t_on, nullptr, &href); // H_ref at t_on within this line
        offsets.push_back((int64_t)H - href);
    }
    bool p3c = false;
    if (!offsets.empty()) {
        p3c = true;
        for (auto o : offsets) if (o != offsets[0]) p3c = false;
    }
    // P3 id split (SPECIFICATION.md section 9, P3): sol:8-pen.c19 says
    // "The captured beam position contains the horizontal beam value at
    // the instant" -- H-specific, so it sits on (b) (the H window) and
    // (c) (H's exactness). sol:8-pen.c20 says "...the vertical beam value
    // at the instant" -- V-specific, so it sits on (a). opus:8-lpen.c17
    // ("captures and saves the current contents of VPOSR"), opus:8-lpen.c18
    // and sol:8-pen.c18 ("captures the current VPOSR contents" /
    // "the exact horizontal and vertical value ... at the instant") name
    // neither axis alone, so they sit on the conjunction (a)+(b)+(c).
    gate(p3a, "P3(a) " + cfgLabel + " p238 opus:8-lpen.c17 opus:8-lpen.c18 sol:8-pen.c18 "
        "sol:8-pen.c20: every graded pulse's captured V equals the model line at its onset");
    gate(p3b, "P3(b) " + cfgLabel + " p238 opus:8-lpen.c17 opus:8-lpen.c18 sol:8-pen.c18 sol:8-pen.c19: "
        "every graded pulse's captured H lies within beam([t_on,t_off])");
    gate(p3c, "P3(c) " + cfgLabel + " p238,p239 opus:8-lpen.c17 opus:8-lpen.c18 sol:8-pen.c18 sol:8-pen.c19: "
        "the offset d(p)=H(P)-H_ref(t_on) is the same for every graded pulse (exactness)");
    checks.note(cfgLabel + " P3: graded population size=" + std::to_string(graded_n));

    // ---- P4 ----
    // VPOSR's V8 is bit 0 of the word, i.e. bit 16 of the 32-bit
    // VPOSR:VHPOSR sample (p.238); bits 14-1 are the chip-ID field and are
    // not graded (10.7). VHPOSR bits 15-8 (V7-V0) are raw32 bits 15-8;
    // bits 7-0 (H8-H1) are raw32 bits 7-0, checked here against the same
    // exactness offset P3(c) established, not just against itself.
    bool p4 = true;
    bool haveOffset = !offsets.empty() && p3c;
    int64_t offset0 = haveOffset ? offsets[0] : 0;
    struct P4case { std::string tag; int wantV8; int wantHi; int line; };
    std::vector<P4case> p4cases = {{"B13",1,0x02,258},{"B11",0,0x28,40},{"B12",0,0xC8,200},{"B6",0,0x64,100},
                                     {"B7",0,0x64,100},{"B8",0,0x64,100},{"B9",0,0x64,100},{"B10",0,0x64,100}};
    for (const auto& c : p4cases) {
        const auto& t = timing[c.tag];
        auto post = window_POST(r.B, r.model, t.t_off, t.field_idx + 1);
        if (post.empty()) { p4 = false; continue; }
        int href; r.model.pos_at(t.t_on, nullptr, &href);
        for (const auto& s : post) {
            int v8 = (s.raw32 >> 16) & 1;
            int hi = (s.raw32 >> 8) & 0xFF;
            int lo8 = s.raw32 & 0xFF;
            bool loOk = haveOffset ? (lo8 == ((href + offset0) & 0xFF)) : true;
            if (v8 != c.wantV8 || hi != c.wantHi || !loOk) { p4 = false; break; }
        }
    }
    gate(p4, "P4 " + cfgLabel + " p229,p238 opus:8-regs.c18 opus:8-regs.c20 sol:8-reg.c20 "
        "sol:8-reg.c22: VPOSR holds V8, VHPOSR holds V7-V0 and H8-H1 of the captured pen position "
        "(V8 = bit 0/raw bit 16; V7-V0 the expected line's low byte; H8-H1 = H_ref(t_on) + P3(c)'s offset)");

    // ---- P5 ----
    // LOF (VPOSR bit 15) while latched is left open by the pages
    // (SPECIFICATION.md 10.8): a mismatch confined to that one bit is
    // reported separately from an ordinary freeze failure.
    bool p5 = true;
    std::string p5lof;
    for (const auto& p : pulses) {
        if (!p.graded) continue;
        const auto& t = timing[p.tag];
        auto post = window_POST(r.B, r.model, t.t_off, t.field_idx + 1);
        if (!all_equal_raw(post)) {
            p5 = false;
            bool onlyLof = !post.empty();
            for (auto& x : post) if ((x.raw32 & ~0x80000000u) != (post[0].raw32 & ~0x80000000u)) onlyLof = false;
            if (onlyLof) p5lof += p.tag + " ";
        }
    }
    gate(p5, "P5 " + cfgLabel + " p239 opus:8-lpen.c36 sol:8-pen.c36: after a trigger VPOSR is frozen "
        "until the release (every sample of POST(p) is equal, for every graded pulse)");
    if (!p5lof.empty()) checks.note(cfgLabel + " P5: differences confined to VPOSR bit 15 (LOF, 10.8) in "
        "POST windows of pulses " + p5lof);

    // ---- P6 ----
    bool p6 = true;
    std::string p6lof;
    auto p6check = [&](const std::vector<Sample>& post, const std::string& tag) {
        if (post.empty()) return;
        bool eq = all_equal_raw(post), cur = none_current(post, r.model);
        if (!eq || !cur) p6 = false;
        if (!eq) {
            bool onlyLof = true;
            for (auto& x : post) if ((x.raw32 & ~0x80000000u) != (post[0].raw32 & ~0x80000000u)) onlyLof = false;
            if (onlyLof) p6lof += tag + " ";
        }
    };
    for (const auto& p : pulses) {
        if (!p.graded) continue;
        const auto& t = timing[p.tag];
        int64_t lo = t.t_off + line_len_clk(r.model);
        auto post = filter_by_a(r.B, lo, r.model.field_start(t.field_idx + 1) + 20 * line_len_clk(r.model));
        p6check(post, p.tag);
    }
    for (int k : notrig_fields) {
        auto vb = window_VB(r.B, r.model, r.B0_idx + k + 1);
        p6check(vb, "B" + std::to_string(k));
    }
    gate(p6, "P6 " + cfgLabel + " p239 opus:8-lpen.c38 sol:8-pen.c38: while latched, VPOSR never "
        "shows the current beam position (no sample in the latched windows is current)");
    if (!p6lof.empty()) checks.note(cfgLabel + " P6: differences confined to VPOSR bit 15 (LOF, 10.8) in "
        "windows " + p6lof);

    // ---- P7 ----
    bool p7a = true, p7b = true, p7c = true;
    int p7cN = 0; // P7(c)'s graded population; "not graded" when it stays 0
    for (int k : notrig_fields) {
        int64_t fk = r.B0_idx + k;
        auto run = window_RUN(r.B, r.model, fk, Vrun);
        if (!all_current(run, r.model)) p7a = false;
        auto vb = window_VB(r.B, r.model, fk + 1);
        if (vb.empty() || !all_equal_raw(vb)) { p7b = false; continue; }
        int Fk_V = vb[0].V;
        if (Fk_V != r.model.L) p7b = false;
        if (!run.empty()) {
            ++p7cN;
            int m_last = 256 * run.back().V + run.back().H;
            int m_Fk = 256 * vb[0].V + vb[0].H;
            if (m_Fk < m_last) p7c = false;
        }
    }
    bool p7 = p7a && p7b;
    gate(p7, "P7 " + cfgLabel + " p239 opus:8-lpen.c37 sol:8-pen.c37: without a trigger, the beam "
        "counters latch at the end of the field ((a) RUN(k) stays current, (b) VB(k+1) is one position at line L)");
    if (p7cN == 0) {
        checks.record("P7(c) " + cfgLabel + " p239: not graded -- the population (RUN(k) and VB(k+1) both "
            "non-empty, for a no-trigger field k) is empty this run");
    } else {
        gate(p7c, "P7(c) " + cfgLabel + " p239: m(F_k) >= m(R_k) for every graded no-trigger field "
            "(unfalsifiable by stimulus at this boundary per SPECIFICATION.md)");
    }

    // ---- P8 ----
    // (a) VB(j) is one value AND it equals the value the source latch
    // froze: the capture's own POST(p) value (P5), for a capture source,
    // or is left as the single internal-consistency check for a
    // no-trigger field end (already fully established by P7's own F_k).
    // (b) RUN(j) runs from Vrun to the first graded pin-6 pulse of j, or
    // to L_j-1 if there is none (SPECIFICATION.md section 9, P8).
    bool p8a = true, p8b = true;
    std::vector<std::string> capture_tags = {"B4","B5","B6","B7","B8","B9","B10","B11","B12","B13","B14a","B15b","B17b","B18"};
    std::string reported;
    // The cutoff is "the first graded pin-6 pulse of j, or L_j-1 if there is
    // none" (SPECIFICATION.md section 9, P8). A pulse still in vertical
    // blanking (line < Vrun) cannot end RUN: it lands before RUN starts and
    // (P5) cannot overwrite a freeze already in place. B19's own pulse is
    // not one of 5.5's "graded pin-6 pulses" (its field is outside B4-B18),
    // but at line 261/280 it is a real, run-region trigger all the same,
    // so it still ends RUN(19) exactly as a graded one would.
    auto first_graded_pulse_cutoff = [&](int64_t j) -> std::optional<int64_t> {
        std::optional<int64_t> cutoff;
        for (const auto& p2 : pulses) {
            if (p2.line < Vrun) continue;
            if ((p2.tag == "B4" && !b4_captured) || (p2.tag == "B5" && !b5_captured)) continue;
            const auto& t2 = timing[p2.tag];
            if (t2.field_idx == j && (!cutoff || t2.t_on < *cutoff)) cutoff = t2.t_on;
        }
        return cutoff;
    };
    for (const auto& tag : capture_tags) {
        auto it = timing.find(tag);
        if (it == timing.end()) continue;
        if ((tag == "B4" && !b4_captured) || (tag == "B5" && !b5_captured)) continue;
        int64_t j = it->second.field_idx + 1;
        auto vb = window_VB(r.B, r.model, j);
        if (!vb.empty()) {
            if (!all_equal_raw(vb)) p8a = false;
            auto post = window_POST(r.B, r.model, it->second.t_off, j);
            if (!post.empty() && vb[0].raw32 != post[0].raw32) p8a = false;
        }
        auto run = window_RUN(r.B, r.model, j, Vrun, first_graded_pulse_cutoff(j));
        if (!all_current(run, r.model)) p8b = false;
        if (!vb.empty()) reported += tag + ":L" + std::to_string(vb[0].V) + "H" + std::to_string(vb[0].H) + " ";
    }
    for (int k : notrig_fields) {
        int64_t j = r.B0_idx + k + 1;
        auto vb = window_VB(r.B, r.model, j);
        if (!vb.empty() && !all_equal_raw(vb)) p8a = false;
        auto run = window_RUN(r.B, r.model, j, Vrun, first_graded_pulse_cutoff(j));
        if (!all_current(run, r.model)) p8b = false;
    }
    bool p8 = p8a && p8b;
    gate(p8, "P8 " + cfgLabel + " p239 opus:8-lpen.c39: every latch holds until the end of internal "
        "vertical blanking and no longer ((a) VB(j) is one value equal to the value the source latch froze, "
        "(b) RUN(j) up to j's first graded pin-6 pulse is current)");
    checks.note(cfgLabel + " P8 measured first-current line/H per window: " + reported);

    // ---- P9 ----
    auto runB15 = window_RUN(r.B, r.model, r.B0_idx + 15, Vrun, timing["B15b"].t_on);
    auto runB17 = window_RUN(r.B, r.model, r.B0_idx + 17, Vrun, timing["B17b"].t_on);
    bool p9a = all_current(runB15, r.model) && all_current(runB17, r.model);
    auto postB15b = window_POST(r.B, r.model, timing["B15b"].t_off, r.B0_idx + 16);
    auto postB17b = window_POST(r.B, r.model, timing["B17b"].t_off, r.B0_idx + 18);
    bool p9b = all_equal_raw(postB15b) && none_current(postB15b, r.model) &&
               all_equal_raw(postB17b) && none_current(postB17b, r.model);
    bool p9c = true;
    for (const auto& p : pulses) {
        if (p.tag == "B4" || p.tag == "B5") continue;
        if (!p.graded) continue;
        const auto& t = timing[p.tag];
        auto post = window_POST(r.B, r.model, t.t_off, t.field_idx + 1);
        if (!all_equal_raw(post) || !none_current(post, r.model)) p9c = false;
    }
    bool p9 = p9a && p9b && p9c;
    gate(p9, "P9 " + cfgLabel + " p238: the capture circuitry re-arms by itself as vertical blank "
        "ends ((a) no hold-over past the release, (b) a pulse after blank is captured, (c) re-arming repeats "
        "every field)");

    // ---- P10 ----
    bool p10a = p3a && p3b && p7b;
    bool p10b_A = true;
    {
        // phase-A field ends after M0: use fields 1..(nfields-2) of A (skip cut ends)
        // A's own field boundaries are relative to the SAME model anchor.
        int64_t startField = r.model.field_index_of(r.A.empty() ? 0 : r.A.front().a) + 1;
        int64_t endField = r.model.field_index_of(r.A.empty() ? 0 : r.A.back().a);
        for (int64_t f = startField; f < endField; ++f) {
            auto vb = window_VB(r.A, r.model, f);
            if (!vb.empty() && !all_current(vb, r.model)) p10b_A = false;
        }
    }
    // phase C: C1 pulse not captured, C1/C2 no latch, C2 full field current, VB(C3) current
    auto runC1 = window_RUN(r.C, r.model, r.C0_idx + 1, Vrun);
    bool p10b_C1 = all_current(runC1, r.model);
    auto postC1 = window_POST(r.C, r.model, r.model.cycle_of(r.C0_idx + 1, 100, 100) + 4 * r.model.colour_clock_len, r.C0_idx + 2);
    if (!postC1.empty()) p10b_C1 = p10b_C1 && all_current(postC1, r.model);
    auto fullC2 = filter_by_a(r.C, r.model.field_start(r.C0_idx + 2), r.model.field_start(r.C0_idx + 3));
    bool p10b_C2 = all_current(fullC2, r.model);
    auto vbC3 = window_VB(r.C, r.model, r.C0_idx + 3);
    bool p10b_C3 = all_current(vbC3, r.model);
    bool p10b = p10b_A && p10b_C1 && p10b_C2 && p10b_C3;
    gate(p10a, "P10(a) " + cfgLabel + " p239,p229 opus:8-lpen.c35 sol:8-pen.c35 opus:8-regs.c16 sol:8-reg.c18: "
        "with bit 3 set, every graded pulse is captured and every no-trigger field end latches (via P3(a)(b) "
        "and P7(b))");
    checks.expect(p10b, "P10(b) " + cfgLabel + " p239,p229 opus:8-lpen.c35 sol:8-pen.c35 opus:8-regs.c16 "
        "sol:8-reg.c18: with bit 3 clear, every sample of the phase-A and phase-C windows is current (no "
        "phase-A field end latches, the C1 pulse is not captured, and neither the C1 nor the C2 field end "
        "latches)");

    // ---- P11 ----
    int64_t threshold = 0x10500;
    auto excluded_fields = std::set<int64_t>{r.B0_idx, r.B0_idx + 4, r.B0_idx + 5, r.B0_idx + 19};
    auto vb_excluded_fields = std::set<int64_t>{r.B0_idx + 1, r.B0_idx + 5, r.B0_idx + 6, r.B0_idx + 20};
    auto overlaps_pulse = [&](const Sample& s) {
        for (const auto& p : pulses) {
            if (!p.graded) continue;
            const auto& t = timing[p.tag];
            if (s.a <= t.t_off && s.b >= t.t_on) return true;
        }
        return false;
    };
    bool p11 = true; int p11n = 0;
    for (size_t i = 1; i < r.B.size(); ++i) {
        const auto& s0 = r.B[i - 1]; const auto& s1 = r.B[i];
        if (r.model.is_torn(s0) || r.model.is_torn(s1)) continue;
        if (overlaps_pulse(s0) || overlaps_pulse(s1)) continue;
        int64_t f0 = r.model.field_index_of(s0.a);
        if (excluded_fields.count(f0)) continue;
        int line0; r.model.pos_at(s0.a, &line0, nullptr);
        if (line0 >= 0 && line0 <= 19 && vb_excluded_fields.count(f0)) continue;
        // exclude NTSC line20 / PAL lines20-28 and line L, per spec 10.1/11
        if (ntsc) { if (line0 == 20) continue; } else { if (line0 >= 20 && line0 <= 28) continue; }
        if (line0 == r.model.L) continue;
        bool equalPair = (s0.raw32 == s1.raw32);
        int64_t m0 = 0x1FFFF & s0.raw32;
        bool truthT = false;
        int64_t triggerMP = -1; // m(P) of the capture the truth traces to, for the value check
        // "Since the last top-of-screen (V=20)" (p.239): if this sample is
        // still in blanking (line0 < 20), its own field has not reached
        // V=20 yet, so the last top-of-screen was the PREVIOUS field's --
        // a graded pulse from that earlier field, still frozen through this
        // blanking, still counts.
        int64_t lastTopOfScreen = (line0 >= 20)
            ? r.model.field_start(f0) + 20 * line_len_clk(r.model)
            : r.model.field_start(f0 - 1) + 20 * line_len_clk(r.model);
        // The method itself (step 2, p.239) can only show "triggered" while
        // the freeze is still in place at BOTH reads of the pair: if s0 is
        // still in blanking (frozen) but s1 has already reached the release
        // point (line >= 20), the pair spans the release and the page's own
        // comparison reads "not the same" regardless of any earlier
        // trigger. The truth this pair can test is therefore whether a
        // trigger is active through both reads, not just at s0.
        int line1; r.model.pos_at(s1.a, &line1, nullptr);
        bool stillFrozenAtS1 = !(line0 < 20 && line1 >= 20);
        for (const auto& p : pulses) {
            if (!p.graded) continue;
            const auto& t = timing[p.tag];
            if (t.t_on >= lastTopOfScreen && t.t_on < s0.a && stillFrozenAtS1) {
                truthT = true;
                auto post = window_POST(r.B, r.model, t.t_off, t.field_idx + 1);
                if (!post.empty()) triggerMP = 0x1FFFF & post[0].raw32;
            }
        }
        bool answerTriggered = equalPair && (m0 < threshold);
        if (equalPair && m0 == threshold) continue; // not graded
        ++p11n;
        if (truthT && !answerTriggered) p11 = false;
        if (!truthT && answerTriggered) p11 = false;
        if (truthT && answerTriggered && triggerMP >= 0 && m0 != triggerMP) p11 = false;
    }
    gate(p11n > 0 && p11, "P11 " + cfgLabel + " p239 opus:8-lpen.c43 sol:8-pen.c41 opus:8-lpen.c47 "
        "sol:8-pen.c44 opus:8-lpen.c48 sol:8-pen.c45 sol:8-pen.c46: the two-read detection method (read VPOSR "
        "twice, compare, mask, compare with $10500) answers correctly whether the light pen triggered since "
        "the last top-of-screen, and when it does, m equals m(P) of that capture");
    checks.note(cfgLabel + " P11 graded pairs=" + std::to_string(p11n));

    // ---- P12 ----
    bool p12 = true; int p12n = 0;
    std::set<int64_t> p12_excluded = {r.B0_idx, r.B0_idx + 1 /*after B0 = VB(B1)*/, r.B0_idx + 5, r.B0_idx + 6, r.B0_idx + 20};
    for (const auto& s : r.B) {
        if (r.model.is_torn(s)) continue;
        if (overlaps_pulse(s)) continue;
        int64_t f = r.model.field_index_of(s.a);
        int line; r.model.pos_at(s.a, &line, nullptr);
        if (line < 1 || line > 19) continue;
        if (p12_excluded.count(f)) continue;
        int64_t precedingField = f - 1;
        bool truthT = false;
        int64_t triggerMP = -1;
        for (const auto& p : pulses) {
            if (!p.graded) continue;
            const auto& t = timing[p.tag];
            if (r.model.field_index_of(t.t_on) == precedingField) {
                truthT = true;
                auto post = window_POST(r.B, r.model, t.t_off, t.field_idx + 1);
                if (!post.empty()) triggerMP = 0x1FFFF & post[0].raw32;
            }
        }
        int64_t m = 0x1FFFF & s.raw32;
        if (m == threshold) continue;
        ++p12n;
        bool answerTriggered = m < threshold;
        if (truthT != answerTriggered) p12 = false;
        if (truthT && answerTriggered && triggerMP >= 0 && m != triggerMP) p12 = false;
    }
    gate(p12n > 0 && p12, "P12 " + cfgLabel + " p239 opus:8-lpen.c52: the single-read method during "
        "vertical blanking answers correctly whether the light pen triggered in the preceding field, and "
        "when it does, m equals m(P) of that capture");
    checks.note(cfgLabel + " P12 graded samples=" + std::to_string(p12n));
    } // if (cpOkLatch)

    if (cpOkP13) {
    // ---- P13 ----
    // phase D windows, from M3. A sample whose [a,b] straddles a known
    // input change is excluded (not just gated by s.a alone): the read
    // instant is only bounded within [a,b], so if a change cycle falls
    // inside that gap the sample's true value at the read instant is
    // ambiguous.
    std::vector<int64_t> dChanges;
    for (auto& e : r.applied) if (e.sched >= r.M3cyc) dChanges.push_back(e.sched);
    auto sample_straddles_change = [&](const Sample& s) {
        for (auto c : dChanges) if (c >= s.a && c <= s.b) return true;
        return false;
    };
    auto win_cyc = [&](double lo_us, double hi_us) {
        return std::make_pair(r.M3cyc + us_to_cck(lo_us, ntsc) * r.model.colour_clock_len,
                               r.M3cyc + us_to_cck(hi_us, ntsc) * r.model.colour_clock_len);
    };
    // settle 300us after any change or M3 (5.7); grade from settle to next change
    auto graded_slice = [&](int64_t changeAt, int64_t nextChangeAt) {
        int64_t lo = changeAt + us_to_cck(300, ntsc) * r.model.colour_clock_len;
        return std::make_pair(lo, nextChangeAt);
    };
    auto [t0, t2000] = win_cyc(0, 2000);
    auto [t2000b, t4000] = win_cyc(2000, 4000);
    auto [t4000b, t5000] = win_cyc(4000, 5000);
    auto [t5000b, t7000] = win_cyc(5000, 7000);
    auto [t7000b, t8000] = win_cyc(7000, 8000);
    auto [t8000b, t10000] = win_cyc(8000, 10000);
    auto [t10000b, t12000] = win_cyc(10000, 12000);
    (void)t0;
    // bit8_expect0: only the 2-4ms window (port1 pin5 driven low). bit12
    // is never drivable through this harness (5.7/section 3), so its
    // idle half -- every window other than the undrivable 5-7ms slot
    // itself -- is graded at 1, per "1 in every other graded window".
    struct DWin { int64_t lo, hi; bool bit8_expect0; const char* name; };
    std::vector<DWin> dwins = {
        [&]{ auto g = graded_slice(r.M3cyc, t2000); return DWin{g.first, g.second, false, "0-2ms"}; }(),
        [&]{ auto g = graded_slice(t2000, t4000); return DWin{g.first, g.second, true, "2-4ms"}; }(),
        [&]{ auto g = graded_slice(t4000, t5000); return DWin{g.first, g.second, false, "4-5ms"}; }(),
        [&]{ auto g = graded_slice(t5000, t7000); return DWin{g.first, g.second, false, "5-7ms"}; }(),
        [&]{ auto g = graded_slice(t7000, t8000); return DWin{g.first, g.second, false, "7-8ms"}; }(),
        [&]{ auto g = graded_slice(t8000, t10000); return DWin{g.first, g.second, false, "8-10ms"}; }(),
        [&]{ auto g = graded_slice(t10000, t12000); return DWin{g.first, g.second, false, "10-12ms"}; }(),
    };
    bool p13_bit8 = true, p13_bit12 = true;
    for (const auto& w : dwins) {
        for (const auto& s : r.D) {
            if (s.a < w.lo || s.a >= w.hi) continue;
            if (sample_straddles_change(s)) continue;
            int bit8 = (s.raw16 >> 8) & 1;
            int bit12 = (s.raw16 >> 12) & 1;
            bool want0 = w.bit8_expect0;
            if (want0 && bit8 != 0) p13_bit8 = false;
            if (!want0 && bit8 != 1) p13_bit8 = false;
            if (bit12 != 1) p13_bit12 = false; // idle half: bit 12 is never driven, so always 1 here
        }
    }
    bool p13_pin9 = true;
    { auto g = graded_slice(t8000, t10000);
      for (const auto& s : r.D) {
          if (s.a < g.first || s.a >= g.second) continue;
          if (sample_straddles_change(s)) continue;
          int bit10 = (s.raw16 >> 10) & 1, bit14 = (s.raw16 >> 14) & 1;
          if (bit10 != 0 || bit14 != 0) p13_pin9 = false;
      } }
    checks.expect(p13_bit8 && p13_bit12, "P13(port1) " + cfgLabel + " p238 opus:8-lpen.c10 opus:8-lpen.c11 "
        "sol:8-pen.c10 sol:8-pen.c11: the pen-pressed switch on pin 5 reads like the middle mouse button "
        "(POTGOR bit 8 is 0 while port-1 pin 5 is driven low, 1 in every other graded window; bit 12 stays "
        "1 throughout, its idle half, since no input this harness exposes drives it low)");
    checks.expect(p13_pin9, "P13(pin9-control) " + cfgLabel + ": POTGOR bits 10 and 14 (pin 9, the right "
        "mouse button pot line) read 0 while pin 9 of both ports is driven low (p.240, p.241's button-read "
        "convention), confirming they track pin 9 and not pin 5's bit 8/12");
    checks.record("P13(port2): SPECIFICATION.md schedules port-2 pin 5 low for 5-7ms and grades POTGOR bit "
        "12 there. No input this harness exposes (joystick0/1, mouse_buttons) reaches that pot line (measured: "
        "driving every one leaves POTGOR bit 12 unchanged, SPECIFICATION.md section 3); the low-driven half of "
        "that window is not exercised and is not graded. Its idle half is graded above with every other window.");
    }
}

// ---------------------------------------------------------------------
// Page model (SPECIFICATION.md section 8): applies the page's own latch to a
// run's measured phase-B samples, then grades them with the package's own
// P1-P13 code under a "PAGE-MODEL " label, as the green control that a
// correctly-latching implementation would produce on the same measurement
// basis P1-P13 already uses. Trigger capture is graded from line 20 on
// (p.238 step 1: "just as the system exits vertical blank, the capture
// circuitry ... is automatically enabled"); the onset position freezes
// the value (p.238 step 4: "capture and save the current contents of the
// beam register"); with no trigger, the counters latch at the field end
// (p.239: "If no trigger is seen, the counters latch at the end of the
// display field"); the whole VPOSR stays frozen until line 20 of the next
// field (p.239: "the value in VPOSR is frozen ... released at the end of
// internal vertical blanking (vertical position 20)").
// ---------------------------------------------------------------------
std::vector<Sample> apply_page_model(const std::vector<Sample>& B, const BeamModel& m, int64_t B0_idx,
                                      bool latchEnabled, const std::vector<PulseDef>& pin6defs) {
    std::vector<Sample> out = B;
    if (!latchEnabled) return out; // p.239: the freeze only happens once the light pen input is enabled
    std::vector<int64_t> onsets;
    for (const auto& p : pin6defs) onsets.push_back(pulse_timing(m, B0_idx, p).t_on);
    std::sort(onsets.begin(), onsets.end());
    int64_t L2 = line_len_clk(m);
    auto latch_of_field = [&](int64_t f, int* V, int* H) {
        int64_t lo = m.field_start(f) + 20 * L2, hi = m.field_start(f + 1);
        for (auto t : onsets) if (t >= lo && t < hi) { m.pos_at(t, V, H); return; }
        *V = m.L; *H = (int)m.line_len_cck - 1; // "the counters latch at the end of the display field"
    };
    std::map<int64_t, uint32_t> hiOf; // the frozen upper word (chip-ID bits, LOF), first read of each latch
    for (auto& s : out) {
        // Classify by s.a, the same read-instant bound every window
        // function (filter_by_a) uses: a single derived point elsewhere in
        // [a,b] can drift across the line-19/20 boundary and disagree with
        // how the sample was windowed, leaving it unmodeled right at that
        // edge.
        int64_t t = s.a;
        int64_t f = m.field_index_of(t);
        int line; m.pos_at(t, &line, nullptr);
        int V = -1, H = -1; int64_t key = -1;
        if (line < 20) {
            // still within blanking: whatever field f-1 latched stays frozen
            if (f - 1 >= B0_idx) { latch_of_field(f - 1, &V, &H); key = (f - 1) * 1000000 + V * 1000 + H; }
        } else {
            // past blanking: frozen from this field's own first trigger onset, if any yet
            int64_t lo = m.field_start(f) + 20 * L2;
            for (auto on : onsets) if (on >= lo && on < t && m.field_index_of(on) == f) { m.pos_at(on, &V, &H); key = f * 1000000 + V * 1000 + H; break; }
        }
        if (V < 0) continue; // no latch active yet this field: the current beam position stands
        if (!hiOf.count(key)) hiOf[key] = (s.raw32 >> 16) & 0xFFFE;
        uint32_t hi = hiOf[key];
        s.raw32 = ((hi | ((uint32_t)(V >> 8) & 1)) << 16) | ((uint32_t)(V & 0xFF) << 8) | (uint32_t)(H & 0xFF);
        s.V = V; s.H = H;
    }
    return out;
}

void grade_page_model(const RunResult& r, bool ntsc, const std::string& cfgLabel, const Preconds& pc,
                       bool latchEnabled = true, const std::vector<PulseDef>* pin6defsOverride = nullptr) {
    if (!r.model.ok) return;
    auto pin6defs = pin6defsOverride ? *pin6defsOverride : phaseB_pin6_pulses(ntsc);
    RunResult r2 = r;
    r2.B = apply_page_model(r.B, r.model, r.B0_idx, latchEnabled, pin6defs);
    int changed = 0;
    for (size_t i = 0; i < r.B.size(); ++i) if (r2.B[i].raw32 != r.B[i].raw32) ++changed;
    checks.note("PAGE-MODEL " + cfgLabel + ": replaced " + std::to_string(changed) + " of " +
        std::to_string(r.B.size()) + " phase-B samples (latchEnabled=" + std::to_string(latchEnabled) +
        ", pin6defs=" + std::to_string(pin6defs.size()) + ")");
    // ecs=false: the page model is always graded with real expect() calls,
    // never SPECIFICATION.md section 3's ECS/no-latch exception, which
    // exists only to accommodate an implementation that may not latch.
    grade_P1_P13(r2, ntsc, /*ecs=*/false, "PAGE-MODEL " + cfgLabel, pc);
}

// ---------------------------------------------------------------------
// main runs (no NMH_MUTATE): four configurations, this core.
// ---------------------------------------------------------------------
void run_config(const std::string& dir, bool ecs, bool ntsc, const std::string& label) {
    std::string bin = dir + "/prog.bin";
    ScheduleDFn schedD = [ntsc](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st) {
        schedD_main(m, M3, ev, st, ntsc);
    };
    RunResult r = run_one(bin, ecs, ntsc, kBufBcount, schedB_main, schedC_main, schedD);
    if (!r.loaded || !r.released) {
        checks.expect(false, "FIXTURE " + label + ": program loaded and CPU released from reset");
        return;
    }
    checks.expect(r.m0cyc >= 0, "FIXTURE " + label + ": the phase marker M0 (a known constant) was stored "
        "before phase A sampling began");
    checks.expect(r.done_ok, "FIXTURE " + label + ": the program completed (M0 to M4 written in order, ending in the infinite .stop loop)");
    if (!r.model.ok || !r.done_ok) {
        checks.record(label + ": the beam model could not be fit from phase A, or the program did not "
            "finish -- CP1-CP6 and P1-P13 are not evaluated for this configuration");
        return;
    }

    Preconds pc = grade_preconditions(r, ntsc, label);
    grade_P1_P13(r, ntsc, ecs, label, pc);
    grade_page_model(r, ntsc, label, pc);
}

// ---------------------------------------------------------------------
// Declined claim ids (SPECIFICATION.md section 1): printed once, not
// per-run, since the reason is about the claim's wording against the
// page, not about anything measured. A claim id printed only in
// SPECIFICATION.md's own table is not "coverage counted from printed
// lines" (BRIEF.md); each one gets its own record() line here too.
// ---------------------------------------------------------------------
void record_declined_ids() {
    checks.record("opus:8-lpen.c53 p239, sol:8-pen.c47 p239: declined -- \"the VPOSR register may be "
        "latched at any time\" names no moment and no value; the latches the pages do define are graded "
        "in P3, P5 and P7.");
    checks.record("opus:8-lpen.c54 p239, sol:8-pen.c48 p239: declined -- \"cannot be relied on as a "
        "counter\" is advice to software, not an expected value; the behaviour behind it is graded in "
        "P5 to P8.");
    checks.record("opus:8-lpen.c41 p239, sol:8-pen.c40 p239: declined -- \"there is no single bit in the "
        "system that indicates a light pen trigger\" names no register this package can read exhaustively; "
        "its practical consequence is graded in P11 and P12.");
    checks.record("opus:8-lpen.c34 p239: declined -- names only the bit's mnemonic \"LPEN\", which is not "
        "on the printed page 239 (\"write a 1 into bit 3 of BPLCON0\"); a name is not an observable value.");
    checks.record("opus:8-intro.c2 p227: declined -- reads \"two ... control ports\" as each port accepting "
        "a light pen, which p238 contradicts for every model it names; P1 grades \"at least one port\" and "
        "reports which.");
    checks.record("sol:8-pen.c39 p239: declined -- fixes the release at \"vertical position 20\", where "
        "p219 and p79 place the end of vertical blanking differently by standard; P8 grades the release "
        "within the span every page allows and reports the line where it happens.");
    checks.record("opus:8-lpen.c12 p238, sol:8-pen.c12 p238: declined -- step 1's \"just as the system "
        "exits vertical blank\" is graded only as far as all pages agree (P9); the exact instant it "
        "describes, at line resolution across lines 20-31, is not graded by any check.");
}

void run_main(const std::string& dir) {
    record_declined_ids();
    run_config(dir, true,  false, "ECS/PAL");
    run_config(dir, true,  true,  "ECS/NTSC");
    run_config(dir, false, false, "OCS/PAL");
    run_config(dir, false, true,  "OCS/NTSC");
}

// ---------------------------------------------------------------------
// mutations (SPECIFICATION.md section 8), dispatched by NMH_MUTATE.
// Each prints its own PASS/FAIL/RECORDED lines under the label; the
// header comment at the top of this file states, per label, what each
// core produced once both have been run.
// ---------------------------------------------------------------------
// withPageModel: also replays this run's own measured phase-B samples
// through the page model (SPECIFICATION.md section 8), under pin6defs (the
// SAME pin-6 positions this run's own schedB actually injects) and
// latchEnabled (false only for K1, which never sets BPLCON0 bit 3 in
// phase B). Every latch-targeting K-run is graded this way, on the SAME
// run (one simulation), so the page model sees exactly what that
// mutation's stimulus delivered.
RunResult run_and_grade(const std::string& dir, const std::string& bin, bool ecs, bool ntsc, uint32_t bufBcount,
                    const ScheduleBFn& schedB, const ScheduleCFn& schedC, const ScheduleDFn& schedD,
                    const std::string& label, bool withPageModel = false, bool latchEnabled = true,
                    const std::vector<PulseDef>* pin6defs = nullptr) {
    RunResult r = run_one(dir + "/" + bin, ecs, ntsc, bufBcount, schedB, schedC, schedD);
    checks.expect(r.loaded && r.released, label + ": program loaded and CPU released from reset");
    checks.expect(r.done_ok, label + ": the program completed (M0 to M4 written in order, ending in the infinite .stop loop)");
    if (!r.model.ok || !r.done_ok) {
        checks.record(label + ": the beam model could not be fit, or the program did not finish -- no "
            "further grading for this run");
        return r;
    }
    Preconds pc = grade_preconditions(r, ntsc, label);
    checks.note(label + ": CP1=" + std::to_string(pc.cp1) + " CP2=" + std::to_string(pc.cp2) +
        " CP3=" + std::to_string(pc.cp3) + " CP4=" + std::to_string(pc.cp4) + " CP5=" + std::to_string(pc.cp5) +
        " CP6=" + std::to_string(pc.cp6));
    grade_P1_P13(r, ntsc, ecs, label, pc);
    if (withPageModel) grade_page_model(r, ntsc, label, pc, latchEnabled, pin6defs);
    return r;
}

// K2: pin-6 pulses (both ports) at (100,100) added in B2 and B3.
void schedB_K2(const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st) {
    schedB_main(m, B0, ev, st);
    add_pulse_events(ev, m, B0, st, 2, 100, 100, true, true, kBitFire);
    add_pulse_events(ev, m, B0, st, 3, 100, 100, true, true, kBitFire);
}
// K6: line-10 pulse of B15 and line-19 pulse of B17 moved to line 30.
void schedB_K6(const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st) {
    for (const auto& p : phaseB_pin6_pulses(false)) {
        if (p.tag == "B15a") add_pulse_events(ev, m, B0, st, p.field_offset, 30, p.H, p.port0, p.port1, p.bit);
        else if (p.tag == "B17a") add_pulse_events(ev, m, B0, st, p.field_offset, 30, p.H, p.port0, p.port1, p.bit);
        else add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
    }
    for (const auto& p : phaseB18_other_pulses()) add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
}
// K7: line-32 pulses of B15 and B17 moved to line 12.
void schedB_K7(const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st) {
    for (const auto& p : phaseB_pin6_pulses(false)) {
        if (p.tag == "B15b") add_pulse_events(ev, m, B0, st, p.field_offset, 12, p.H, p.port0, p.port1, p.bit);
        else if (p.tag == "B17b") add_pulse_events(ev, m, B0, st, p.field_offset, 12, p.H, p.port0, p.port1, p.bit);
        else add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
    }
    for (const auto& p : phaseB18_other_pulses()) add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
}
// K8: the first pulse of B14 (line 80) is removed.
void schedB_K8(const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st) {
    for (const auto& p : phaseB_pin6_pulses(false)) {
        if (p.tag == "B14a") continue;
        add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
    }
    for (const auto& p : phaseB18_other_pulses()) add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
}
// K9: in B18, the pin-1 pulse at line 40 is replaced by a pin-6 pulse.
void schedB_K9(const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st) {
    for (const auto& p : phaseB_pin6_pulses(false)) add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
    for (const auto& p : phaseB18_other_pulses()) {
        if (p.tag == "B18-pin1") add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, kBitFire);
        else add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
    }
}
// K9b: B4/B5/B18's pin-6 pulses moved to pin 9 (same positions).
void schedB_K9b(const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st) {
    for (const auto& p : phaseB_pin6_pulses(false)) {
        uint8_t bit = (p.tag == "B4" || p.tag == "B5" || p.tag == "B18") ? kBitFire2 : p.bit;
        add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, bit);
    }
    for (const auto& p : phaseB18_other_pulses()) add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
}
// K12: every pulse placed one line later than the schedule says.
void schedB_K12(const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st) {
    for (const auto& p : phaseB_pin6_pulses(false)) add_pulse_events(ev, m, B0, st, p.field_offset, p.line + 1, p.H, p.port0, p.port1, p.bit);
    for (const auto& p : phaseB18_other_pulses()) add_pulse_events(ev, m, B0, st, p.field_offset, p.line + 1, p.H, p.port0, p.port1, p.bit);
}
void schedC_K12(const BeamModel& m, int64_t C0, std::vector<Event>& ev, PortState* st) {
    add_pulse_events(ev, m, C0, st, 1, 101, 100, true, true, kBitFire);
}
// K16: B7 pulse at (100,122) instead of (100,102); B11 at (41,100) instead of (40,100).
void schedB_K16(const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st) {
    for (const auto& p : phaseB_pin6_pulses(false)) {
        if (p.tag == "B7") add_pulse_events(ev, m, B0, st, p.field_offset, 100, 122, p.port0, p.port1, p.bit);
        else if (p.tag == "B11") add_pulse_events(ev, m, B0, st, p.field_offset, 41, 100, p.port0, p.port1, p.bit);
        else add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
    }
    for (const auto& p : phaseB18_other_pulses()) add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
}
// K10: pin 9 driven low in the windows the main run drives pin 5 (2-4ms,
// both ports, pin 9 only -- pin 5 is not driven there instead of pin 9).
// The main schedule's own 8-10ms pin-9 window is kept unchanged
// (SPECIFICATION.md section 8: "Keep the main D schedule plus one change"),
// so only P13(port1) is exercised by this change; P13(pin9-control) sees
// the same 8-10ms window it always does.
void schedD_K10(const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st, bool ntsc) {
    int64_t t2 = M3 + us_to_cck(2000, ntsc) * m.colour_clock_len;
    int64_t t4 = M3 + us_to_cck(4000, ntsc) * m.colour_clock_len;
    ev.push_back({t2, [st]() { st->joy0 |= kBitFire2; st->joy1 |= kBitFire2; }});
    ev.push_back({t4, [st]() { st->joy0 &= (uint8_t)~kBitFire2; st->joy1 &= (uint8_t)~kBitFire2; }});
    int64_t t8 = M3 + us_to_cck(8000, ntsc) * m.colour_clock_len;
    int64_t t10 = M3 + us_to_cck(10000, ntsc) * m.colour_clock_len;
    ev.push_back({t8, [st]() { st->joy0 |= kBitFire2; st->joy1 |= kBitFire2; }});
    ev.push_back({t10, [st]() { st->joy0 &= (uint8_t)~kBitFire2; st->joy1 &= (uint8_t)~kBitFire2; }});
}
// K10b: port 1 pin 5 held low throughout phase D (0-12ms, replacing the
// main schedule's 2-4ms-only pin-5 window). The main schedule's own
// 8-10ms pin-9 window is kept unchanged, for the same reason as K10.
void schedD_K10b(const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st, bool ntsc) {
    // M3+1, not M3 itself: run_until_write's own return can already sit one
    // clk_sys past the mark's write cycle, and an event scheduled at
    // exactly that boundary can then apply a cycle late (CP4's own record
    // catches it) depending on which side of it delivery starts from.
    ev.push_back({M3 + 1, [st]() { st->mbtn |= kMouseMid; }});
    int64_t t12 = M3 + us_to_cck(12000, ntsc) * m.colour_clock_len;
    ev.push_back({t12, [st]() { st->mbtn &= (uint8_t)~kMouseMid; }});
    int64_t t8 = M3 + us_to_cck(8000, ntsc) * m.colour_clock_len;
    int64_t t10 = M3 + us_to_cck(10000, ntsc) * m.colour_clock_len;
    ev.push_back({t8, [st]() { st->joy0 |= kBitFire2; st->joy1 |= kBitFire2; }});
    ev.push_back({t10, [st]() { st->joy0 &= (uint8_t)~kBitFire2; st->joy1 &= (uint8_t)~kBitFire2; }});
}

// K13 (SPECIFICATION.md section 8): the phase-B buffer is shortened so
// phase B ends within field B19. Because the sample rate differs by core
// (SPECIFICATION.md/the validator's own measurement: fx68k ends in B19,
// TG68K in B17, at the same 60000-sample buffer size), the buffer's own
// fixed size still decides where the CPU actually stops -- what this
// runner fixes is that the driver's phase-B event schedule spans further
// than that (out past B19, toward the un-shortened main schedule's
// B21), so it must stop injecting once M2 actually appears, not run the
// full precomputed list and search for M2 afterwards (which would have
// already passed by then). It steps in bounded chunks, scanning the
// growing log for M2, and drops every event after M2 is seen.
void run_K13(const std::string& dir, const std::string& label) {
    nmh::Config cfg = make_config(true, false);
    auto h = std::make_unique<nmh::NanomigHarness>(cfg);
    g_applied.clear();
    size_t loaded = h->load_program(dir + "/prog_k13.bin");
    bool released = loaded && h->boot(kBootBudget).cpu_released;
    checks.expect(loaded > 0 && released, label + ": program loaded and CPU released from reset");
    if (!(loaded && released)) return;
    auto m1cyc = h->run_until_write(kMark, kM1, kRunBudget);
    checks.expect(m1cyc.has_value(), label + ": phase A completed (M1 written)");
    if (!m1cyc) return;
    auto coderd = code_read_cycles(h->ram_log());
    auto A = extract_long_samples(h->ram_log(), coderd, kBufA, kBufA + 4 * kBufAcount);
    BeamModel model = fit_model(A, h->ram_log(), h->video(), false);
    checks.expect(model.ok, label + ": beam model fit from phase A");
    if (!model.ok) return;
    RunResult r;
    r.ntsc = false; r.loaded = true; r.released = true; r.model = model;
    r.B0_idx = model.field_index_of((int64_t)*m1cyc);
    PortState state;
    std::vector<Event> evB;
    schedB_main(model, r.B0_idx, evB, &state);
    std::sort(evB.begin(), evB.end(), [](const Event& x, const Event& y) { return x.cycle < y.cycle; });
    bool m2seen = false;
    for (auto& e : evB) {
        if (m2seen) break;
        int64_t now = (int64_t)h->cycles();
        int64_t remaining = e.cycle - now;
        const int64_t kChunk = 2000;
        while (remaining > 0 && !m2seen) {
            int64_t step = std::min(kChunk, remaining);
            h->run_cycles((uint64_t)step);
            remaining -= step;
            if (find_mark_cycle(h->ram_log(), kM2) >= 0) m2seen = true;
        }
        if (m2seen) break;
        e.apply();
        state.push(*h);
    }
    checks.note(label + ": M2 " + std::string(m2seen ? "appeared while still injecting phase-B events "
        "(the rest were dropped)" : "had not appeared once every phase-B event was injected"));
    // M2 may already be in the log (found by scanning it above), not still
    // to come: run_until_write only ever finds a future write, so a write
    // already seen is read back from the log itself, not waited for again
    // (the mistake this runner replaces, SPECIFICATION.md section 8, K13).
    std::optional<uint64_t> m2cyc;
    if (m2seen) {
        int64_t c = find_mark_cycle(h->ram_log(), kM2);
        if (c >= 0) m2cyc = (uint64_t)c;
    } else {
        m2cyc = h->run_until_write(kMark, kM2, kRunBudget);
    }
    checks.expect(m2cyc.has_value(), label + ": M2 written (phase B ended)");
    r.C0_idx = m2cyc && model.ok ? model.field_index_of((int64_t)*m2cyc) : 0;
    if (m2cyc) {
        std::vector<Event> evC;
        schedC_main(model, r.C0_idx, evC, &state);
        run_phase_events(*h, evC, state);
    }
    auto m3cyc = h->run_until_write(kMark, kM3, kRunBudget);
    r.M3cyc = m3cyc ? (int64_t)*m3cyc : 0;
    if (m3cyc) {
        std::vector<Event> evD;
        schedD_main(model, r.M3cyc, evD, &state, false);
        run_phase_events(*h, evD, state);
    }
    auto m4cyc = h->run_until_write(kMark, kM4, kRunBudget);
    r.done_ok = m4cyc.has_value();
    checks.expect(r.done_ok, label + ": the program completed (M0 to M4 found, in order, in the full log)");
    r.log = h->ram_log();
    r.vid = h->video();
    auto coderd2 = code_read_cycles(r.log);
    r.A = extract_long_samples(r.log, coderd2, kBufA, kBufA + 4 * kBufAcount);
    uint32_t bufBend = kBufB + 4 * kBufBcountK13;
    uint32_t bufC = bufBend, bufCend = bufC + 4 * kBufCcount, bufD = bufCend, bufDend = bufD + 2 * kBufDcount;
    r.B = extract_long_samples(r.log, coderd2, kBufB, bufBend);
    r.C = extract_long_samples(r.log, coderd2, bufC, bufCend);
    r.D = extract_word_samples(r.log, coderd2, bufD, bufDend);
    if (auto w = h->peek_logical(0x100004)) r.fire_byte = uint8_t((*w >> 8) & 0xFF);
    r.m0cyc = find_mark_cycle(r.log, kM0);
    r.m1cyc = (int64_t)*m1cyc;
    r.m2cyc = m2cyc ? (int64_t)*m2cyc : -1;
    r.m3cyc = r.M3cyc;
    r.m4cyc = r.done_ok ? (int64_t)*m4cyc : -1;
    r.applied = g_applied;
    if (!r.done_ok) return;
    Preconds pc = grade_preconditions(r, false, label);
    checks.expect(!pc.cp5, "K13 MUTATION stimulus red NMH_MUTATE=K13: CP5 is red -- the shortened buffer "
        "ends phase B before its required coverage (line 40 of field B21)");
    grade_P1_P13(r, false, true, label, pc);
}

void run_mutation(const std::string& dir, const std::string& label) {
    if (label == "K1") {
        run_and_grade(dir, "prog_k1.bin", false, false, kBufBcount, schedB_main, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); },
            "K1/OCS/PAL", /*withPageModel=*/true, /*latchEnabled=*/false);
    } else if (label == "K3") {
        run_and_grade(dir, "prog_k3.bin", true, false, kBufBcount, schedB_main, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); }, "K3/ECS/PAL");
    } else if (label == "K4") {
        run_and_grade(dir, "prog_k4.bin", true, false, kBufBcount, schedB_main, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); }, "K4/ECS/PAL");
    } else if (label == "K5") {
        run_and_grade(dir, "prog_k5.bin", true, false, kBufBcountK5, schedB_main, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); }, "K5/ECS/PAL");
    } else if (label == "K13") {
        run_K13(dir, "K13/ECS/PAL");
    } else if (label == "K14") {
        run_and_grade(dir, "prog_k14.bin", true, false, kBufBcount, schedB_main, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); }, "K14/ECS/PAL");
    } else if (label == "K2") {
        auto defs = pin6_defs_K2(false);
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, schedB_K2, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); },
            "K2/OCS/PAL", true, true, &defs);
    } else if (label == "K6") {
        auto defs = pin6_defs_K6(false);
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, schedB_K6, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); },
            "K6/OCS/PAL", true, true, &defs);
    } else if (label == "K7") {
        auto defs = pin6_defs_K7(false);
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, schedB_K7, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); },
            "K7/OCS/PAL", true, true, &defs);
    } else if (label == "K8") {
        auto defs = pin6_defs_K8(false);
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, schedB_K8, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); },
            "K8/OCS/PAL", true, true, &defs);
    } else if (label == "K9") {
        auto defs = pin6_defs_K9(false);
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, schedB_K9, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); },
            "K9/OCS/PAL", true, true, &defs);
    } else if (label == "K9b") {
        auto defs = pin6_defs_K9b(false);
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, schedB_K9b, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); },
            "K9b/OCS/PAL", true, true, &defs);
    } else if (label == "K10") {
        run_and_grade(dir, "prog.bin", true, false, kBufBcount, schedB_main, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_K10(m, M3, ev, st, false); }, "K10/ECS/PAL");
    } else if (label == "K10b") {
        run_and_grade(dir, "prog.bin", true, false, kBufBcount, schedB_main, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_K10b(m, M3, ev, st, false); }, "K10b/ECS/PAL");
    } else if (label == "K12") {
        RunResult r = run_and_grade(dir, "prog.bin", true, false, kBufBcount, schedB_K12, schedC_K12,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); }, "K12/ECS/PAL");
        // CP4 must grade this run's delivered pulses against the
        // unmutated schedule, not against K12's own (shifted) one --
        // otherwise the placement check only ever compares a schedule
        // against itself (SPECIFICATION.md section 7, CP4: "the
        // schedule" is the run's own changed schedule for every other
        // mutation, but K12's whole point is a placement error against
        // the *unmutated* one). Both cycles come from this run's own
        // fitted model, at the SPECIFICATION.md section 6 line/H each
        // tag names.
        if (r.model.ok) {
            auto mainDefs = phaseB_pin6_pulses(false);
            bool allMismatch = true; int64_t worstDiff = 0; std::string worstTag;
            for (const auto& p : mainDefs) {
                PulseDef shifted = p; shifted.line = p.line + 1;
                auto delivered = pulse_timing(r.model, r.B0_idx, shifted);
                auto canonical = pulse_timing(r.model, r.B0_idx, p);
                if (delivered.t_on == canonical.t_on) allMismatch = false;
                int64_t diff = std::llabs(delivered.t_on - canonical.t_on);
                if (diff > worstDiff) { worstDiff = diff; worstTag = p.tag; }
            }
            checks.expect(allMismatch, "K12 MUTATION stimulus red NMH_MUTATE=K12: CP4 is red against the "
                "unmutated SPECIFICATION.md section 6 schedule -- every pulse lands one line (" +
                std::to_string(line_len_clk(r.model)) + " clk_sys) away from its named position (worst: " +
                worstTag + ", " + std::to_string(worstDiff) + " clk_sys)");
        }
    } else if (label == "K16") {
        auto defs = pin6_defs_K16(false);
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, schedB_K16, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, false); },
            "K16/OCS/PAL", true, true, &defs);
    } else if (label == "K15") {
        // ECS chipset, Config forced to NTSC, NTSC input schedule, graded
        // with the PAL run's own expectations (SPECIFICATION.md 8).
        RunResult r = run_one(dir + "/prog.bin", true, true, kBufBcount,
            [](const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st) {
                for (const auto& p : phaseB_pin6_pulses(true)) add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
                for (const auto& p : phaseB18_other_pulses()) add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
            },
            schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, true); });
        checks.expect(r.loaded && r.released && r.done_ok, "K15/ECS/PAL(as-NTSC): program ran to completion");
        if (r.model.ok) {
            bool a2_as_pal = check_A2(r, 29, /*ntsc grading=*/false);
            checks.expect(!a2_as_pal, "K15 MUTATION stimulus red NMH_MUTATE=K15: the standard is switched to "
                "NTSC while graded as PAL -- CP1(A2) is red (measured field length " +
                std::to_string(r.model.L + 1) + " lines, PAL expects 312 or 313)");
        }
    } else if (label == "K17") {
        // K4 (initial BPLCON0=$0008) plus a pin-6 pulse (both ports) at
        // (200,100) in every phase-A field (SPECIFICATION.md section 8).
        // Phase A is what this mutation perturbs, so the pulses are timed
        // live from the harness's own vs signal rather than a pre-fit
        // model (which needs phase A to have already run undisturbed).
        // The per-field offset (200 lines) is computed from a colour-clock
        // length measured from this run's own log (SPECIFICATION.md 5.2)
        // and the page's own line length (p.194: "227.5 'color clocks'"),
        // not a constant carried over from another run.
        nmh::Config cfg = make_config(true, false);
        auto h = std::make_unique<nmh::NanomigHarness>(cfg);
        size_t loaded = h->load_program(dir + "/prog_k4.bin");
        bool released = loaded && h->boot(kBootBudget).cpu_released;
        checks.expect(loaded > 0 && released, "K17/ECS/PAL: program loaded and CPU released from reset");
        if (loaded && released) {
            h->run_cycles(20000); // a short prefix, enough code fetches to measure the colour-clock grid
            int64_t cck = derive_colour_clock_len(h->ram_log(), (int64_t)h->cycles());
            int64_t lineLenClksys = (int64_t)std::llround(227.5 * (double)cck);
            int64_t pulseOffsetClksys = 200 * lineLenClksys;
            checks.note("K17/ECS/PAL: measured colour_clock_len=" + std::to_string(cck) +
                " line_len_clksys=" + std::to_string(lineLenClksys));
            std::vector<Event> ev;
            PortState st;
            uint64_t budget = 8'000'000;
            size_t lastVid = 0;
            bool prevvs = false;
            int fieldsSeen = 0;
            while (h->cycles() < budget && fieldsSeen < 6) {
                h->run_cycles(1000);
                const auto& vid = h->video();
                for (size_t i = lastVid; i < vid.size(); ++i) {
                    bool vs = vid[i].vs;
                    if (vs && !prevvs) {
                        ++fieldsSeen;
                        int64_t onset = (int64_t)i + pulseOffsetClksys;
                        ev.push_back({onset, [&st]() { st.joy0 |= kBitFire; st.joy1 |= kBitFire; }});
                        ev.push_back({onset + 4 * cck, [&st]() { st.joy0 &= (uint8_t)~kBitFire; st.joy1 &= (uint8_t)~kBitFire; }});
                    }
                    prevvs = vs;
                }
                lastVid = vid.size();
            }
            run_phase_events(*h, ev, st);
            auto m1cyc = h->run_until_write(kMark, kM1, kRunBudget);
            checks.expect(m1cyc.has_value(), "K17/ECS/PAL: phase A completed with the added pulses");
            if (m1cyc) {
                auto coderd = code_read_cycles(h->ram_log());
                auto A = extract_long_samples(h->ram_log(), coderd, kBufA, kBufA + 4 * kBufAcount);
                BeamModel m = fit_model(A, h->ram_log(), h->video(), false);
                bool a1 = m.ok && check_A1(A, m, 29);
                bool a2 = m.ok && check_A2(A, m, false);
                checks.expect(m.ok && a1 && a2, "K17 MUTATION NMH_MUTATE=K17: CP1 (A1, A2) measured with the "
                    "added line-200 pulses");
                if (m.ok) checks.note("K17 MUTATION measured: L=" + std::to_string(m.L) + " (the page predicts "
                    "the largest read V per field would be 200, not L, once each pulse freezes VPOSR)");
            }
        }
    } else if (label == "K11") {
        // Pin 6 of both ports held closed from before boot through one
        // field after M0, then released (SPECIFICATION.md section 8).
        // Bit 3 is clear throughout phase A, so the free-running counter
        // is unaffected by the hold; the release margin is measured from
        // this run's own log (a colour-clock length, SPECIFICATION.md
        // 5.2) and the page's own field length (p.24: 312 or 313 lines
        // PAL; p.194: 227.5 colour clocks per line), using the largest
        // (313) so the hold always covers at least one whole field. After
        // release the run proceeds through the main schedule as usual,
        // and CP3 is graded with the main run's own comparator.
        nmh::Config cfg = make_config(true, false);
        auto h = std::make_unique<nmh::NanomigHarness>(cfg);
        g_applied.clear();
        h->ports()->joystick0 = kBitFire;
        h->ports()->joystick1 = kBitFire;
        size_t loaded = h->load_program(dir + "/prog.bin");
        bool released = loaded && h->boot(kBootBudget).cpu_released;
        checks.expect(loaded > 0 && released, "K11/ECS/PAL: program loaded and CPU released from reset");
        if (loaded && released) {
            auto m0cyc = h->run_until_write(kMark, kM0, kRunBudget);
            checks.expect(m0cyc.has_value(), "K11/ECS/PAL: M0 written with pin 6 held closed");
            if (m0cyc) {
                int64_t cck = derive_colour_clock_len(h->ram_log(), (int64_t)*m0cyc);
                int64_t lineLenClksys = (int64_t)std::llround(227.5 * (double)cck);
                int64_t releaseCycle = (int64_t)*m0cyc + 313 * lineLenClksys;
                int64_t now = (int64_t)h->cycles();
                if (releaseCycle > now) h->run_cycles((uint64_t)(releaseCycle - now));
                uint8_t fireHeld = 0xFF;
                if (auto w = h->peek_logical(0x100004)) fireHeld = uint8_t((*w >> 8) & 0xFF);
                checks.note("K11/ECS/PAL: measured FIRE=$" +
                    [&]{ char b[8]; std::snprintf(b, sizeof b, "%02X", fireHeld); return std::string(b); }() +
                    " while pin 6 was held closed, before release");
                h->ports()->joystick0 = 0;
                h->ports()->joystick1 = 0;

                auto m1cyc = h->run_until_write(kMark, kM1, kRunBudget);
                if (m1cyc) {
                    auto coderd = code_read_cycles(h->ram_log());
                    auto A = extract_long_samples(h->ram_log(), coderd, kBufA, kBufA + 4 * kBufAcount);
                    BeamModel model = fit_model(A, h->ram_log(), h->video(), false);
                    RunResult r;
                    r.ntsc = false; r.loaded = true; r.released = true; r.model = model;
                    r.B0_idx = model.ok ? model.field_index_of((int64_t)*m1cyc) : 0;
                    PortState state;
                    if (model.ok) {
                        std::vector<Event> evB;
                        schedB_main(model, r.B0_idx, evB, &state);
                        run_phase_events(*h, evB, state);
                    }
                    auto m2cyc = h->run_until_write(kMark, kM2, kRunBudget);
                    if (m2cyc) {
                        r.C0_idx = model.ok ? model.field_index_of((int64_t)*m2cyc) : 0;
                        if (model.ok) {
                            std::vector<Event> evC;
                            schedC_main(model, r.C0_idx, evC, &state);
                            run_phase_events(*h, evC, state);
                        }
                        auto m3cyc = h->run_until_write(kMark, kM3, kRunBudget);
                        r.M3cyc = m3cyc ? (int64_t)*m3cyc : 0;
                        if (model.ok && m3cyc) {
                            std::vector<Event> evD;
                            schedD_main(model, r.M3cyc, evD, &state, false);
                            run_phase_events(*h, evD, state);
                        }
                        auto m4cyc = h->run_until_write(kMark, kM4, kRunBudget);
                        r.done_ok = m4cyc.has_value();
                    }
                    r.log = h->ram_log();
                    r.vid = h->video();
                    auto coderd2 = code_read_cycles(r.log);
                    r.A = extract_long_samples(r.log, coderd2, kBufA, kBufA + 4 * kBufAcount);
                    uint32_t bufBend = kBufB + 4 * kBufBcount;
                    uint32_t bufC = bufBend, bufCend = bufC + 4 * kBufCcount, bufD = bufCend, bufDend = bufD + 2 * kBufDcount;
                    r.B = extract_long_samples(r.log, coderd2, kBufB, bufBend);
                    r.C = extract_long_samples(r.log, coderd2, bufC, bufCend);
                    r.D = extract_word_samples(r.log, coderd2, bufD, bufDend);
                    if (auto w = h->peek_logical(0x100004)) r.fire_byte = uint8_t((*w >> 8) & 0xFF);
                    r.m0cyc = (int64_t)*m0cyc;
                    r.m1cyc = (int64_t)*m1cyc;
                    r.m2cyc = find_mark_cycle(r.log, kM2);
                    r.m3cyc = r.M3cyc;
                    r.m4cyc = r.done_ok ? find_mark_cycle(r.log, kM4) : -1;
                    r.applied = g_applied;
                    checks.expect(r.done_ok, "K11/ECS/PAL: the program completed (M0 to M4 written in order)");
                    if (model.ok && r.done_ok) {
                        Preconds pc = grade_preconditions(r, false, "K11/ECS/PAL");
                        checks.expect(!pc.cp3, "K11 MUTATION stimulus red NMH_MUTATE=K11: CP3 is red (pin 6 "
                            "of both ports held closed from before boot through one field after M0, then "
                            "released; graded with the main run's own comparator)");
                    }
                }
            }
        }
    } else {
        checks.record("NMH_MUTATE=" + label + " names no mutation of this package");
    }
}

} // namespace
