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
// samples whose whole read interval [a,b] lies in [lo,hi)
std::vector<Sample> filter_ab(const std::vector<Sample>& s, int64_t lo, int64_t hi) {
    std::vector<Sample> out;
    for (const auto& x : s) if (x.a >= lo && x.b < hi) out.push_back(x);
    return out;
}
int64_t line_len_clk(const BeamModel& m) { return m.line_len_cck * m.colour_clock_len; }

// VB(k): lines 0 to 19 of field k, counted by the counter (5.5)
std::vector<Sample> window_VB(const std::vector<Sample>& s, const BeamModel& m, int64_t field_idx) {
    return filter_ab(s, m.line_start(field_idx, 0), m.line_start(field_idx, 20));
}
// RUN(k): lines Vrun to L-1 of field k, ended by `cutoff` where given (5.5)
std::vector<Sample> window_RUN(const std::vector<Sample>& s, const BeamModel& m, int64_t field_idx,
                                int Vrun, std::optional<int64_t> cutoff = std::nullopt) {
    int64_t lo = m.line_start(field_idx, Vrun);
    int64_t hi = m.line_start(field_idx, m.L);
    if (cutoff && *cutoff < hi) hi = *cutoff;
    if (hi < lo) hi = lo;
    return filter_ab(s, lo, hi);
}
// POST(p): from t_off to the end of VB(k+1)
std::vector<Sample> window_POST(const std::vector<Sample>& s, const BeamModel& m, int64_t t_off, int64_t field_idx_plus1) {
    int64_t lo = t_off;
    int64_t hi = m.line_start(field_idx_plus1, 20);
    return filter_ab(s, lo, hi);
}

bool all_equal_raw(const std::vector<Sample>& s) {
    if (s.empty()) return false;
    for (const auto& x : s) if (x.raw32 != s[0].raw32) return false;
    return true;
}
// The position (V,H) of every sample that is not torn is the same one
// (SPECIFICATION.md 10.8: P6 to P8 compare positions, not whole words).
// Torn samples are left out, as they are of the currency checks; a window
// with no sample left is not equal.
bool all_equal_pos(const std::vector<Sample>& s, const BeamModel& m) {
    const Sample* first = nullptr;
    for (const auto& x : s) {
        if (m.is_torn(x)) continue;
        if (!first) first = &x;
        else if (x.V != first->V || x.H != first->H) return false;
    }
    return first != nullptr;
}
const Sample* first_untorn(const std::vector<Sample>& s, const BeamModel& m) {
    for (const auto& x : s) if (!m.is_torn(x)) return &x;
    return nullptr;
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
// Where a sample lies against the release at the end of vertical blanking
// of its own field (5.5): -1 when its whole interval is before the model's
// V = 20, +1 when it starts at least u colour clocks after it, 0 between:
// the counter reaches V = 20 between the model's time and u later, so a
// sample in between may read either side of the release.
int release_side(const Sample& s, const BeamModel& m) {
    int64_t f = m.field_index_of(s.a);
    int64_t t20 = m.line_start(f, 20);
    if (s.b < t20) return -1;
    if (s.a >= t20 + m.u_cck * m.colour_clock_len) return 1;
    return 0;
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
std::vector<PulseDef> phaseB_pin6_pulses() {
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
        // The boundary pulse at V = 20 (P9's re-arming instant): the only
        // pulse of field B19. It sits before Vrun, so it does not fall inside
        // any RUN(j) window that P7 or P8 grade for currency, and no later
        // pulse follows it in its field: a second trigger while the value is
        // frozen would not change it (P5).
        {19, 20, 150, true, true, kBitFire, false, "B19c"},
    };
}
// B18's other-pin pulses (not pin 6): pins 1,2,3,4,9 both ports at lines
// 40,60,80,100,140 (H=100). Pin 5 is the pot line (POTGOR bit 8/12): port 1's
// is driven through mouse_buttons at line 120 (kMousePulseLine), port 2's
// through joystick0 bit 6 (kBitFire3) at the same line and H, as a second
// event on the same cycle.
std::vector<PulseDef> phaseB18_other_pulses() {
    return {
        {18, 40,  100, true,  true,  kBitUp,    true, "B18-pin1"},
        {18, 60,  100, true,  true,  kBitDown,  true, "B18-pin2"},
        {18, 80,  100, true,  true,  kBitLeft,  true, "B18-pin3"},
        {18, 100, 100, true,  true,  kBitRight, true, "B18-pin4"},
        {18, 120, 100, true,  false, kBitFire3, true, "B18-pin5-port2"},
        {18, 140, 100, true,  true,  kBitFire2, true, "B18-pin9"},
    };
}
constexpr int kMousePulseLine = 120;

// A mutation of the phase-B schedule: the main schedule plus exactly the
// changes named here.
struct PulseMods {
    std::function<bool(PulseDef&)> pin6;   // changes a pin-6 pulse; false drops it
    std::function<void(PulseDef&)> other;  // changes an other-pin pulse
    std::vector<PulseDef> extra_pin6;      // pulses added
    int mouse_line_delta = 0;              // moves B18's port-1 pin-5 pulse
};

// the phase-B pin-6 pulses a schedule delivers, after `mods`
std::vector<PulseDef> scheduled_pin6(const PulseMods& mods) {
    std::vector<PulseDef> out;
    for (auto p : phaseB_pin6_pulses()) {
        if (mods.pin6 && !mods.pin6(p)) continue;
        out.push_back(p);
    }
    for (auto p : mods.extra_pin6) out.push_back(p);
    return out;
}
// ---------------------------------------------------------------------
// main-run schedules (SPECIFICATION.md section 6)
// ---------------------------------------------------------------------
void schedB_build(const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st, const PulseMods& mods) {
    for (const auto& p : scheduled_pin6(mods))
        add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
    for (auto p : phaseB18_other_pulses()) {
        if (mods.other) mods.other(p);
        add_pulse_events(ev, m, B0, st, p.field_offset, p.line, p.H, p.port0, p.port1, p.bit);
    }
    add_mouse_pulse_events(ev, m, B0, st, 18, kMousePulseLine + mods.mouse_line_delta, 100); // B18's pin-5 pulse, port 1 only
}
void schedB_main(const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st) {
    schedB_build(m, B0, ev, st, PulseMods());
}
void schedC_main(const BeamModel& m, int64_t C0, std::vector<Event>& ev, PortState* st) {
    add_pulse_events(ev, m, C0, st, 1, 100, 100, true, true, kBitFire);
}
void add_port2_pin5_window(const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st, bool ntsc) {
    int64_t t5 = M3 + us_to_cck(5000, ntsc) * m.colour_clock_len;
    int64_t t7 = M3 + us_to_cck(7000, ntsc) * m.colour_clock_len;
    ev.push_back({t5, [st]() { st->joy0 |= kBitFire3; }});
    ev.push_back({t7, [st]() { st->joy0 &= (uint8_t)~kBitFire3; }});
}
void schedD_main(const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st, bool ntsc) {
    // port 1 pin 5 low 2-4ms
    int64_t t2 = M3 + us_to_cck(2000, ntsc) * m.colour_clock_len;
    int64_t t4 = M3 + us_to_cck(4000, ntsc) * m.colour_clock_len;
    ev.push_back({t2, [st]() { st->mbtn |= kMouseMid; }});
    ev.push_back({t4, [st]() { st->mbtn &= (uint8_t)~kMouseMid; }});
    // port 2 pin 5 (joystick0 bit 6) low 5-7ms
    add_port2_pin5_window(m, M3, ev, st, ntsc);
    // both ports pin 9 low 8-10ms
    int64_t t8 = M3 + us_to_cck(8000, ntsc) * m.colour_clock_len;
    int64_t t10 = M3 + us_to_cck(10000, ntsc) * m.colour_clock_len;
    ev.push_back({t8, [st]() { st->joy0 |= kBitFire2; st->joy1 |= kBitFire2; }});
    ev.push_back({t10, [st]() { st->joy0 &= (uint8_t)~kBitFire2; st->joy1 &= (uint8_t)~kBitFire2; }});
}

// ---------------------------------------------------------------------
// The mutations of the phase-B schedule (SPECIFICATION.md section 8): each
// is the main schedule plus one change.
// ---------------------------------------------------------------------
PulseMods mods_K2() { // pin-6 pulses (both ports) added at (100,100) in B2 and B3
    PulseMods k;
    k.extra_pin6 = {{2, 100, 100, true, true, kBitFire, true, "K2-B2"},
                    {3, 100, 100, true, true, kBitFire, true, "K2-B3"}};
    return k;
}
PulseMods mods_K6() { // the line-10 pulse of B15 and the line-19 pulse of B17 moved to line 30
    PulseMods k;
    k.pin6 = [](PulseDef& p) { if (p.tag == "B15a" || p.tag == "B17a") p.line = 30; return true; };
    return k;
}
PulseMods mods_K7() { // the line-32 pulses of B15 and B17 moved to line 12
    PulseMods k;
    k.pin6 = [](PulseDef& p) { if (p.tag == "B15b" || p.tag == "B17b") p.line = 12; return true; };
    return k;
}
PulseMods mods_K8() { // the first pulse of B14 (line 80) removed
    PulseMods k;
    k.pin6 = [](PulseDef& p) { return p.tag != "B14a"; };
    return k;
}
PulseMods mods_K9b() { // the pin-6 pulses of B4, B5 and B18 moved to pin 9 of the same ports
    PulseMods k;
    k.pin6 = [](PulseDef& p) { if (p.tag == "B4" || p.tag == "B5" || p.tag == "B18") p.bit = kBitFire2; return true; };
    return k;
}
PulseMods mods_K12() { // every pulse one line later
    PulseMods k;
    k.pin6 = [](PulseDef& p) { p.line += 1; return true; };
    k.other = [](PulseDef& p) { p.line += 1; };
    k.mouse_line_delta = 1;
    return k;
}
PulseMods mods_K16() { // B7 at (100,82) instead of (100,102); B11 at (39,100) instead of (40,100)
    PulseMods k;
    k.pin6 = [](PulseDef& p) {
        if (p.tag == "B7") { p.line = 100; p.H = 82; }
        else if (p.tag == "B11") { p.line = 39; p.H = 100; }
        return true;
    };
    return k;
}
PulseMods mods_K19() { // B4's and B5's pulses at (100,80) instead of (100,100)
    PulseMods k;
    k.pin6 = [](PulseDef& p) { if (p.tag == "B4" || p.tag == "B5") p.H = 80; return true; };
    return k;
}
PulseMods mods_K20() { // B19c at (19,150) instead of (20,150)
    PulseMods k;
    k.pin6 = [](PulseDef& p) { if (p.tag == "B19c") p.line = 19; return true; };
    return k;
}
// ---------------------------------------------------------------------
// preconditions
// ---------------------------------------------------------------------
struct Preconds { bool cp1=false, cp2=false, cp3=false, cp4=false, cp5=false, cp6=false; };
constexpr int kUBoundCck = 15; // A2 (SPECIFICATION.md 5.4): u <= 15 colour clocks
constexpr int kUFloorCck = 4;  // A2 (5.4): the gap [a,b] holds two 68000 bus cycles of two memory cycles each (p.196)

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
// counter numbers lines 0 to L and returns to 0 exactly once per field".
// The first is graded per model field: no non-torn sample in the fitted
// lines reads a smaller V than an earlier one of the same model field. The
// second is graded on the samples' own read values, split as fit_model
// splits them (a decrease of the read V starts a new field of reads; torn
// samples, which combine two lines' bits, are left out): each split must
// begin exactly one model field after the previous one, never in the same
// model field (a spurious extra split) and never two or more later (a missed
// field). The measured V-increment phase must be bracketed consistently and
// stay within the bound u has.
bool check_A2(const std::vector<Sample>& A, const BeamModel& m, bool ntsc) {
    if (m.u_cck > kUBoundCck) return false;
    if (m.u_cck < kUFloorCck) return false; // 5.2's floor: the gap holds at least two bus cycles
    if (m.vinc_lo > m.vinc_hi) return false; // the phase is not bracketed
    if (m.vinc_cck > kUBoundCck) return false;
    int want1 = ntsc ? 262 : 312, want2 = ntsc ? 263 : 313;
    if (m.L + 1 != want1 && m.L + 1 != want2) return false;
    if (A.empty()) return false;
    int64_t span = A.back().a - A.front().a;
    if (span < 4 * m.field_len_clksys) return false; // at least four fields (>= two periods of <=2 fields)

    int Vrun = ntsc ? 21 : 29;
    // V never decreases within the fitted lines of a model field
    std::optional<int64_t> lastFieldIdx;
    int prevV = -1;
    for (size_t i = 0; i < A.size(); ++i) {
        int64_t f = m.field_index_of(A[i].a);
        if (!lastFieldIdx || f != *lastFieldIdx) prevV = -1;
        if (A[i].V >= Vrun && A[i].V <= m.L - 1 && !m.is_torn(A[i])) {
            if (prevV >= 0 && A[i].V < prevV) return false; // V decreased within one field's fitted lines
            prevV = A[i].V;
        }
        lastFieldIdx = f;
    }
    // the counter returns to 0 exactly once per field: the splits of the
    // read V land exactly one model field apart. The model field of a split
    // is that of its first sample's later bound b, which lies after the
    // counter's V-increment phase of the field it starts.
    int prevRead = -1;
    std::optional<int64_t> lastSplitField;
    for (size_t i = 0; i < A.size(); ++i) {
        if (m.is_torn(A[i])) continue;
        bool split = prevRead >= 0 && A[i].V < prevRead;
        prevRead = A[i].V;
        if (!split) continue;
        int64_t f = m.field_index_of(A[i].b);
        if (lastSplitField && f - *lastSplitField != 1) return false;
        lastSplitField = f;
    }
    return true;
}
bool check_A2(const RunResult& r, int Vrun, bool ntsc) { (void)Vrun; return check_A2(r.A, r.model, ntsc); }

// CP1 (SPECIFICATION.md 7) over a set of phase-A samples: the combined line
// and its two components, each printed under its own label. `ntsc` is the
// standard the samples are graded as.
bool grade_cp1(const std::vector<Sample>& A, const BeamModel& m, bool ntsc, const std::string& cfgLabel,
               const std::string& what) {
    int Vrun = ntsc ? 21 : 29;
    bool a1 = check_A1(A, m, Vrun);
    bool a2 = check_A2(A, m, ntsc);
    char vinc[128];
    std::snprintf(vinc, sizeof vinc, "%d (bracket %d..%d, %d of %d fitted samples read the previous line's V)",
                  m.vinc_cck, m.vinc_lo, m.vinc_hi, m.vinc_shifted, m.vinc_fitted);
    checks.expect(a1 && a2, "CP1 " + cfgLabel + ": " + what + " (A1 and A2)");
    checks.expect(a1, "CP1(A1) " + cfgLabel + ": every non-torn phase-A sample in the fitted lines is current");
    checks.expect(a2, "CP1(A2) " + cfgLabel + ": field length is 262/263 or 312/313 lines as graded (measured " +
        std::to_string(m.L + 1) + "), " + std::to_string(kUFloorCck) + "<=u<=" + std::to_string(kUBoundCck) +
        " colour clocks (measured u=" + std::to_string(m.u_cck) + "), the counter's V-increment phase measured at H=" +
        std::string(vinc) + " and bracketed within " + std::to_string(kUBoundCck) +
        ", at least four fields covered, V never decreases within a field's fitted lines, and the splits of the "
        "read V (a decrease starts a field of reads, as fit_model splits them) each begin exactly one model "
        "field after the previous one");
    return a1 && a2;
}

// A "complete field": one whose whole span [field_start(f), field_start(f+1))
// lies inside [lo,hi). A3 and A4 both grade only complete fields -- a field
// only partly inside the window cannot show its own full line population or
// know whether it holds exactly one vsync onset.
void complete_field_range(const BeamModel& m, int64_t lo, int64_t hi, int64_t* f0, int64_t* f1) {
    *f0 = m.field_index_of(lo);
    if (m.field_start(*f0) < lo) ++*f0;
    *f1 = m.field_index_of(hi);
    if (m.field_start(*f1) + m.field_len_clksys > hi) --*f1;
}

// A3 (SPECIFICATION.md 5.4): "exactly four refresh accesses at the
// Chip-memory port, at the same model H positions in every model line." Every
// non-code, non-write access at the Chip-memory port in phases A-C is a
// refresh access -- the program's own register accesses ($DFF004/6/16)
// go through the custom-register bus, not the Chip-memory port, and its
// only other Chip-memory reads are its own code fetches (excluded by
// kCodeLo/kCodeHi) and the two stack reads of `rts` (excluded by their
// known address, PROGRAM_STACK). No percentage tolerance, and no partial
// population: every complete field in the window must show every one of
// its L+1 lines with the same H-set, not merely "at least 8 lines seen"
// (which can be satisfied while whole lines, and whole fields, are
// missing from the count). Keyed by (field, line), not by line alone, so
// two different fields never merge into one line's population. The line is
// the model's geometric line (V changes at H = 0), not the counter's V.
constexpr uint32_t kStackWordLo = (0x0FFF00 - 64) / 2, kStackWordHi = (0x0FFF00 + 8) / 2;
bool check_A3(const RunResult& r, int64_t lo, int64_t hi, std::string* detail = nullptr) {
    // (field,line) -> the H of every refresh access of the line, one entry
    // per access (a multiset: two accesses at one H count twice)
    std::map<std::pair<int64_t,int64_t>, std::vector<int64_t>> byLine;
    for (const auto& a : r.log) {
        if (a.is_write) continue;
        if ((int64_t)a.cycle < lo || (int64_t)a.cycle >= hi) continue;
        if (a.word_addr >= kCodeLo/2 && a.word_addr < kCodeHi/2) continue;
        if (a.word_addr >= kStackWordLo && a.word_addr < kStackWordHi) continue;
        int mv, mh; r.model.geo_at((int64_t)a.cycle, &mv, &mh);
        int64_t f = r.model.field_index_of((int64_t)a.cycle);
        byLine[{f, (int64_t)mv}].push_back(mh);
    }
    int64_t f0, f1;
    complete_field_range(r.model, lo, hi, &f0, &f1);
    if (f0 > f1) return false; // no complete field in the window
    std::vector<int64_t> fixedHs;
    bool first = true, ok = true;
    int lines = 0, wrongCount = 0, minN = 1 << 30, maxN = 0;
    for (int64_t f = f0; f <= f1; ++f) {
        for (int line = 0; line <= r.model.L; ++line) {
            ++lines;
            auto it = byLine.find({f, (int64_t)line});
            int n = it == byLine.end() ? 0 : (int)it->second.size();
            minN = std::min(minN, n); maxN = std::max(maxN, n);
            if (n != 4) { ++wrongCount; ok = false; continue; } // p.194: 4 cycles for memory refresh, every line
            std::vector<int64_t> hs = it->second;
            std::sort(hs.begin(), hs.end());
            if (first) { fixedHs = hs; first = false; }
            else if (hs != fixedHs) ok = false; // the same H positions in every line
        }
    }
    if (detail) {
        *detail = std::to_string(lines) + " lines, " + std::to_string(wrongCount) + " with other than four accesses (per-line count " +
            std::to_string(minN) + ".." + std::to_string(maxN) + "), refresh H of the first line:";
        for (auto h : fixedHs) *detail += " " + std::to_string(h);
    }
    return ok && !first;
}

// A4 (SPECIFICATION.md 5.4): "one vertical-sync pulse begins in each
// model field, at the same model position in every field." Graded from
// the model position (line, H) of each vsync onset in the window -- a
// position the beam-counter model supplies independently of vsync itself
// (the model's anchor and line length are fit from the phase-A counter
// reads; only field_len_clksys, used only to pick line length, comes
// from vsync). Comparing onset positions across fields is therefore not
// circular: it cross-checks the counter-fitted position mapping against
// an independently-timed signal. Every complete field must contain
// EXACTLY one onset (not zero, not more than one): a window with no
// complete field at all is not graded (there is nothing to require one
// onset of), but a complete field with zero onsets, or with more than
// one, is a genuine violation, not a vacuous pass.
bool check_A4(const RunResult& r, int64_t lo, int64_t hi) {
    auto spans = nmh::find_frames(r.vid);
    int64_t f0, f1;
    complete_field_range(r.model, lo, hi, &f0, &f1);
    if (f0 > f1) return false; // no complete field in the window
    std::map<int64_t, int> countByField;
    std::optional<std::pair<int,int>> fixedPos;
    for (size_t i = 0; i < spans.size(); ++i) {
        int64_t onset = (int64_t)spans[i].begin; // video sample index == clk_sys cycle (1:1)
        if (onset < lo || onset >= hi) continue;
        int64_t f = r.model.field_index_of(onset);
        if (f < f0 || f > f1) continue;
        ++countByField[f];
        int mv, mh; r.model.geo_at(onset, &mv, &mh);
        if (!fixedPos) fixedPos = {mv, mh};
        else if (fixedPos->first != mv || fixedPos->second != mh) return false;
    }
    for (int64_t f = f0; f <= f1; ++f) {
        auto it = countByField.find(f);
        if (it == countByField.end() || it->second != 1) return false;
    }
    return true;
}

// The cycles of every phase-B and phase-C input change of the
// SPECIFICATION.md section 6 schedule (not of a mutated one), from a run's
// own model: each pulse's onset and its end 4 colour clocks later.
std::vector<int64_t> spec_cycles_BC(const RunResult& r) {
    std::vector<int64_t> v;
    auto add = [&](int64_t t_on) { v.push_back(t_on); v.push_back(t_on + 4 * r.model.colour_clock_len); };
    for (const auto& p : phaseB_pin6_pulses())
        add(r.model.cycle_of(r.B0_idx + p.field_offset, p.line, p.H));
    for (const auto& p : phaseB18_other_pulses())
        add(r.model.cycle_of(r.B0_idx + p.field_offset, p.line, p.H));
    add(r.model.cycle_of(r.B0_idx + 18, kMousePulseLine, 100));
    add(r.model.cycle_of(r.C0_idx + 1, 100, 100));
    std::sort(v.begin(), v.end());
    return v;
}

// CP4 (SPECIFICATION.md section 7): every applied input change landed on
// its own scheduled cycle (the harness's own delivery record, not an
// assumption), and every phase A-C pulse (a momentary bit, not one of
// phase D's multi-millisecond windows) was exactly 4 colour clocks wide.
// With `spec_cycles`, the cycles the phase-B and phase-C changes landed on
// must also be exactly the section 6 schedule's (for a contrasting run
// whose own target is a different clause, "the schedule" is its changed
// schedule, and none is passed).
bool check_CP4(const RunResult& r, std::string* detail, const std::vector<int64_t>* spec_cycles = nullptr) {
    for (auto& e : r.applied) if (e.sched != e.applied) {
        if (detail) *detail = "delivery not cycle-exact sched=" + std::to_string(e.sched) +
            " applied=" + std::to_string(e.applied) + " j0=" + std::to_string(e.j0) +
            " j1=" + std::to_string(e.j1) + " mb=" + std::to_string(e.mb);
        return false;
    }
    if (spec_cycles) {
        std::vector<int64_t> got;
        for (auto& e : r.applied) if (e.applied < r.M3cyc) got.push_back(e.applied);
        std::sort(got.begin(), got.end());
        if (got.size() != spec_cycles->size()) {
            if (detail) *detail = std::to_string(got.size()) + " phase-B/C changes delivered, the section 6 schedule has " +
                std::to_string(spec_cycles->size());
            return false;
        }
        for (size_t i = 0; i < got.size(); ++i) if (got[i] != (*spec_cycles)[i]) {
            if (detail) *detail = "change " + std::to_string(i) + " landed on cycle " + std::to_string(got[i]) +
                ", the section 6 schedule has " + std::to_string((*spec_cycles)[i]) + " (a difference of " +
                std::to_string(got[i] - (*spec_cycles)[i]) + " clk_sys)";
            return false;
        }
    }
    int64_t cck = std::max<int64_t>(1, r.model.colour_clock_len);
    uint8_t pj0 = 0, pj1 = 0, pmb = 0;
    bool have = false;
    int64_t onSched = 0; bool pending = false;
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
    }
    return true;
}

// CP3 (b) (SPECIFICATION.md 7), from the record of every write the run made
// to the harness's input ports -- the schedule's changes and any other write:
// every interval in which pin 6 of either port was closed lasts exactly 4
// colour clocks, and none lies in phase A (it begins at or after M1).
bool check_CP3b(const RunResult& r, std::string* detail) {
    int64_t cck = std::max<int64_t>(1, r.model.colour_clock_len);
    bool closed = false;
    int64_t since = 0;
    for (const auto& e : r.portlog) {
        bool now = ((e.j0 | e.j1) & kBitFire) != 0 || (e.mb & kMousePin6) != 0;
        if (now && !closed) {
            closed = true;
            since = e.applied;
            if (r.m1cyc >= 0 && since < r.m1cyc) {
                if (detail) *detail = "pin 6 closed in phase A, from cycle " + std::to_string(since);
                return false;
            }
        } else if (!now && closed) {
            closed = false;
            if (e.applied - since != 4 * cck) {
                if (detail) *detail = "pin 6 closed for " + std::to_string(e.applied - since) +
                    " clk_sys from cycle " + std::to_string(since) + ", expected " + std::to_string(4 * cck);
                return false;
            }
        }
    }
    if (closed) {
        if (detail) *detail = "pin 6 still closed at the end of the record";
        return false;
    }
    return true;
}

// FIXTURE (SPECIFICATION.md 7): the program's marker M0 was stored (a known
// constant) and the program completed.
void grade_fixture(const RunResult& r, const std::string& label) {
    checks.expect(r.m0cyc >= 0, "FIXTURE(M0) " + label + ": the phase marker M0 (a known constant) was stored "
        "before phase A sampling began");
    checks.expect(r.done_ok, "FIXTURE(DONE) " + label + ": the program completed (M0 to M4 written in order, "
        "DONE reached)");
}

// CP1 to CP6 for a run. `vs_spec`: CP4 also compares the delivered
// phase-B/C cycles with the SPECIFICATION.md section 6 schedule. `cp1`: CP1
// is graded as another standard than the run's own (K15), with its own text.
struct Cp1As { bool ntsc; std::string what; };
Preconds grade_preconditions(const RunResult& r, bool ntsc, const std::string& cfgLabel, bool vs_spec = false,
                             const Cp1As* cp1 = nullptr) {
    Preconds pc;
    int Vrun = ntsc ? 21 : 29;
    pc.cp1 = cp1 ? grade_cp1(r.A, r.model, cp1->ntsc, cfgLabel, cp1->what)
                 : grade_cp1(r.A, r.model, ntsc, cfgLabel, "beam model anchored on phase A");
    checks.note(cfgLabel + " model: colour_clock_len=" + std::to_string(r.model.colour_clock_len) +
        " line_len_cck=" + std::to_string(r.model.line_len_cck) + " L=" + std::to_string(r.model.L) +
        " field_len_clksys=" + std::to_string(r.model.field_len_clksys) + " u_cck=" + std::to_string(r.model.u_cck) +
        " vinc_cck=" + std::to_string(r.model.vinc_cck) + " (bracket " + std::to_string(r.model.vinc_lo) + ".." +
        std::to_string(r.model.vinc_hi) + ", " + std::to_string(r.model.vinc_shifted) + " of " +
        std::to_string(r.model.vinc_fitted) + " fitted samples read the previous line's V)");

    // CP2: dense sampling in phases A-C (<=56 colour clocks between
    // consecutive read instants, at least two samples per line of the
    // fitted/graded ranges) and in phase D (<=50 microseconds).
    int64_t g = max_gap_cck(r.A, r.model);
    g = std::max(g, max_gap_cck(r.B, r.model));
    g = std::max(g, max_gap_cck(r.C, r.model));
    // The expected cells of a phase: every (field, line) from the cell of
    // its first tallied sample to the cell of its last, with the line in the
    // phase's graded range. A cell with no sample at all counts zero and
    // fails like a cell with one.
    std::map<std::pair<int64_t,int>, int> perLine; // (field, line) -> sample count, phases A-C
    int expectedCells = 0, thin = 0;
    // The very last sample of a buffer ends its line's tally mid-line (the
    // buffer simply runs out there), not because sampling thinned out; it
    // is excluded, one per phase, as that boundary artifact.
    auto tally = [&](const std::vector<Sample>& s, int loLine, int hiLine) {
        std::map<std::pair<int64_t,int>, int> cnt;
        size_t n = s.empty() ? 0 : s.size() - 1;
        if (n == 0) return;
        for (size_t i = 0; i < n; ++i) {
            const auto& x = s[i];
            int line; r.model.geo_at(x.a, &line, nullptr);
            if (line < loLine || line > hiLine) continue;
            ++cnt[{r.model.field_index_of(x.a), line}];
        }
        int l0, l1; r.model.geo_at(s[0].a, &l0, nullptr); r.model.geo_at(s[n - 1].a, &l1, nullptr);
        std::pair<int64_t,int> first{r.model.field_index_of(s[0].a), l0}, last{r.model.field_index_of(s[n - 1].a), l1};
        for (int64_t f = first.first; f <= last.first; ++f)
            for (int line = loLine; line <= hiLine; ++line) {
                std::pair<int64_t,int> cell{f, line};
                if (cell < first || cell > last) continue;
                auto it = cnt.find(cell);
                int c = it == cnt.end() ? 0 : it->second;
                ++expectedCells;
                if (c < 2) ++thin;
                perLine[cell] += c;
            }
    };
    tally(r.A, Vrun, r.model.L - 1);
    tally(r.B, 0, r.model.L);
    tally(r.C, 0, r.model.L);
    int64_t dgap_us_cck = us_to_cck(50, ntsc) * r.model.colour_clock_len;
    int64_t dgap = max_gap_cck(r.D, r.model) * r.model.colour_clock_len; // convert back to clk_sys for a direct compare
    bool cp2 = g <= 56 && !r.A.empty() && !r.B.empty() && !r.C.empty() && thin == 0 &&
               (r.D.empty() || dgap <= dgap_us_cck);
    pc.cp2 = cp2;
    checks.expect(pc.cp2, "CP2 " + cfgLabel + ": sampling is dense (max phase A-C read-instant gap " +
        std::to_string(g) + " colour clocks, <=56 required; " + std::to_string(thin) +
        " (field,line) cells with fewer than two samples, zero included, of " + std::to_string(expectedCells) +
        " expected cells" +
        "; max phase-D gap " + std::to_string(dgap) + " clk_sys, <=" + std::to_string(dgap_us_cck) + " required)");

    bool cp3a = (r.fire_byte & 0xC0) == 0xC0; // bits 6,7 = 1 ("switch open")
    // (b): from the record of every write to the input ports, pin 6 of either
    // port is closed only in intervals of exactly 4 colour clocks (the
    // scheduled pulses' width), none of them in phase A.
    std::string cp3detail;
    bool cp3b = check_CP3b(r, &cp3detail);
    pc.cp3 = cp3a && cp3b;
    std::string cp4detail;
    char fireHex[8];
    std::snprintf(fireHex, sizeof fireHex, "%02X", r.fire_byte);
    checks.expect(pc.cp3, "CP3 " + cfgLabel + ": no trigger is presented outside the schedule -- (a) the "
        "CIAAPRA read of phase A shows bits 6 and 7 = 1 (measured FIRE=$" + std::string(fireHex) +
        "); (b) by the record of every write to the input ports (" + std::to_string(r.portlog.size()) +
        " writes), every interval in which pin 6 of either port was closed lasts exactly 4 colour clocks and "
        "none lies in phase A" + (cp3detail.empty() ? "" : (" (" + cp3detail + ")")));

    std::vector<int64_t> spec;
    if (vs_spec) spec = spec_cycles_BC(r);
    cp4detail.clear();
    pc.cp4 = check_CP4(r, &cp4detail, vs_spec ? &spec : nullptr);
    checks.expect(pc.cp4, "CP4 " + cfgLabel + ": inputs are delivered as scheduled -- by the harness's own "
        "record (" + std::to_string(r.applied.size()) + " events), every change landed on its scheduled "
        "cycle exactly" + std::string(vs_spec ? ", on the cycles of the SPECIFICATION.md section 6 schedule," : "") +
        " no event changed more than one input bit, and every phase A-C pulse was exactly 4 "
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
    int lv1 = -1, lv2 = -1;
    if (m_order) {
        r.model.geo_at(r.m1cyc, &lv1, nullptr);
        r.model.geo_at(r.m2cyc, &lv2, nullptr);
        bplconAtLine150 = (lv1 == 150 || lv1 == 151) && (lv2 == 150 || lv2 == 151);
    }
    pc.cp5 = m_order && r.done_ok && covA && covB && covC && covD && bplconAtLine150;
    std::string m1line = m_order ? std::to_string(lv1) : std::string("n/a");
    std::string m2line = m_order ? std::to_string(lv2) : std::string("n/a");
    checks.expect(pc.cp5, "CP5 " + cfgLabel + ": the program ran as specified -- M0..M4 appear in order and "
        "DONE is reached; "
        "the BPLCON0 writes fall at model line 150 (measured M1 line=" + m1line +
        ", M2 line=" + m2line +
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
    std::string a3detail;
    bool a3 = check_A3(r, cp6lo, cp6hi, &a3detail);
    bool cp6 = a3 && check_A4(r, cp6lo, cp6hi);
    pc.cp6 = cp6;
    checks.expect(pc.cp6, "CP6 " + cfgLabel + ": the beam keeps its timing in phases B and C (A3: exactly four "
        "refresh accesses on every model line from B0 to M3, p.194, at the same H positions in every line; A4: "
        "every vsync onset in that span sits at the same model position)");
    checks.note(cfgLabel + " CP6 A3: " + a3detail + "; A3 " + (a3 ? "holds" : "fails"));

    return pc;
}

// ---------------------------------------------------------------------
// P10 (b), one function for the main run and for every contrast of it
// (SPECIFICATION.md 9, P10): with bit 3 clear, every sample of the phase-A
// and phase-C windows is current. A: VB of every phase-A field whose
// preceding field end comes after M0 (fields 1 to the one before the last,
// by the model's own numbering). C: C1 from Vrun to L - 1 and after the
// C1 pulse, all lines of C2, and VB(C3).
// ---------------------------------------------------------------------
const char* const kP10bText = "with bit 3 clear, every sample of the phase-A and phase-C windows is current "
    "(no phase-A field end latches, the C1 pulse is not captured, and neither the C1 nor the C2 field end latches)";
bool grade_p10b(const std::vector<Sample>& A, const std::vector<Sample>& C, const BeamModel& m, int64_t C0_idx,
                int Vrun) {
    bool p10b_A = true;
    {
        // phase-A field ends after M0: use fields 1..(nfields-2) of A (skip cut ends)
        // A's own field boundaries are relative to the SAME model anchor.
        int64_t startField = m.field_index_of(A.empty() ? 0 : A.front().a) + 1;
        int64_t endField = m.field_index_of(A.empty() ? 0 : A.back().a);
        for (int64_t f = startField; f < endField; ++f) {
            auto vb = window_VB(A, m, f);
            if (!vb.empty() && !all_current(vb, m)) p10b_A = false;
        }
    }
    // phase C: C1 pulse not captured, C1/C2 no latch, C2 full field current, VB(C3) current
    auto runC1 = window_RUN(C, m, C0_idx + 1, Vrun);
    bool p10b_C1 = all_current(runC1, m);
    auto postC1 = window_POST(C, m, m.cycle_of(C0_idx + 1, 100, 100) + 4 * m.colour_clock_len, C0_idx + 2);
    if (!postC1.empty()) p10b_C1 = p10b_C1 && all_current(postC1, m);
    auto fullC2 = filter_by_a(C, m.field_start(C0_idx + 2), m.field_start(C0_idx + 3));
    bool p10b_C2 = all_current(fullC2, m);
    auto vbC3 = window_VB(C, m, C0_idx + 3);
    bool p10b_C3 = all_current(vbC3, m);
    return p10b_A && p10b_C1 && p10b_C2 && p10b_C3;
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
// the combined P10 = P10(a) && P10(b) (SPECIFICATION.md section 2). P13 is
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
    {"P8",  "p239",      "opus:8-lpen.c39 (P8); sol:8-pen.c39 (P8(release-at-20))"},
    {"P9",  "p238",      "opus:8-lpen.c12 sol:8-pen.c12"},
    {"P10", "p239,p229", "opus:8-lpen.c35 sol:8-pen.c35 opus:8-regs.c16 sol:8-reg.c18 (the combined P10 only)"},
    {"P11", "p239",      "opus:8-lpen.c43 sol:8-pen.c41 opus:8-lpen.c47 sol:8-pen.c44 opus:8-lpen.c48 sol:8-pen.c45 sol:8-pen.c46"},
    {"P12", "p239",      "opus:8-lpen.c52"},
};
static const PropIds kPropIdsP13 = {"P13", "p238", "opus:8-lpen.c10 opus:8-lpen.c11 sol:8-pen.c10 sol:8-pen.c11 (the combined P13 only)"};

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
    bool p13_port2_shown = false, p13_port2_evaluated = false, p2_graded = false;
    auto pulses = phaseB_pin6_pulses();
    std::map<std::string, PulseTiming> timing;
    std::map<std::string, PulseDef> def;
    for (const auto& p : pulses) { timing[p.tag] = pulse_timing(r.model, r.B0_idx, p); def[p.tag] = p; }
    const std::vector<int> notrig_fields = {1, 2, 3, 20};
    const int64_t u_cck = r.model.u_cck;

    // P3 (a) and (b) for one pulse, on the constant value of its POST
    // window: V is the model line of the onset; H lies in
    // [H_ref(t_on), H_ref(t_on) + 4 + u], H_ref taken at b (5.3, 9 P3).
    auto capture_v_ok = [&](const PulseDef& p, const std::vector<Sample>& post) {
        return !post.empty() && post[0].V == p.line;
    };
    auto capture_h_ok = [&](const PulseTiming& t, const std::vector<Sample>& post) {
        if (post.empty()) return false;
        int href = r.model.href_b(t.t_on);
        return post[0].H >= href && post[0].H <= href + 4 + (int)u_cck;
    };

    // SPECIFICATION.md section 3 ("ECS."): an ECS main run uses prog_ecs_pal /
    // prog_ecs_ntsc, which clear BEAMCON0 bit 13 (LPENDIS) and write bit 5 as
    // the configuration's standard before BPLCON0 bit 3 is set, so every
    // proposition is graded in ECS as in OCS. `ecs` here is true only for a
    // run of a program that never writes BEAMCON0 on an ECS configuration (a
    // contrasting run of a precondition, prog.bin): whether such a part shows
    // the latch rests on LPENDIS's unstated reset value (p.305), so the
    // propositions that need a latch are recorded in that run.
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
    bool gateActive = ecs;
    if (ecs) checks.note(cfgLabel + ": " + (noLatchAtAll ? "every phase-B sample measured current (no pen capture, "
        "no end-of-field latch)" : "a latch is visible in phase B (a sample is not current)") +
        " -- this program never writes BEAMCON0, so SPECIFICATION.md section 3's ECS/LPENDIS record applies to P1-P9, "
        "P10, P10(a), P11, P12 below");
    // One graded line, or, in the ECS exception, one record without claim
    // ids: the claims are graded in OCS, not declined here.
    auto gate = [&](bool cond, const std::string& head, const std::string& page, const std::string& ids,
                    const std::string& text) {
        if (gateActive) checks.record(head + " " + cfgLabel + " " + page +
            ": not graded: this program never writes BEAMCON0 on an ECS configuration, and p.305 gives LPENDIS no reset value");
        else checks.expect(cond, head + " " + cfgLabel + " " + page + (ids.empty() ? "" : " " + ids) + ": " + text);
    };

    // ---- P1 ----
    // A pulse counts as captured when its POST window is one frozen value
    // that is not current and sits at the position P3 (a) and (b) require
    // for that pulse.
    auto postB4 = window_POST(r.B, r.model, timing["B4"].t_off, timing["B4"].field_idx + 1);
    auto postB5 = window_POST(r.B, r.model, timing["B5"].t_off, timing["B5"].field_idx + 1);
    bool b4_captured = all_equal_raw(postB4) && none_current(postB4, r.model) &&
                       capture_v_ok(def["B4"], postB4) && capture_h_ok(timing["B4"], postB4);
    bool b5_captured = all_equal_raw(postB5) && none_current(postB5, r.model) &&
                       capture_v_ok(def["B5"], postB5) && capture_h_ok(timing["B5"], postB5);
    bool p1 = b4_captured || b5_captured;
    gate(p1, "P1", "p238,p227", "opus:8-lpen.c1 sol:8-pen.c1 opus:8-intro.c12 sol:8-overview.c6",
        "a pulse on pin 6 of at least one controller port is captured as a light-pen trigger (POST(B4) or "
        "POST(B5) is one frozen value, not current, at the position P3 (a) and (b) require)");
    checks.record("P1 " + cfgLabel + ": which port captures is not graded (10.4) -- measured: port1(B4) captured=" +
        std::string(b4_captured ? "yes" : "no") + ", port2(B5) captured=" + std::string(b5_captured ? "yes" : "no"));

    // ---- P2 ----
    // RUN(B18) runs from Vrun to the pin-6 pulse at line 160 and contains the
    // pulses on pins 1-4 and 9 of both ports and on pin 5 of both ports
    // (port 1 through mouse_buttons bit 2, port 2 through joystick0 bit 6).
    // The comparator is the same for every pulse in it. Whether port 2's
    // pin-5 pulse reaches the chip is not shown by this window: P13(port2)
    // observes the chip's response to that input, and the record printed
    // after P13 says which way it went.
    auto runB18 = window_RUN(r.B, r.model, r.B0_idx + 18, Vrun, timing["B18"].t_on);
    bool p2i = all_current(runB18, r.model);
    auto postB18 = window_POST(r.B, r.model, timing["B18"].t_off, r.B0_idx + 19);
    bool p2ii = all_equal_raw(postB18) && none_current(postB18, r.model) &&
                capture_v_ok(def["B18"], postB18) && capture_h_ok(timing["B18"], postB18);
    bool p2 = p2i && p2ii;
    gate(p2, "P2", "p238,p228", "opus:8-lpen.c16 sol:8-pen.c17 opus:8-tbl8-1.c20 sol:8-pin.c22",
        "pin 6 is the beam-trigger line and the driven population of the other signal pins is not ((i) RUN(B18) "
        "up to the pin-6 pulse stays current, (ii) the pin-6 pulse itself is captured, at the position P3 (a) and "
        "(b) require)");
    gate(p2i, "P2(i)", "p238,p228", "", "none of the driven other-pin pulses in RUN(B18) is captured "
        "(component of the combined P2 assertion; no independently-sufficient id)");
    gate(p2ii, "P2(ii)", "p238,p228", "", "the pin-6 pulse in B18 is captured, at the position P3 (a) and (b) "
        "require (component of the combined P2 assertion; no independently-sufficient id)");
    p2_graded = !gateActive;

    // ---- P3 ----
    bool p3a = true, p3b = true;
    std::vector<int64_t> offsets;
    std::string p3detail; // per graded pulse: the captured V, H and the offset d, or why there is none
    int graded_n = 0;
    for (const auto& p : pulses) {
        if (!p.graded) continue;
        if ((p.tag == "B4" && !b4_captured) || (p.tag == "B5" && !b5_captured)) continue;
        ++graded_n;
        const auto& t = timing[p.tag];
        auto post = window_POST(r.B, r.model, t.t_off, t.field_idx + 1);
        if (post.empty() || !all_equal_raw(post)) {
            p3a = false; p3b = false;
            p3detail += p.tag + ":" + (post.empty() ? "no-sample" : "not-one-value") + " ";
            continue;
        }
        if (!capture_v_ok(p, post)) p3a = false;
        // (b) H(P) must lie within beam([t_on,t_off]) widened by u: the
        // window [H_ref(t_on), H_ref(t_on) + 4 + u] of 9 P3, H_ref taken at
        // b (5.3), is the pulse's own 4-colour-clock duration plus the
        // model uncertainty, not a tolerance granted to the implementation.
        // A stable capture at an H outside this window is graded red here
        // even if (c)'s offsets still agree.
        if (!capture_h_ok(t, post)) p3b = false;
        offsets.push_back((int64_t)post[0].H - r.model.href_b(t.t_on));
        p3detail += p.tag + ":V" + std::to_string(post[0].V) + "H" + std::to_string(post[0].H) + "d" +
                    std::to_string(offsets.back()) + " ";
    }
    // (c) Exactness requires one valid offset for EVERY graded pulse: a
    // pulse (a)/(b) already dropped for an empty or varying POST must not
    // silently shrink the population (c) compares against (SPECIFICATION.md
    // section 9, P3(c)) -- offsets.size() short of graded_n is itself a
    // failure, not merely "no offsets recorded".
    bool p3c = !offsets.empty() && offsets.size() == (size_t)graded_n;
    if (p3c) for (auto o : offsets) if (o != offsets[0]) p3c = false;
    bool p3 = p3a && p3b && p3c;
    // P3 id placement (SPECIFICATION.md section 9, P3): opus:8-lpen.c17,
    // opus:8-lpen.c18 and sol:8-pen.c18 name neither axis alone and need
    // (a), (b) and (c) together; sol:8-pen.c19 ("the horizontal beam value
    // at the instant") needs (b) and (c) together. All four sit on one
    // combined P3 assertion, not on the separate component lines below.
    // sol:8-pen.c20 ("the vertical beam value at the instant") is
    // V-specific and independently sufficient from (a) alone, so it stays
    // on that component line; (b) and (c) have no id that either
    // establishes alone.
    gate(p3, "P3", "p238", "opus:8-lpen.c17 opus:8-lpen.c18 sol:8-pen.c18 sol:8-pen.c19",
        "a trigger captures the beam position exactly at its instant ((a) V equals the model line, (b) H lies "
        "within [H_ref(t_on), H_ref(t_on)+4+u], (c) the offset is the same for every graded pulse)");
    gate(p3a, "P3(a)", "p238", "sol:8-pen.c20", "every graded pulse's captured V equals the model line at its onset");
    gate(p3b, "P3(b)", "p238", "", "every graded pulse's captured H lies within [H_ref(t_on), H_ref(t_on)+4+u] "
        "(component of the combined P3 assertion; no independently-sufficient id)");
    gate(p3c, "P3(c)", "p238,p239", "", "the offset d(p)=H(P)-H_ref(t_on) is the same for every graded pulse, "
        "with one valid offset required for each (component of the combined P3 assertion; no "
        "independently-sufficient id)");
    checks.note(cfgLabel + " P3: graded population size=" + std::to_string(graded_n));
    checks.note(cfgLabel + " P3: captured V, H and offset d=H-H_ref(t_on) per graded pulse: " + p3detail);

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
        int href = r.model.href_b(t.t_on);
        for (const auto& s : post) {
            int v8 = (s.raw32 >> 16) & 1;
            int hi = (s.raw32 >> 8) & 0xFF;
            int lo8 = s.raw32 & 0xFF;
            // Absence of a validated P3(c) offset fails H8-H1's own check
            // rather than skipping it: H is part of the p.238 register
            // layout and the P4 label, so it must be graded (or fail)
            // whenever no offset can validate it, never silently pass.
            bool loOk = haveOffset && (lo8 == ((href + offset0) & 0xFF));
            if (v8 != c.wantV8 || hi != c.wantHi || !loOk) { p4 = false; break; }
        }
    }
    gate(p4, "P4", "p229,p238", "opus:8-regs.c18 opus:8-regs.c20 sol:8-reg.c20 sol:8-reg.c22",
        "VPOSR holds V8, VHPOSR holds V7-V0 and H8-H1 of the captured pen position (V8 = bit 0/raw bit 16; V7-V0 "
        "the expected line's low byte; H8-H1 = H_ref(t_on) + P3(c)'s offset)");

    // ---- P5 ----
    // LOF (VPOSR bit 15) while latched is left open by the pages
    // (SPECIFICATION.md 10.8): a mismatch confined to that one bit is
    // reported separately from an ordinary freeze failure.
    bool p5 = true;
    std::string p5lof;
    for (const auto& p : pulses) {
        if (!p.graded) continue;
        // p.229 leaves which controller port carries the pen to the model
        // (SPECIFICATION.md section 3, "Port mapping"; P1's own point 4).
        // B4 and B5 are single-port pulses: grade the one P1 found latching
        // on at least one port, not both unconditionally -- a conforming
        // implementation with only its selected port active could
        // otherwise fail this on the port it never uses.
        if ((p.tag == "B4" && !b4_captured) || (p.tag == "B5" && !b5_captured)) continue;
        const auto& t = timing[p.tag];
        auto post = window_POST(r.B, r.model, t.t_off, t.field_idx + 1);
        if (!all_equal_raw(post)) {
            p5 = false;
            bool onlyLof = !post.empty();
            for (auto& x : post) if ((x.raw32 & ~0x80000000u) != (post[0].raw32 & ~0x80000000u)) onlyLof = false;
            if (onlyLof) p5lof += p.tag + " ";
        }
    }
    gate(p5, "P5", "p239", "opus:8-lpen.c36 sol:8-pen.c36", "after a trigger VPOSR is frozen until the release "
        "(every sample of POST(p) is equal, for every graded pulse)");
    checks.record("P5 " + cfgLabel + ": which single-port pulse (B4 port1 or B5 port2) is graded follows P1's own "
        "measured capture -- port1(B4) graded=" + std::string(b4_captured ? "yes" : "no") + ", port2(B5) graded=" +
        std::string(b5_captured ? "yes" : "no"));
    if (!p5lof.empty()) checks.note(cfgLabel + " P5: differences confined to VPOSR bit 15 (LOF, 10.8) in "
        "POST windows of pulses " + p5lof);

    // ---- P6 ----
    // Positions are compared, not whole words (10.8), and torn samples are
    // left out of the equality as they are of the currency check.
    bool p6 = true;
    auto p6check = [&](const std::vector<Sample>& post) {
        if (post.empty()) { p6 = false; return; } // an empty window is not a passed one
        if (!all_equal_pos(post, r.model) || !none_current(post, r.model)) p6 = false;
    };
    for (const auto& p : pulses) {
        if (!p.graded) continue;
        // Same single-port rule as P5 (SPECIFICATION.md section 3, "Port
        // mapping"; P1's point 4): grade B4/B5 only for the port P1 found.
        if ((p.tag == "B4" && !b4_captured) || (p.tag == "B5" && !b5_captured)) continue;
        const auto& t = timing[p.tag];
        int64_t lo = t.t_off + line_len_clk(r.model);
        p6check(filter_ab(r.B, lo, r.model.line_start(t.field_idx + 1, 20)));
    }
    for (int k : notrig_fields) p6check(window_VB(r.B, r.model, r.B0_idx + k + 1));
    gate(p6, "P6", "p239", "opus:8-lpen.c38 sol:8-pen.c38", "while latched, VPOSR never shows the current beam "
        "position (every latched window is one position and no sample in it is current)");
    checks.record("P6 " + cfgLabel + ": which single-port pulse (B4 port1 or B5 port2) is graded follows P1's own "
        "measured capture -- port1(B4) graded=" + std::string(b4_captured ? "yes" : "no") + ", port2(B5) graded=" +
        std::string(b5_captured ? "yes" : "no"));

    // ---- P7 ----
    bool p7a = true, p7b = true, p7c = true;
    int p7cN = 0;        // P7(c)'s graded population: RUN(k) and VB(k+1) both
                         // non-empty AND VB(k+1) is one consistent position.
    int p7cSkipped = 0;  // same population, but VB(k+1) was not one consistent
                         // position (P7(b) already failed there), so F_k has
                         // no single value to compare against m(R_k).
    for (int k : notrig_fields) {
        int64_t fk = r.B0_idx + k;
        auto run = window_RUN(r.B, r.model, fk, Vrun);
        if (!all_current(run, r.model)) p7a = false;
        auto vb = window_VB(r.B, r.model, fk + 1);
        bool vbOk = all_equal_pos(vb, r.model);
        const Sample* Fk = first_untorn(vb, r.model);
        if (!vbOk) {
            p7b = false;
        } else if (Fk->V != r.model.L) {
            p7b = false;
        }
        // The population for (c) is "RUN(k) and VB(k+1) both non-empty for a
        // no-trigger field k" (SPECIFICATION.md section 9, P7): whether that
        // population is non-empty does not depend on whether (b) itself
        // passed, only on whether there are samples to compare at all.
        if (!run.empty() && !vb.empty()) {
            if (vbOk) {
                ++p7cN;
                int m_last = 256 * run.back().V + run.back().H;
                int m_Fk = 256 * Fk->V + Fk->H;
                if (m_Fk < m_last) p7c = false;
            } else {
                ++p7cSkipped;
            }
        }
    }
    bool p7 = p7a && p7b;
    gate(p7, "P7", "p239", "opus:8-lpen.c37 sol:8-pen.c37", "without a trigger, the beam counters latch at the end "
        "of the field ((a) RUN(k) stays current, (b) VB(k+1) is one position at line L)");
    if (p7cN == 0 && p7cSkipped == 0) {
        checks.record("P7(c) " + cfgLabel + " p239: not graded -- the population (RUN(k) and VB(k+1) both "
            "non-empty, for a no-trigger field k) is empty this run");
    } else if (p7cN == 0) {
        checks.record("P7(c) " + cfgLabel + " p239: not evaluated because P7(b) failed for every field with "
            "a population (" + std::to_string(p7cSkipped) + " of " + std::to_string(notrig_fields.size()) +
            "): VB(k+1) is not one consistent position there, so F_k has no single value to compare against "
            "m(R_k)");
    } else {
        gate(p7c, "P7(c)", "p239", "", "m(F_k) >= m(R_k) for every graded no-trigger field" +
            (p7cSkipped ? (" -- " + std::to_string(p7cSkipped) + " field(s) not evaluated because P7(b) "
                "failed for them") : std::string()));
    }

    // ---- P8 ----
    // (a) VB(j) is one position AND it equals the position the source latch
    // froze: the capture's own POST(p) position (P5), for a capture source,
    // or is left as the single internal-consistency check for a
    // no-trigger field end (already fully established by P7's own F_k).
    // (b) RUN(j) runs from Vrun to the first pin-6 pulse of j from the
    // release line on, or to L_j-1 if there is none (SPECIFICATION.md
    // section 9, P8).
    // (c, sol:8-pen.c39) the held value is still visible up to the model's
    // V = 20 and the live counter is visible from u colour clocks after it
    // -- p.239's own "vertical position 20" is standard-independent (the
    // pages that disagree, 10.1, describe general video/display blanking,
    // not this light-pen-specific internal latch), so this is graded the
    // same way in PAL and NTSC.
    bool p8a = true, p8b = true, p8c = true;
    int p8cN = 0;
    // The latched window that follows each pulse of fields B4 to B19: the
    // capture of B19c (field B19) latches VB(B20) too.
    std::vector<std::string> capture_tags = {"B4","B5","B6","B7","B8","B9","B10","B11","B12","B13","B14a","B15b","B17b","B18","B19c"};
    // P8's own no-trigger source set additionally includes B16 (P7 leaves
    // its end out only because VB(B17) also holds the line-19 pulse P9
    // grades, so a P7 red there could come through P9's clause instead --
    // that reason does not apply to P8, a different proposition).
    const std::vector<int> p8_notrig_fields = {1, 2, 3, 16, 20};
    std::string reported;
    // The cutoff is "the first pin-6 pulse of j from line 20 on" (not only
    // 5.5's narrow "graded" population of line >= 32 pulses): a pulse
    // still fully inside vertical blanking (line < 20) cannot end RUN,
    // since it lands before the release and (P5) cannot overwrite a freeze
    // already in place, but ANY pulse from the release line on -- the
    // boundary pulse B19c -- is a real trigger that ends the field's
    // unlatched span the same way a graded one would, even though it sits
    // before Vrun itself. Where such an early pulse preempts the whole
    // RUN(j) window (cutoff at or before Vrun), the window comes back empty;
    // that is a structural consequence of the preemption, not missing
    // data, so it is graded as satisfied below rather than through
    // all_current's own empty-is-false default (reserved for a window that
    // should have samples and does not).
    auto first_graded_pulse_cutoff = [&](int64_t j) -> std::optional<int64_t> {
        std::optional<int64_t> cutoff;
        for (const auto& p2 : pulses) {
            if (p2.line < 20) continue;
            if ((p2.tag == "B4" && !b4_captured) || (p2.tag == "B5" && !b5_captured)) continue;
            const auto& t2 = timing[p2.tag];
            if (t2.field_idx == j && (!cutoff || t2.t_on < *cutoff)) cutoff = t2.t_on;
        }
        return cutoff;
    };
    // The actual held-to-current transition for field j: scans VB(j) (lines
    // 0-19) then lines 20 to L-1 in order, returning the first sample that
    // matches the live beam -- not merely vb[0] (which is simply the first
    // sample of the blanking window, almost always still held).
    auto find_transition = [&](int64_t j) -> std::optional<std::pair<int,int>> {
        for (const auto& s : window_VB(r.B, r.model, j))
            if (!r.model.is_torn(s) && r.model.is_current(s)) return std::make_pair(s.V, s.H);
        int64_t lo = r.model.line_start(j, 20);
        int64_t hi = r.model.line_start(j, r.model.L);
        for (const auto& s : filter_ab(r.B, lo, hi))
            if (!r.model.is_torn(s) && r.model.is_current(s)) return std::make_pair(s.V, s.H);
        return std::nullopt;
    };
    // (c) for field j: no sample of VB(j) is current (the held value is
    // still visible before the model's V = 20), and every sample of line 20
    // that starts u colour clocks after it and ends before the first pulse
    // of the field is current (the release). Both populations must be
    // non-empty: the window between them is the release's own uncertainty
    // (5.3), and is not graded.
    auto grade_release_at_20 = [&](int64_t j, const std::vector<Sample>& vb) {
        ++p8cN;
        if (!none_current(vb, r.model)) p8c = false; // held value must not be the live beam
        int64_t lo = r.model.line_start(j, 20) + u_cck * r.model.colour_clock_len;
        int64_t hi = r.model.line_start(j, 21);
        if (auto cut = first_graded_pulse_cutoff(j)) hi = std::min(hi, *cut);
        auto l20 = filter_ab(r.B, lo, hi);
        if (!all_current(l20, r.model)) p8c = false; // released by line 20, not later
    };
    for (const auto& tag : capture_tags) {
        auto it = timing.find(tag);
        if (it == timing.end()) continue;
        bool wasCaptured = true;
        if (tag == "B4") wasCaptured = b4_captured;
        if (tag == "B5") wasCaptured = b5_captured;
        int64_t j = it->second.field_idx + 1;
        auto vb = window_VB(r.B, r.model, j);
        // (a)'s consistency half is required unconditionally -- "the ends of
        // B4 and B5 are latched either way" (SPECIFICATION.md section 9, P8):
        // whichever port did not capture still sees no trigger that field,
        // so P7's own field-end latch applies to it instead.
        if (!vb.empty()) {
            if (!all_equal_pos(vb, r.model)) p8a = false;
            if (wasCaptured) {
                auto post = window_POST(r.B, r.model, it->second.t_off, j);
                const Sample* f0 = first_untorn(post, r.model);
                const Sample* v0 = first_untorn(vb, r.model);
                if (!f0 || !v0 || f0->V != v0->V || f0->H != v0->H) p8a = false;
            }
        }
        auto run = window_RUN(r.B, r.model, j, Vrun, first_graded_pulse_cutoff(j));
        if (!run.empty() && !all_current(run, r.model)) p8b = false;
        grade_release_at_20(j, vb);
        auto tr = find_transition(j);
        reported += tag + ":" + (tr ? ("L" + std::to_string(tr->first) + "H" + std::to_string(tr->second)) : "none") + " ";
    }
    for (int k : p8_notrig_fields) {
        int64_t j = r.B0_idx + k + 1;
        auto vb = window_VB(r.B, r.model, j);
        if (!vb.empty() && !all_equal_pos(vb, r.model)) p8a = false;
        auto run = window_RUN(r.B, r.model, j, Vrun, first_graded_pulse_cutoff(j));
        if (!run.empty() && !all_current(run, r.model)) p8b = false;
        grade_release_at_20(j, vb);
    }
    bool p8 = p8a && p8b && p8c;
    gate(p8, "P8", "p239", "opus:8-lpen.c39", "every latch holds until the end of internal vertical blanking and "
        "no longer ((a) VB(j) is one position equal to the position the source latch froze, (b) RUN(j) up to the "
        "first pin-6 pulse of j from the release line on is current, (c) the release is at vertical position 20: "
        "the held value is visible before the model's V = 20 and the live counter from u colour clocks after it)");
    gate(p8c, "P8(release-at-20)", "p239", "sol:8-pen.c39", "the held value is still visible up to the model's "
        "V = 20 and the live counter is visible from u colour clocks after it, in every latched window ((c) "
        "of P8; " + std::to_string(p8cN) + " windows)");
    checks.note(cfgLabel + " P8 measured first-CURRENT (live) line/H per window: " + reported);

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
    gate(p9, "P9", "p238", "", "the capture circuitry re-arms by itself as vertical blank ends ((a) no hold-over "
        "past the release, (b) a pulse after blank is captured, (c) re-arming repeats every field)");
    gate(p9a, "P9(a)", "p238", "", "no hold-over past the release: RUN(B15) and RUN(B17) are current, so neither "
        "the line-10 nor the line-19 pulse is held over the release (component of P9(release); no "
        "independently-sufficient id)");

    // (d) the boundary itself: B19's only pulse (B19c) sits exactly at the
    // stated release line (vertical position 20, the same line P8 (c)
    // grades), well before Vrun. Grading whether THIS pulse is captured --
    // the first eligible position -- observes the re-arming instant
    // directly. The window runs to line 200 of the same field, inside
    // phase B's dense, fully-covered early fields.
    const auto& tB19c = timing["B19c"];
    int64_t b19c_hi = r.model.line_start(tB19c.field_idx, 200);
    auto postB19c = filter_ab(r.B, tB19c.t_off, b19c_hi);
    // The held value must also be the capture of B19c itself: V is the model
    // line of its onset and H lies in [H_ref(t_on), H_ref(t_on)+4+u], the
    // capture-position oracle P3 uses (capture_v_ok, capture_h_ok).
    bool p9boundary = !postB19c.empty() && all_equal_raw(postB19c) && none_current(postB19c, r.model) &&
                      capture_v_ok(def["B19c"], postB19c) && capture_h_ok(tB19c, postB19c);
    gate(p9boundary, "P9(boundary)", "p238", "", "a pulse exactly at the stated release line (vertical position "
        "20) is captured, as the held position P3 (a) and (b) require for that pulse (component of P9(release); no independently-sufficient id)");
    // "Just as the system exits vertical blank" needs both halves: nothing
    // is held over from before the release (P9(a)) and the pulse at the
    // release is captured (P9(boundary)).
    gate(p9a && p9boundary, "P9(release)", "p238", "opus:8-lpen.c12 sol:8-pen.c12", "the capture circuitry is "
        "enabled just as the system exits vertical blank: the pulses before the release are not held over it "
        "(P9(a)) and the pulse at the release line is captured (P9(boundary))");

    // ---- P10 ----
    bool p10a = p3a && p3b && p7b;
    bool p10b = grade_p10b(r.A, r.C, r.model, r.C0_idx, Vrun);
    gate(p10a && p10b, "P10", "p239,p229", "opus:8-lpen.c35 sol:8-pen.c35 opus:8-regs.c16 sol:8-reg.c18",
        "bit 3 of BPLCON0 enables the latch: with bit 3 set every graded pulse is captured and every no-trigger "
        "field end latches (P10(a)), with bit 3 clear neither happens (P10(b))");
    gate(p10a, "P10(a)", "p239,p229", "",
        "with bit 3 set, every graded pulse is captured and every no-trigger field end latches (via P3(a)(b) "
        "and P7(b)) (component of the combined P10 assertion; no independently-sufficient id)");
    checks.expect(p10b, "P10(b) " + cfgLabel + " p239,p229: " + kP10bText +
        " (component of the combined P10 assertion; no independently-sufficient id)");

    // ---- P11 ----
    int64_t threshold = 0x10500;
    auto excluded_fields = std::set<int64_t>{r.B0_idx, r.B0_idx + 4, r.B0_idx + 5, r.B0_idx + 19};
    auto vb_excluded_fields = std::set<int64_t>{r.B0_idx + 1, r.B0_idx + 5, r.B0_idx + 6, r.B0_idx + 20};
    // m(P) (SPECIFICATION.md 5.1): the value read is the position P3 (a) and
    // (b) require for the pulse: V is the model line of its onset, H lies in
    // [H_ref(t_on), H_ref(t_on) + 4 + u].
    auto at_pen_position = [&](int64_t m, const PulseDef& p) {
        int V = int(m >> 8), H = int(m & 0xFF);
        int href = r.model.href_b(timing[p.tag].t_on);
        return V == p.line && H >= href && H <= href + 4 + (int)u_cck;
    };
    // "A pulse" (SPECIFICATION.md section 9, P11 point 3) is any scheduled
    // trigger, not only a P3-graded one: an ungraded pulse (B4/B5 before
    // P1 decides them, the P9 boundary pulse B19c) still perturbs VPOSR and
    // must be excluded the same way.
    auto overlaps_pulse = [&](const Sample& s) {
        for (const auto& p : pulses) {
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
        // Pairs with a read inside the release's own uncertainty (5.3) are
        // not graded: such a read may show either side of the release.
        int side0 = release_side(s0, r.model), side1 = release_side(s1, r.model);
        if (side0 == 0 || side1 == 0) continue;
        bool equalPair = (s0.raw32 == s1.raw32);
        int64_t m0 = 0x1FFFF & s0.raw32;
        bool truthT = false;
        const PulseDef* truthPulse = nullptr; // the capture the truth traces to, for the value check
        // "Since the last top-of-screen (V=20)" (p.239): if this sample is
        // still in blanking (before the release of its own field), its own
        // field has not reached V=20 yet, so the last top-of-screen was the
        // PREVIOUS field's -- a graded pulse from that earlier field, still
        // frozen through this blanking, still counts.
        int64_t lastTopOfScreen = (side0 > 0)
            ? r.model.line_start(f0, 20)
            : r.model.line_start(f0 - 1, 20);
        // The method itself (step 2, p.239) can only show "triggered" while
        // the freeze is still in place at BOTH reads of the pair: if s0 is
        // still in blanking (frozen) but s1 has already reached the release
        // point, the pair spans the release and the page's own comparison
        // reads "not the same" regardless of any earlier trigger. The truth
        // this pair can test is therefore whether a trigger is active
        // through both reads, not just at s0.
        bool stillFrozenAtS1 = !(side0 < 0 && side1 > 0);
        for (const auto& p : pulses) {
            if (!p.graded) continue;
            const auto& t = timing[p.tag];
            if (t.t_on >= lastTopOfScreen && t.t_on < s0.a && stillFrozenAtS1) {
                truthT = true;
                truthPulse = &p;
            }
        }
        bool answerTriggered = equalPair && (m0 < threshold);
        if (equalPair && m0 == threshold) continue; // not graded
        ++p11n;
        if (truthT && !answerTriggered) p11 = false;
        if (!truthT && answerTriggered) p11 = false;
        if (truthT && answerTriggered && truthPulse && !at_pen_position(m0, *truthPulse)) p11 = false;
    }
    gate(p11n > 0 && p11, "P11", "p239", "opus:8-lpen.c43 sol:8-pen.c41 opus:8-lpen.c47 sol:8-pen.c44 "
        "opus:8-lpen.c48 sol:8-pen.c45 sol:8-pen.c46", "the two-read detection method (read VPOSR twice, compare, "
        "mask, compare with $10500) answers correctly whether the light pen triggered since the last "
        "top-of-screen, and when it does, m equals m(P) of that capture");
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
        if (release_side(s, r.model) >= 0) continue; // the whole interval lies before the release
        if (p12_excluded.count(f)) continue;
        int64_t precedingField = f - 1;
        bool truthT = false;
        const PulseDef* truthPulse = nullptr;
        for (const auto& p : pulses) {
            if (!p.graded) continue;
            const auto& t = timing[p.tag];
            if (r.model.field_index_of(t.t_on) == precedingField) {
                truthT = true;
                truthPulse = &p;
            }
        }
        int64_t m = 0x1FFFF & s.raw32;
        if (m == threshold) continue;
        ++p12n;
        bool answerTriggered = m < threshold;
        if (truthT != answerTriggered) p12 = false;
        if (truthT && answerTriggered && truthPulse && !at_pen_position(m, *truthPulse)) p12 = false;
    }
    gate(p12n > 0 && p12, "P12", "p239", "opus:8-lpen.c52", "the single-read method during vertical blanking "
        "answers correctly whether the light pen triggered in the preceding field, and when it does, m equals "
        "m(P) of that capture");
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
    // bit8_expect0: only the 2-4ms window (port 1 pin 5 driven low);
    // bit12_expect0: only the 5-7ms window (port 2 pin 5 driven low,
    // joystick0 bit 6). Every other graded window expects 1 on both bits.
    struct DWin { int64_t lo, hi; bool bit8_expect0, bit12_expect0; const char* name; };
    std::vector<DWin> dwins = {
        [&]{ auto g = graded_slice(r.M3cyc, t2000); return DWin{g.first, g.second, false, false, "0-2ms"}; }(),
        [&]{ auto g = graded_slice(t2000, t4000); return DWin{g.first, g.second, true, false, "2-4ms"}; }(),
        [&]{ auto g = graded_slice(t4000, t5000); return DWin{g.first, g.second, false, false, "4-5ms"}; }(),
        [&]{ auto g = graded_slice(t5000, t7000); return DWin{g.first, g.second, false, true, "5-7ms"}; }(),
        [&]{ auto g = graded_slice(t7000, t8000); return DWin{g.first, g.second, false, false, "7-8ms"}; }(),
        [&]{ auto g = graded_slice(t8000, t10000); return DWin{g.first, g.second, false, false, "8-10ms"}; }(),
        [&]{ auto g = graded_slice(t10000, t12000); return DWin{g.first, g.second, false, false, "10-12ms"}; }(),
    };
    // Port 1 (bit 8) and port 2 (bit 12) are tracked as two booleans, then
    // combined into P13, which carries the claim ids.
    bool p13_bit8 = true, p13_bit12 = true;
    int n8 = 0, n12 = 0;
    for (const auto& w : dwins) {
        for (const auto& s : r.D) {
            if (s.a < w.lo || s.a >= w.hi) continue;
            if (sample_straddles_change(s)) continue;
            int bit8 = (s.raw16 >> 8) & 1;
            int bit12 = (s.raw16 >> 12) & 1;
            ++n8; ++n12;
            if (bit8 != (w.bit8_expect0 ? 0 : 1)) p13_bit8 = false;
            if (bit12 != (w.bit12_expect0 ? 0 : 1)) p13_bit12 = false;
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
    // p.238: "Regardless of the port used, the light pen design is the same":
    // the pen-pressed switch is on pin 5 of whichever port carries the pen,
    // so the claims need both ports; the ids sit on the combined assertion.
    p13_port2_shown = p13_bit12;
    p13_port2_evaluated = true;
    checks.expect(p13_bit8 && p13_bit12, "P13 " + cfgLabel + " p238 opus:8-lpen.c10 opus:8-lpen.c11 "
        "sol:8-pen.c10 sol:8-pen.c11: the pen-pressed switch on pin 5 reads like the middle mouse button on "
        "either port (P13(port1) and P13(port2))");
    checks.expect(p13_bit8, "P13(port1) " + cfgLabel + " p238: POTGOR bit 8 is 0 while port-1 pin 5 is driven "
        "low, 1 in every other graded window (component of the combined P13 assertion; no independently-sufficient id; " +
        std::to_string(n8) + " samples)");
    checks.expect(p13_bit12, "P13(port2) " + cfgLabel + " p238,p240: POTGOR bit 12 (Table 8-4, DATRX, data for port 2, "
        "pin 5) is 0 while port-2 pin 5 is driven low (joystick0 bit 6), 1 in every other graded window "
        "(component of the combined P13 assertion; no independently-sufficient id; " + std::to_string(n12) + " samples)");
    checks.expect(p13_pin9, "P13(pin9-control) " + cfgLabel + ": POTGOR bits 10 and 14 (pin 9, the right "
        "mouse button pot line) read 0 while pin 9 of both ports is driven low (p.240, p.241's button-read "
        "convention), confirming they track pin 9 and not pin 5's bit 8/12");
    }
    if (p2_graded) {
        std::string why = !p13_port2_evaluated
            ? "P13(port2) was not evaluated in this run"
            : (p13_port2_shown ? "" : "P13(port2) is red in this run");
        if (p13_port2_evaluated && p13_port2_shown)
            checks.record("P2 " + cfgLabel + " p238,p228 (port-2 pin 5): the pulse (joystick0 bit 6, B18 line 120) "
                "was delivered at the boundary (CP4) and the chip responds to that input (P13(port2) is green), "
                "so RUN(B18) of P2(i) covers port 2's pin 5 as it covers the other driven pins");
        else
            checks.record("P2 " + cfgLabel + " p238,p228 (port-2 pin 5): the pulse (joystick0 bit 6, B18 line 120) "
                "was delivered at the boundary (CP4), but its delivery to the chip is not established: " + why +
                ". A green P2(i) therefore does not cover port 2's pin 5; it covers the other driven pulses"
                " only");
    }
}

// ---------------------------------------------------------------------
// main runs (no NMH_MUTATE): four configurations, this core.
// ---------------------------------------------------------------------
void run_config(const std::string& dir, bool ecs, bool ntsc, const std::string& label,
                const std::string& binName = "") {
    // The ECS programs write BEAMCON0 (clear LPENDIS, PAL bit as the
    // configuration's standard) before BPLCON0 bit 3 is set; the OCS
    // programs do not (SPECIFICATION.md section 3).
    std::string bin = dir + "/" + (!binName.empty() ? binName
        : ecs ? (ntsc ? "prog_ecs_ntsc.bin" : "prog_ecs_pal.bin") : "prog.bin");
    ScheduleDFn schedD = [ntsc](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st) {
        schedD_main(m, M3, ev, st, ntsc);
    };
    RunResult r = run_one(bin, ecs, ntsc, kBufBcount, schedB_main, schedC_main, schedD);
    if (!r.loaded || !r.released) {
        checks.expect(false, "FIXTURE(boot) " + label + ": program loaded and CPU released from reset");
        return;
    }
    grade_fixture(r, label);
    if (!r.model.ok || !r.done_ok) {
        checks.record(label + ": the beam model could not be fit from phase A, or the program did not "
            "finish -- CP1-CP6 and P1-P13 are not evaluated for this configuration");
        return;
    }

    Preconds pc = grade_preconditions(r, ntsc, label, /*vs_spec=*/true);
    grade_P1_P13(r, ntsc, /*ecs=*/false, label, pc);
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
    // sol:8-pen.c39 and opus:8-lpen.c12/sol:8-pen.c12 are graded, not
    // declined: p.239 alone speaks of the light-pen-specific internal
    // latch and does not restrict "vertical position 20" to one standard
    // (unlike p.219/p.79's general video/display blanking), so P8 (c)
    // grades the release at line 20 and P9 (d) grades a pulse there
    // directly (SPECIFICATION.md section 1).
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
// A contrasting run: one simulation, graded with the main run's code
// (grade_preconditions, grade_P1_P13) and nothing else. Every line of it is
// a raw line of the DUT the binary was built against. `ecs` is the harness
// configuration; the programs of the contrasting runs never write BEAMCON0,
// so an ECS run of them records the latch propositions (section 3).
// vsSpec: CP4 also compares the delivered cycles with the SPECIFICATION.md
// section 6 schedule.
RunResult run_and_grade(const std::string& dir, const std::string& bin, bool ecs, bool ntsc, uint32_t bufBcount,
                    const ScheduleBFn& schedB, const ScheduleCFn& schedC, const ScheduleDFn& schedD,
                    const std::string& label, bool vsSpec = false) {
    RunResult r = run_one(dir + "/" + bin, ecs, ntsc, bufBcount, schedB, schedC, schedD);
    checks.expect(r.loaded && r.released, label + ": program loaded and CPU released from reset");
    checks.expect(r.done_ok, label + ": the program completed (M0 to M4 written in order, DONE reached)");
    if (!r.model.ok || !r.done_ok) {
        checks.record(label + ": the beam model could not be fit, or the program did not finish -- no "
            "further grading for this run");
        return r;
    }
    Preconds pc = grade_preconditions(r, ntsc, label, vsSpec);
    checks.note(label + ": CP1=" + std::to_string(pc.cp1) + " CP2=" + std::to_string(pc.cp2) +
        " CP3=" + std::to_string(pc.cp3) + " CP4=" + std::to_string(pc.cp4) + " CP5=" + std::to_string(pc.cp5) +
        " CP6=" + std::to_string(pc.cp6));
    grade_P1_P13(r, ntsc, ecs, label, pc);
    return r;
}

// K10: pin 9 driven low in the windows the main run drives pin 5 (2-4ms,
// both ports, pin 9 only -- pin 5 is not driven there instead of pin 9).
// The main schedule's own 8-10ms pin-9 window is kept unchanged
// (SPECIFICATION.md section 8: "Keep the main D schedule plus one change"),
// so only P13(port1) is exercised by this change; P13(pin9-control) sees
// the same 8-10ms window it always does.
void schedD_K10(const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st, bool ntsc) {
    add_port2_pin5_window(m, M3, ev, st, ntsc);
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
    add_port2_pin5_window(m, M3, ev, st, ntsc);
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
// K23: port 2 pin 5 (joystick0 bit 6) held low throughout phase D (0-12ms,
// replacing the 5-7ms window); everything else as the main schedule.
void schedD_K23(const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st, bool ntsc) {
    ev.push_back({M3 + 1, [st]() { st->joy0 |= kBitFire3; }});
    int64_t t12 = M3 + us_to_cck(12000, ntsc) * m.colour_clock_len;
    ev.push_back({t12, [st]() { st->joy0 &= (uint8_t)~kBitFire3; }});
    int64_t t2 = M3 + us_to_cck(2000, ntsc) * m.colour_clock_len;
    int64_t t4 = M3 + us_to_cck(4000, ntsc) * m.colour_clock_len;
    ev.push_back({t2, [st]() { st->mbtn |= kMouseMid; }});
    ev.push_back({t4, [st]() { st->mbtn &= (uint8_t)~kMouseMid; }});
    int64_t t8 = M3 + us_to_cck(8000, ntsc) * m.colour_clock_len;
    int64_t t10 = M3 + us_to_cck(10000, ntsc) * m.colour_clock_len;
    ev.push_back({t8, [st]() { st->joy0 |= kBitFire2; st->joy1 |= kBitFire2; }});
    ev.push_back({t10, [st]() { st->joy0 &= (uint8_t)~kBitFire2; st->joy1 &= (uint8_t)~kBitFire2; }});
}
// PIN9CTRL: the main schedule's D phase, but WITHOUT the 8-10ms pin-9 low
// window (pin 9 of both ports stays released/high throughout). p.240's
// Table 8-4 and p.241's button-read convention predict POTGOR bits 10 and
// 14 then stay 1 in that window, contradicting the unchanged P13(pin9-
// control) comparator (which expects both 0 there).
void schedD_PIN9CTRL(const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st, bool ntsc) {
    add_port2_pin5_window(m, M3, ev, st, ntsc);
    int64_t t2 = M3 + us_to_cck(2000, ntsc) * m.colour_clock_len;
    int64_t t4 = M3 + us_to_cck(4000, ntsc) * m.colour_clock_len;
    ev.push_back({t2, [st]() { st->mbtn |= kMouseMid; }});
    ev.push_back({t4, [st]() { st->mbtn &= (uint8_t)~kMouseMid; }});
    // pin 9 of both ports: no events at all -- stays released (high)
}

// K13 (SPECIFICATION.md section 8): the phase-B buffer is shortened so
// phase B ends within field B19. The sample rate differs by core at the
// same 60000-sample buffer size (measured: fx68k ends in B19, TG68K,
// the default build, in B17), so the buffer's own fixed size decides where the CPU actually
// stops, while the driver's phase-B event schedule spans further than
// that (out past B19, toward the un-shortened main schedule's B21). This
// runner therefore stops injecting once M2 actually appears, rather than
// running the full precomputed event list and searching for M2
// afterwards. It steps in bounded chunks, scanning the growing log for
// M2, and drops every event after M2 is seen.
void run_K13(const std::string& dir, const std::string& label) {
    nmh::Config cfg = make_config(true, false);
    auto h = std::make_unique<nmh::NanomigHarness>(cfg);
    clear_logs();
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
        state.push(*h, e.cycle);
        g_applied.push_back({e.cycle, (int64_t)h->cycles(), state.joy0, state.joy1, state.mbtn});
    }
    checks.note(label + ": M2 " + std::string(m2seen ? "appeared while still injecting phase-B events "
        "(the rest were dropped)" : "had not appeared once every phase-B event was injected"));
    // M2 may already be in the log (found by scanning it above), not still
    // to come: run_until_write only ever finds a future write, so a write
    // already seen is read back from the log itself, not waited for again.
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
        int endL; model.geo_at((int64_t)*m2cyc, &endL, nullptr);
        checks.note(label + ": M2 written in field B" + std::to_string(r.C0_idx - r.B0_idx) + ", line " +
            std::to_string(endL));
    }
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
    auto donecyc = m4cyc ? h->run_until_write(kDoneAddr, kDoneValue, kRunBudget) : std::nullopt;
    r.done_ok = m4cyc.has_value() && donecyc.has_value();
    checks.expect(r.done_ok, label + ": the program completed (M0 to M4 found, in order, in the full log, "
        "and DONE reached)");
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
    r.portlog = g_portlog;
    if (!r.done_ok) return;
    Preconds pc = grade_preconditions(r, false, label);
    // The unchanged CP5 comparator above (grade_preconditions's own "CP5 ..."
    // line) prints the red this mutation targets: the shortened buffer ends
    // phase B before line 40 of field B21.
    grade_P1_P13(r, false, true, label, pc);
}

// K17 and K18 (SPECIFICATION.md section 8), in OCS/PAL: the program of K4
// (BPLCON0 = $0008 from the start) with a pin-6 pulse (both ports) at
// (200,100) in every phase-A field (K17) or at (Vrun,100), line 29 in PAL,
// in one phase-A field (K18). The positions come from the beam model of a
// first pass of the same program without input changes (5.6: the system is
// deterministic), so each pulse is delivered at its own field's line, in
// the second pass, as the run reaches its cycle. The pass-2 samples are the
// run's own, read from the DUT the binary was built against, and are graded
// with the unchanged CP1 comparators.
void run_K17(const std::string& dir, bool every_field, const std::string& tag) {
    const std::string label = tag + "/OCS/PAL";
    nmh::Config cfg = make_config(false, false);
    const int pulseLine = every_field ? 200 : 29;

    // pass 1: no input changes; fits the model and reports where phase A ends
    BeamModel m;
    int64_t m0cyc1 = -1, m1cyc1 = 0;
    {
        auto h = std::make_unique<nmh::NanomigHarness>(cfg);
        size_t loaded = h->load_program(dir + "/prog_k4.bin");
        bool released = loaded && h->boot(kBootBudget).cpu_released;
        checks.expect(loaded > 0 && released, label + ": program loaded and CPU released from reset (model pass)");
        if (!(loaded && released)) return;
        auto m1 = h->run_until_write(kMark, kM1, kRunBudget);
        checks.expect(m1.has_value(), label + ": phase A completed (model pass)");
        if (!m1) return;
        auto coderd = code_read_cycles(h->ram_log());
        auto A = extract_long_samples(h->ram_log(), coderd, kBufA, kBufA + 4 * kBufAcount);
        m = fit_model(A, h->ram_log(), h->video(), false);
        m1cyc1 = (int64_t)*m1;
        m0cyc1 = find_mark_cycle(h->ram_log(), kM0);
    }
    checks.expect(m.ok, label + ": beam model fit from phase A (model pass)");
    if (!m.ok) return;
    int64_t cck = m.colour_clock_len;

    // the pulses: the line-200 instant of every field of phase A (K17), or
    // of its third field (K18)
    std::vector<int64_t> sched;
    for (int64_t f = m.field_index_of(m0cyc1); f <= m.field_index_of(m1cyc1); ++f) {
        int64_t t = m.cycle_of(f, pulseLine, 100);
        if (t > m0cyc1 && t + 4 * cck < m1cyc1) sched.push_back(t);
    }
    if (!every_field) {
        std::vector<int64_t> one;
        if (sched.size() > 2) one.push_back(sched[2]);
        sched = one;
    }
    std::vector<Event> ev;
    PortState st;
    for (auto t : sched) {
        ev.push_back({t, [&st]() { st.joy0 |= kBitFire; st.joy1 |= kBitFire; }});
        ev.push_back({t + 4 * cck, [&st]() { st.joy0 &= (uint8_t)~kBitFire; st.joy1 &= (uint8_t)~kBitFire; }});
    }

    // pass 2: the same program, each pulse delivered when its cycle is reached
    auto h = std::make_unique<nmh::NanomigHarness>(cfg);
    clear_logs();
    size_t loaded = h->load_program(dir + "/prog_k4.bin");
    bool released = loaded && h->boot(kBootBudget).cpu_released;
    checks.expect(loaded > 0 && released, label + ": program loaded and CPU released from reset");
    if (!(loaded && released)) return;
    run_phase_events(*h, ev, st);
    auto m1 = h->run_until_write(kMark, kM1, kRunBudget);
    checks.expect(m1.has_value() && (int64_t)*m1 == m1cyc1, label + ": phase A completed with the added pulses, "
        "M1 written at the cycle of the model pass");
    if (!m1) return;

    RunResult rr;
    rr.model = m;
    rr.applied = g_applied;
    rr.M3cyc = m1cyc1 + 1; // every change of this run is one of phase A's momentary pulses
    std::string det;
    bool delivered = check_CP4(rr, &det) && rr.applied.size() == 2 * sched.size() && !sched.empty();
    checks.expect(delivered, label + ": every added pulse was delivered at its scheduled cycle, 4 colour clocks "
        "wide (" + std::to_string(sched.size()) + " pulses, " + std::to_string(rr.applied.size()) +
        " changes)" + (det.empty() ? "" : (" (" + det + ")")));

    // The run's own phase-A samples, fitted and graded by the CP1 comparators
    // unchanged. On a build that follows the pages each pulse is captured and
    // VPOSR stays frozen until the end of vertical blanking.
    auto coderd = code_read_cycles(h->ram_log());
    auto A = extract_long_samples(h->ram_log(), coderd, kBufA, kBufA + 4 * kBufAcount);
    if (!A.empty()) {
        BeamModel m2 = fit_model(A, h->ram_log(), h->video(), false);
        std::string c = tag + "/OCS/PAL";
        if (every_field)
            checks.note(c + ": the phase-A samples fit with L=" + (m2.ok ? std::to_string(m2.L) : std::string("n/a")) +
                " (a captured pulse at line 200 in every field leaves 200 as the largest V read)");
        if (m2.ok) grade_cp1(A, m2, false, c, "beam model anchored on the run's own phase-A samples");
        else checks.expect(false, "CP1 " + c + ": no beam model could be fit from the run's phase-A samples");
    }

    // The mutated program still finishes (SPECIFICATION.md section 8): the
    // unchanged phases B-D follow with the standard schedule, from the
    // model of the first pass, and DONE is required.
    int64_t B0_idx = m.field_index_of((int64_t)*m1);
    PortState st2;
    std::vector<Event> evB;
    schedB_main(m, B0_idx, evB, &st2);
    run_phase_events(*h, evB, st2);
    auto m2cyc = h->run_until_write(kMark, kM2, kRunBudget);
    if (m2cyc) {
        int64_t C0_idx = m.field_index_of((int64_t)*m2cyc);
        std::vector<Event> evC;
        schedC_main(m, C0_idx, evC, &st2);
        run_phase_events(*h, evC, st2);
    }
    auto m3cyc = h->run_until_write(kMark, kM3, kRunBudget);
    if (m3cyc) {
        std::vector<Event> evD;
        schedD_main(m, (int64_t)*m3cyc, evD, &st2, false);
        run_phase_events(*h, evD, st2);
    }
    auto m4cyc = h->run_until_write(kMark, kM4, kRunBudget);
    auto donecyc = m4cyc ? h->run_until_write(kDoneAddr, kDoneValue, kRunBudget) : std::nullopt;
    checks.expect(m2cyc.has_value() && m4cyc.has_value() && donecyc.has_value(),
        label + ": the program still finishes (M2-M4 written in order, DONE reached) despite the added phase-A "
        "pulses");
}

void run_mutation(const std::string& dir, const std::string& label) {
    auto schedD_main_pal = [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st) {
        schedD_main(m, M3, ev, st, false);
    };
    // a phase-B schedule: the main one plus `mods`
    auto sched_with = [](const PulseMods& mods) {
        return ScheduleBFn([mods](const BeamModel& m, int64_t B0, std::vector<Event>& ev, PortState* st) {
            schedB_build(m, B0, ev, st, mods);
        });
    };
    if (label == "K1") {
        run_and_grade(dir, "prog_k1.bin", false, false, kBufBcount, schedB_main, schedC_main,
            schedD_main_pal, "K1/OCS/PAL");
    } else if (label == "K3") {
        // OCS, not ECS (SPECIFICATION.md section 8). Phase C writes
        // BPLCON0 = $0008: on a build that follows the pages the C1 pulse is
        // captured and the C1 and C2 field ends latch, so the raw P10(b)
        // comparator (grade_p10b, called by grade_P1_P13) reads them.
        run_and_grade(dir, "prog_k3.bin", false, false, kBufBcount, schedB_main, schedC_main,
            schedD_main_pal, "K3/OCS/PAL");
    } else if (label == "K4") {
        // OCS, not ECS, for the same reason. Phase A writes BPLCON0 = $0008:
        // the phase-A field ends latch, and the raw P10(b) comparator reads
        // the phase-A windows.
        run_and_grade(dir, "prog_k4.bin", false, false, kBufBcount, schedB_main, schedC_main,
            schedD_main_pal, "K4/OCS/PAL");
    } else if (label == "K5") {
        run_and_grade(dir, "prog_k5.bin", true, false, kBufBcountK5, schedB_main, schedC_main,
            schedD_main_pal, "K5/ECS/PAL");
    } else if (label == "K21") {
        run_and_grade(dir, "prog_k21.bin", false, false, kBufBcount, schedB_main, schedC_main,
            schedD_main_pal, "K21/OCS/PAL");
    } else if (label == "K13") {
        run_K13(dir, "K13/ECS/PAL");
    } else if (label == "K14") {
        run_and_grade(dir, "prog_k14.bin", true, false, kBufBcount, schedB_main, schedC_main,
            schedD_main_pal, "K14/ECS/PAL");
    } else if (label == "K2") {
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, sched_with(mods_K2()), schedC_main,
            schedD_main_pal, "K2/OCS/PAL");
    } else if (label == "K6") {
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, sched_with(mods_K6()), schedC_main,
            schedD_main_pal, "K6/OCS/PAL");
    } else if (label == "K7") {
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, sched_with(mods_K7()), schedC_main,
            schedD_main_pal, "K7/OCS/PAL");
    } else if (label == "K8") {
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, sched_with(mods_K8()), schedC_main,
            schedD_main_pal, "K8/OCS/PAL");
    } else if (label == "K9" || label == "K22") {
        // K9 and K22 change the implementation, not the stimulus: the
        // stimulus is the unchanged main schedule (OCS/PAL). The build is
        // named by the record (k9-test-*: pin 1 wired as a trigger on a copy
        // of the latched RTL; refresh3-test-*: one of the four refresh slots
        // removed from a copy of the unpatched RTL).
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, schedB_main, schedC_main,
            schedD_main_pal, label + "/OCS/PAL");
    } else if (label == "K9b") {
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, sched_with(mods_K9b()), schedC_main,
            schedD_main_pal, "K9b/OCS/PAL");
    } else if (label == "K10") {
        run_and_grade(dir, "prog.bin", true, false, kBufBcount, schedB_main, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_K10(m, M3, ev, st, false); }, "K10/ECS/PAL");
    } else if (label == "K10b") {
        run_and_grade(dir, "prog.bin", true, false, kBufBcount, schedB_main, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_K10b(m, M3, ev, st, false); }, "K10b/ECS/PAL");
    } else if (label == "K12") {
        // Every pulse one line later than the schedule says; CP4 grades
        // this run's delivered cycles against the SPECIFICATION.md section 6
        // schedule (vsSpec), not against its own shifted one.
        run_and_grade(dir, "prog.bin", true, false, kBufBcount, sched_with(mods_K12()),
            [](const BeamModel& m, int64_t C0, std::vector<Event>& ev, PortState* st) {
                add_pulse_events(ev, m, C0, st, 1, 101, 100, true, true, kBitFire);
            },
            schedD_main_pal, "K12/ECS/PAL", /*vsSpec=*/true);
    } else if (label == "K16") {
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, sched_with(mods_K16()), schedC_main,
            schedD_main_pal, "K16/OCS/PAL");
    } else if (label == "K19") {
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, sched_with(mods_K19()), schedC_main,
            schedD_main_pal, "K19/OCS/PAL");
    } else if (label == "K20") {
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, sched_with(mods_K20()), schedC_main,
            schedD_main_pal, "K20/OCS/PAL");
    } else if (label == "K15") {
        // ECS chipset, Config forced to NTSC, the schedule of the NTSC run
        // (phase D timed in NTSC microseconds), graded with the PAL run's
        // own expectations (SPECIFICATION.md 8).
        RunResult r = run_one(dir + "/prog.bin", true, true, kBufBcount, schedB_main, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_main(m, M3, ev, st, true); });
        checks.expect(r.loaded && r.released && r.done_ok, "K15/ECS/PAL(as-NTSC): program ran to completion");
        if (r.model.ok) {
            // the ordinary CP1 comparators, fed the PAL expectation; the
            // other preconditions are graded as the NTSC run they are, and
            // P1-P13 under the CP1 that went red
            Cp1As asPal{false, "beam model anchored on phase A, graded as PAL (measured " +
                std::to_string(r.model.L + 1) + " lines, from a run whose Config and phase-D schedule were "
                "switched to NTSC)"};
            Preconds pc = grade_preconditions(r, /*ntsc=*/true, "K15/ECS/PAL(as-NTSC)", false, &asPal);
            grade_P1_P13(r, true, true, "K15/ECS/PAL(as-NTSC)", pc);
        }
    } else if (label == "K17") {
        run_K17(dir, /*every_field=*/true, "K17");
    } else if (label == "K18") {
        run_K17(dir, /*every_field=*/false, "K18");
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
        clear_logs();
        write_ports(*h, kBitFire, kBitFire, 0, -1);
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
                char heldHex[8];
                std::snprintf(heldHex, sizeof heldHex, "%02X", fireHeld);
                checks.note("K11/ECS/PAL: measured FIRE=$" + std::string(heldHex) +
                    " while pin 6 was held closed, before release");
                write_ports(*h, 0, 0, 0, -1);

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
                        auto donecyc = m4cyc ? h->run_until_write(kDoneAddr, kDoneValue, kRunBudget) : std::nullopt;
                        r.done_ok = m4cyc.has_value() && donecyc.has_value();
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
                    r.portlog = g_portlog;
                    checks.expect(r.done_ok, "K11/ECS/PAL: the program completed (M0 to M4 written in order, "
                        "DONE reached)");
                    if (model.ok && r.done_ok) {
                        // The unchanged CP3 comparator (grade_preconditions's
                        // own "CP3 ..." line) prints the red this mutation
                        // targets: pin 6 of both ports held closed from before
                        // boot through one field after M0, then released.
                        // P1-P13 are graded under that precondition.
                        Preconds pc = grade_preconditions(r, false, "K11/ECS/PAL");
                        grade_P1_P13(r, false, true, "K11/ECS/PAL", pc);
                    }
                }
            }
        }
    } else if (label == "PIN9CTRL") {
        // P13(pin9-control): omit the 8-10ms pin-9 low drive. The
        // comparator is unchanged (P13's own p13_pin9 check, graded
        // through the normal grade_P1_P13 path below); this run simply
        // never presents that stimulus, so bits 10/14 should read 1
        // instead of the 0 the comparator expects.
        run_and_grade(dir, "prog.bin", true, false, kBufBcount, schedB_main, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_PIN9CTRL(m, M3, ev, st, false); },
            "PIN9CTRL/ECS/PAL");
    } else if (label == "K23") {
        run_and_grade(dir, "prog.bin", false, false, kBufBcount, schedB_main, schedC_main,
            [](const BeamModel& m, int64_t M3, std::vector<Event>& ev, PortState* st){ schedD_K23(m, M3, ev, st, false); },
            "K23/OCS/PAL");
    } else if (label == "FIXTURE-DONE") {
        // The main run's own path (run_config) on a program that omits DONE
        // after M4 (prog_nodone.s): FIXTURE(DONE) goes red and the run
        // records that CP1-CP6 and P1-P13 are not evaluated.
        run_config(dir, false, false, "FIXTURE-DONE-MUTANT/OCS/PAL", "prog_nodone.bin");
    } else if (label == "FIXTURE") {
        // The FIXTURE comparators of the main run, graded on a program that
        // stores a different value where M0 belongs (prog_fixture.s
        // continues through M1-M4 and DONE).
        RunResult r = run_one(dir + "/prog_fixture.bin", false, false, kBufBcount, schedB_main, schedC_main,
            schedD_main_pal);
        std::string cfgName = label + "-MUTANT/OCS/PAL";
        if (!r.loaded || !r.released) {
            checks.expect(false, "FIXTURE(boot) " + cfgName + ": program loaded and CPU released from reset");
        } else {
            grade_fixture(r, cfgName);
        }
    } else {
        checks.record("NMH_MUTATE=" + label + " names no mutation of this package");
    }
}

} // namespace
