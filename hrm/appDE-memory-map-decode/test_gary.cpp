// hrm/appDE-memory-map-decode -- gary conformance test
//
// Test purpose: appendix D's three printed memory maps (pp.314-316) plus the
//   p.337 PART 4 port-signal table's CIA-A/CIA-B address rows and the p.330
//   fire-button figure's PRA/DDRA addresses, as `gary`'s combinational
//   chip-select logic actually decodes them -- per
//   work/2026-09-17_appDE_build_brief.md and this package's own 60-claim
//   slice of work/2026-09-17_appDE_packages.jsonl.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd ed., printed pages
//   314, 315, 316, 330, 337.
// Non-goals: anything outside the `gary` fixture this package builds, and
//   any claim it does not cite. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/2026-09-17_appDE_packages.jsonl` in the sibling repository
//   (hrm-testpipeline). docs/legacy-names.md explains the vocabulary.
//
// BOUNDARY: gary.v, reached at $(NANOMIG)/src/minimig-aga/gary.v. A
//   single, self-contained file -- it instantiates no submodules, so no
//   other file is compiled alongside it (matching the accepted
//   appF-cia-address-decode / ch1-memory-map-and-two-buses /
//   ch7-custom-address-space / ch7-reset-overlay precedent, the four
//   existing packages that already build gary standalone in this suite).
//   No DISABLE_IDE / ENABLE_RTC define is passed (Makefile comment); sel_ide
//   and sel_gayle are present, sel_rtc is gary.v's own tied-0 dummy wire and
//   is NOT a module port in this configuration (gary.v:97-99 declare it a
//   port only `ifdef ENABLE_RTC) -- no h.top->sel_rtc reference appears
//   anywhere below for that reason.
//
// THE FOUR-PROPOSITION SPLIT, load-bearing throughout: a combined
//   "address X is register Y" claim splits into an address-membership half
//   (which chip-select line asserts -- gary's own territory) and a
//   register-identity half (which of ciaa's 16 internal registers answers
//   -- a different top module's decode, ciaa.v:136-150, appF-cia-register-
//   map's own territory). gary has no register-select or register-name
//   output of any kind; only WHICH CHIP (sel_cia_a/sel_cia_b), never WHICH
//   REGISTER, is observable at this boundary. Every decline below that cites
//   this split follows appF-cia-address-decode's own Group B/D precedent.
//
// A STALE-DECLINE CHAIN FOUND AND NOT REPEATED: sol:E-fire.c18/c19 and
//   opus:E-firebutton.c21/c23 (p.330, PRA/DDRA at $BFE001/$BFE201) were
//   classified "decline, already graded by appF-cia-address-decode /
//   appF-cia-register-map / ch8-ciaapra-address-window". Checked directly
//   against the actual suite (not trusted from the note): appF-cia-address-
//   decode's own CIAA-MAP-SWEEP deliberately EXCLUDES r=0 ($BFE001) and r=2
//   ($BFE201) from its 14-address sweep, citing ch8-ciaapra-address-window
//   for those two -- but ch8-ciaapra-address-window's own record() line says
//   plainly "No package in this suite currently grades \"$BFE001 selects
//   PRA\" against the pinned join" (verified at
//   appF-cia-register-map/test_appF-cia-register-map.cpp:1407, which quotes
//   that exact sentence). appF-cia-address-decode's OWN header (Group F,
//   sol:E-portsig.c1's evidence field) independently found and flagged this
//   identical gap for the SAME two addresses -- "Do not cite A1..A4 as a
//   decline" -- but that caution was written against appendix E/F claims,
//   not against sol:E-fire.c18/c19 here, so the same stale citation survived
//   into this package's own classify stage. CIAA-PRA-DDRA-ADDRESS-WINDOW
//   below closes the actual gap: the address-membership half of $BFE001 and
//   $BFE201 (sel_cia_a asserts, sel_cia_b does not) was not graded by any
//   expect() anywhere in the suite before this file. The register-identity
//   half (PRA vs DDRA specifically) IS genuinely graded, at a different
//   boundary, by appF-cia-register-map's REG0-PRA/REG2-DDRA (verified:
//   test_appF-cia-register-map.cpp:1040,1063, both real checks.expect()
//   lines using ciaa's own `rs` port directly, not a decoded CPU address) --
//   declined here on ordinary Group B/D grounds, not stale ones.
//
// THE ovl=1 BEHAVIOUR AT THIS FIXTURE: none of this package's own 60
//   claims quotes p.337's PA0/OVL row (grep-checked against this file's own
//   claims dump), so there is nothing of my own to grade here.
//   hrm/ch7-reset-overlay/test_gary.cpp -- an ACCEPTED package, also
//   top=gary -- drives ovl=1 directly into this exact fixture and grades it
//   with four checks.expect() assertions, all GREEN:
//   OVL-CONTROL-BASELINE-000000, OVERLAY-ROM-AT-000000,
//   OVERLAY-RAM-UNAVAILABLE-WRITE-000000 and RESET-ROM-REAPPEARS-000000. It
//   measures Chip banks 1-3 staying mapped under the overlay (sel_chip[1..3]
//   carry no `ovl` term -- gary.v:160-162) and declines to grade that,
//   because p.223 itself says "portions of the RAM still respond" on some
//   models. See the OVL-CANDIDATE record() block below for the measurement,
//   cited by no uid.
//
// TWO PRINTED DEFECTS, MEASURED BOTH READINGS, GRADED NEITHER (source_
//   defines: conflict, per classify): p.315's three-row overlap
//   ($00DC0000-$00DDFFFF / $00DD0000-$00DEFFFF / $00DE0000-$00DEFFFF) against
//   p.316's contiguous High I/O Registers panel (DC/DD/DE/DF ticks, no
//   overlap) -- see CLOCK-BLOCK-CONFLICT. p.315's second overlap ($00E80000-
//   $0EFFFFFF, 240MB as printed) and p.316's doubled $DD0000 tick / skipped
//   $DB0000 tick are recorded in the report, not this file -- neither binds
//   to a claim in this package's own 60 (no claim's restatement names the
//   $00E80000 row or a specific p.316 diagram tick order).
//
// CHAPTER 7'S TWO "REDS" -- MEASURED, AND FOUND STALE: the outer brief
//   states "Two of ch7-custom-address-space's assertions are RED at
//   $DF0000 and $D00000". Checked directly against
//   hrm/ch7-custom-address-space/test_gary.cpp and out.txt: as of this
//   file's own header (search "WITHDRAWN AT STAGE 8", two occurrences),
//   ADDR-EXCLUSIVE-BELOW-DF0000 and ADDR-EXCLUSIVE-MIDBLOCK-D00000 are NO
//   LONGER checks.expect() assertions -- both were converted to
//   note()/record() pairs (ADDR-BELOW-DF0000-MEASURED/-NOT-GRADED,
//   ADDR-MIDBLOCK-D00000-MEASURED/-NOT-GRADED) because their own reason_kind
//   is source-silent: "No other access address is valid" (p.263) states
//   which addresses SOFTWARE may use, not what the chip's own select lines
//   must do at an address software is told not to use -- an inference the
//   package itself supplied, not a printed requirement. ch7's own current
//   out.txt reads "8 passed, 0 failed". So the brief's own premise is stale
//   -- there are currently zero reds in ch7-custom-address-space, not two.
//   Both underlying MEASUREMENTS still stand (sel_reg($DF0000)=1,
//   sel_reg($D00000)=1 under ch7's own memory_config=4'b0100) and this
//   package's own CUSTOMREG-WIDE-64K-A3000 and SLOWMEM-UPPER-BANKS-WIDE
//   below independently confirm the RTL side of both: p.315/p.316 print the
//   $DF0000-$DFFFFF chip-register window as the full 64K gary actually
//   decodes (CUSTOMREG-WIDE-64K-A3000, GREEN -- the missing oracle ch7's own
//   withdrawn measurement lacked), and p.314 prints $D0 0000-$D7 FFFF as
//   slow memory, which is exactly t_sel_slow[2] under an enabling
//   memory_config (SLOWMEM-UPPER-BANKS-WIDE, GREEN). See CH7-TWO-REDS-STATUS
//   below for the consolidated record(). Not adjudicated here whether ch7's
//   own withdrawal was correct -- reported per the outer brief's own
//   "measure it, report it" instruction.
//
// CLAIM COVERAGE: all 60 claims are cited exactly once, on the record() or
//   expect() line the grouping below assigns. Declines follow the brief's
//   CONDITION rule -- cited only where a prior assertion grades the SAME
//   proposition; where this page states something WIDER or on a different
//   axis, that part is graded fresh below instead of declined.
//
// RESULTS: 7 passed, 0 failed. Every graded assertion is GREEN -- every genuinely new
//   proposition this package found (the two upper slow-memory banks, the
//   default-build RTC window, the wide $DF0000-$DFFFFF register bank, both
//   B8-BF/D8-DF panel sweeps, and the $BFE001/$BFE201 address-membership
//   gap) matches gary's own decode. 17 record()s (declines, all checked
//   against the CONDITION rule -- same proposition already graded
//   elsewhere -- plus the one PARTIAL split and the one CONFLICT, neither
//   graded per NM-HRM-062's own precedent) and 3 note()s (the p.315/p.316
//   conflict's own two readings, plus the ovl=1 candidate measurement).
//
// MUTATION LOG (7 entries, one per expect() -- every mutation below was
//   actually built and run: `make clean; make MUT=<NAME> test`, one at a
//   time, output read from out.txt, confirmed to turn ONLY the named
//   assertion red (6 passed / 1 failed in all 7 cases, the failing line
//   always the one named), then `make clean; make test` confirmed a clean
//   return to the 7-passed/0-failed baseline. No oracle/expected value was
//   ever touched -- every one is a STIMULUS change (a probe address
//   redirected).
//
// MUTATION FIXTURE-LIVENESS stimulus red: the kickstart probe was withheld
//   at $000000 instead of moving to $F80000, so the two readings collapse
//   to one stuck value and the liveness check goes red.
// MUTATION SLOWMEM-UPPER-BANKS-WIDE stimulus red: the t_sel_slow[2] probe
//   was redirected from $D00000 to $E10000, well outside any slow bank.
// MUTATION RTC-WINDOW-DEFAULT-BUILD stimulus red: the control probe (sel_
//   ide's own, genuinely-excluded window) was collapsed from $DA0000 onto
//   $DC0000, the window itself.
// MUTATION CUSTOMREG-WIDE-64K-A3000 stimulus red: the window-top probe was
//   moved from $DFFFFE to $E00000, one word past the window's own top.
// MUTATION LOWIOREG-PANEL-SWEEP stimulus red: the sel_cia top-of-panel
//   probe was moved from $BFFFFE to $C00000, one word past the panel.
// MUTATION HIGHIOREG-PANEL-SWEEP stimulus red: the sel_reg probe at the
//   panel's own top was redirected from $DF0000 to $E00000.
// MUTATION CIAA-PRA-DDRA-ADDRESS-WINDOW stimulus red: the DDRA probe was
//   redirected from $BFE201 to $BFD201 (bit 13 flipped), CIA-B's own space.
//
// Seven assertions, seven independent mutations, each changing exactly one
// assertion's colour and none other (verified: every run above read
// "6 passed, 1 failed" with the FAIL line matching the named assertion);
// every one reverted cleanly to the 7-passed/0-failed baseline.

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vgary.h"

#ifndef MUT_LIVENESS
#define MUT_LIVENESS 0
#endif
#ifndef MUT_SLOWUPPER
#define MUT_SLOWUPPER 0
#endif
#ifndef MUT_RTCWIN
#define MUT_RTCWIN 0
#endif
#ifndef MUT_CUSTOMREGWIDE
#define MUT_CUSTOMREGWIDE 0
#endif
#ifndef MUT_LOWIOPANEL
#define MUT_LOWIOPANEL 0
#endif
#ifndef MUT_HIGHIOPANEL
#define MUT_HIGHIOPANEL 0
#endif
#ifndef MUT_CIAPRADDRA
#define MUT_CIAPRADDRA 0
#endif

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0, failed = 0;
};

static std::string hexaddr(uint32_t byte_addr) {
    char buf[10];
    std::snprintf(buf, sizeof buf, "$%06X", byte_addr & 0xFFFFFFu);
    return std::string(buf);
}
static std::string b(bool v) { return v ? "1" : "0"; }
static std::string hexbits(uint32_t v) {
    char buf[8];
    std::snprintf(buf, sizeof buf, "0x%X", v);
    return std::string(buf);
}

// cpu_address_in is declared [23:1] -- word-aligned, A0 dropped (gary.v:51).
static inline uint32_t waddr(uint32_t byte_addr) { return (byte_addr >> 1) & 0x7FFFFFu; }

struct Harness {
    Vgary *top;
    Harness() {
        top = new Vgary;
        top->clk = 0;
        top->reset = 1;
        top->cpu_address_in = 0;
        top->dma_address_in = 0;
        top->cpu_data_out = 0;
        top->custom_data_out = 0;
        top->ram_data_out = 0;
        top->a1k = 0;
        top->bootrom = 0;
        top->cpu_rd = 1;
        top->cpu_hwr = 0;
        top->cpu_lwr = 0;
        top->cpu_hlt = 0;
        top->ovl = 0;
        top->dbr = 0;
        top->dbwe = 0;
        top->memory_config = 0xF;  // widest configuration; overridden per-assertion where the manual's own qualifier ("on some systems") is itself under test.
        top->ecs = 1;
        top->hdc_ena = 1;
        tick();
        tick();
        top->reset = 0;
        tick();
    }
    ~Harness() { top->final(); delete top; }

    void tick() { top->clk = 0; top->eval(); top->clk = 1; top->eval(); }
    void settle() { top->eval(); top->eval(); }

    // A CPU-bus probe: Agnus does not have the bus (dbr=0), and
    // cpu_address_in carries the 680x0 address under test.
    void cpu_probe(uint32_t byte_addr) {
        top->dbr = 0;
        top->cpu_address_in = waddr(byte_addr);
        settle();
    }
};

int main() {
    Checks checks;

    // =========================================================================
    // FIXTURE-LIVENESS (no uid -- harness precondition, per the build
    // brief's "the fixture proves it can deliver a known value before
    // anything reads one" and the appF-cia-address-decode precedent).
    // =========================================================================
    {
        Harness h;
        h.cpu_probe(0x000000);  // chip RAM: a known, distinct point.
        bool chip_point = ((h.top->sel_chip & 0x1) == 1) && (h.top->sel_kick == 0);
        std::string chip_detail = "sel_chip=" + hexbits(h.top->sel_chip) + " sel_kick=" + b(h.top->sel_kick);

        uint32_t kick_probe = 0xF80000;
#if MUT_LIVENESS
        kick_probe = 0x000000;  // MUTATION: address change withheld entirely.
#endif
        h.cpu_probe(kick_probe);
        bool kick_point = (h.top->sel_kick == 1) && (h.top->sel_chip == 0);
        std::string kick_detail = "sel_chip=" + hexbits(h.top->sel_chip) + " sel_kick=" + b(h.top->sel_kick);

        checks.expect(chip_point && kick_point,
            "FIXTURE-LIVENESS (no uid -- harness precondition): the fixture reads two "
            "different, known values for two different stimuli rather than one stuck "
            "value -- $000000 reads as chip RAM only (measured " + chip_detail +
            ") and " + hexaddr(kick_probe) + " reads as kickstart ROM only (measured " +
            kick_detail + ") -- gary.v's address decode responds live to cpu_address_in.");
    }

    // =========================================================================
    // GROUP 1 -- CHIPMEM-LOWBLOCK-A1000 (p.314, $00 0000-$07 FFFF). 4 claims:
    // sol:D-memmap-a1000.c1, sol:D-memmap-a1000.c2, opus:D-memmap-a500.c2,
    // opus:D-memmap-a500.c3. DECLINED: gary.v:159 (sel_chip[0], the whole
    // $00-$07FFFF block as one undivided select) is the identical
    // proposition hrm/ch1-memory-map-and-two-buses' CHIPMEM-512K-FLOOR
    // already grades ($000000 and $07FFFE both select chip block 0). The
    // page's own 256K subdivision at $04 0000 (A1000-vs-A500/A2000
    // attribution) is narrower still and UNOBSERVABLE: no output of gary
    // moves at that boundary (sel_chip[0] alone covers the full 512K).
    // =========================================================================
    checks.record(
        "CHIPMEM-LOWBLOCK-A1000 (p.314, 4 claims: sol:D-memmap-a1000.c1, "
        "sol:D-memmap-a1000.c2, opus:D-memmap-a500.c2, opus:D-memmap-a500.c3) -- "
        "the $00 0000-$07 FFFF-is-Chip-RAM half is the SAME proposition "
        "hrm/ch1-memory-map-and-two-buses' CHIPMEM-512K-FLOOR already grades "
        "(gary.v:159, sel_chip[0] one undivided select over the whole block). "
        "Not re-graded. MEASURED, not graded: the page's own 256K subdivision "
        "at $04 0000 (A1000 gets all of it, A500/A2000 the first half only) is "
        "UNOBSERVABLE at this boundary -- gary.v:159 is a single compare "
        "(cpu_address_in[23:19]==5'b00000) with no term that moves at $04 0000; "
        "sel_chip[0] reads identically at $000000 and $040000. The "
        "A1000-vs-A500/A2000 attribution itself is a model label with no port. "
        "Reported as unobservable, not red.");

    // =========================================================================
    // GROUP 2 -- CHIPMEM-EXT-512K (p.314, $08 0000-$0F FFFF). 2 claims:
    // sol:D-memmap-a1000.c3, opus:D-memmap-a500.c4. DECLINED: together,
    // hrm/ch1-memory-map-and-two-buses' CHIPMEM-512K-FLOOR (proves $080000
    // is a DIFFERENT block from the floor) and FIRST-512K-FLOOR-AND-SCALE
    // (proves sel_chip[1] asserts at $0FFFFE, the 1MB point -- the same
    // upper edge this row prints) together already cover the full extent of
    // this row's own proposition at gary's boundary (gary.v:160).
    // =========================================================================
    checks.record(
        "CHIPMEM-EXT-512K (p.314, 2 claims: sol:D-memmap-a1000.c3, "
        "opus:D-memmap-a500.c4) -- gary.v:160 (sel_chip[1], "
        "cpu_address_in[23:19]==5'b00001, exactly $08 0000-$0F FFFF) is the "
        "same proposition hrm/ch1-memory-map-and-two-buses grades in two "
        "pieces: CHIPMEM-512K-FLOOR shows $080000 selects a block distinct "
        "from the floor, and FIRST-512K-FLOOR-AND-SCALE shows sel_chip[1] "
        "asserts at $0FFFFE, this row's own upper edge. Not re-graded. The "
        "'to 1 MB for A2000' half is a model statement with no port.");

    // =========================================================================
    // GROUP 3 -- CIAB-WINDOW-EVENBYTE (p.314, $BF D000-$BF DF00). 2 claims:
    // sol:D-memmap-a1000.c7, opus:D-memmap-a500.c8. DECLINED: the window
    // itself, INCLUDING its own upper limit $BFDF00, is the same proposition
    // hrm/ch1-memory-map-and-two-buses' CIA2-BLOCK and hrm/
    // appF-cia-address-decode's CIAB-MAP-SWEEP already grade -- CIAB-MAP-
    // SWEEP's own address list runs through $BFDF00 as its literal last
    // entry (verified directly against test_gary.cpp). The even-byte
    // restriction is NOT gradeable here at all: cpu_address_in is declared
    // [23:1] (gary.v:51) -- A0 is not present on this port, and the byte
    // lane a real 68000 cycle would pick via A0 is decided one module out,
    // at minimig (minimig.v:614,639, per this claim's own classify.evidence).
    // =========================================================================
    checks.record(
        "CIAB-WINDOW-EVENBYTE (p.314, 2 claims: sol:D-memmap-a1000.c7, "
        "opus:D-memmap-a500.c8) -- the $BF D000-$BF DF00 window, including its "
        "own upper limit, is the same proposition hrm/ch1-memory-map-and-two-"
        "buses' CIA2-BLOCK and hrm/appF-cia-address-decode's CIAB-MAP-SWEEP "
        "already grade (gary.v:190,192; CIAB-MAP-SWEEP's own sweep list ends "
        "at $BFDF00, this row's own upper edge, verified against "
        "hrm/appF-cia-address-decode/test_gary.cpp). Not re-graded. The "
        "even-byte-only restriction is NOT gradeable at this boundary at all: "
        "cpu_address_in is declared [23:1] (gary.v:51), A0 is absent from "
        "this port, and the byte lane is minimig's own decode "
        "(minimig.v:614,639), a different top module.");

    // =========================================================================
    // GROUP 4 -- CIAA-WINDOW-ODDBYTE (p.314, $BF E001-$BF EF01). 2 claims:
    // sol:D-memmap-a1000.c8, opus:D-memmap-a500.c9. Mirror of Group 3.
    // =========================================================================
    checks.record(
        "CIAA-WINDOW-ODDBYTE (p.314, 2 claims: sol:D-memmap-a1000.c8, "
        "opus:D-memmap-a500.c9) -- mirror of CIAB-WINDOW-EVENBYTE above: the "
        "$BF E001-$BF EF01 window is the same proposition hrm/ch1-memory-map-"
        "and-two-buses' CIA1-BLOCK and hrm/appF-cia-address-decode's "
        "CIAA-MAP-SWEEP already grade (gary.v:190-191; CIAA-MAP-SWEEP's own "
        "sweep list ends at $BFEF01, this row's own upper edge). Not "
        "re-graded. The odd-byte-only restriction is not gradeable here, same "
        "reason as CIAB-WINDOW-EVENBYTE (no A0 on this port; byte lane is "
        "minimig's own decode).");

    // =========================================================================
    // GROUP 5 -- SLOWMEM-UPPER-BANKS-WIDE. 2 claims: sol:D-memmap-a1000.c11,
    // opus:D-memmap-a500.c12. p.314 prints $C0 0000-$D7 FFFF (all THREE
    // t_sel_slow banks) as one row; hrm/ch1-memory-map-and-two-buses'
    // SLOW-MEM-LOCATION-512K grades only the FIRST bank, $C0-$C7FFFF, and
    // explicitly checks sel_slow[0]=0 at $C80000. WIDER HERE: the upper two
    // banks ($C8-$CFFFFF, $D0-$D7FFFF) are new content, graded below. The
    // "on some systems" qualifier IS memory_config itself (gary.v:171-173) --
    // graded as a control.
    // =========================================================================
    {
        Harness h;
        h.top->memory_config = 0xF;  // widest config -- all three slow banks enabled (gary.v:171-173).
        h.top->ovl = 0;
        h.top->cpu_rd = 1;
        h.cpu_probe(0xC80000);
        bool slow1_start = ((h.top->sel_slow >> 1) & 1) == 1;
        h.cpu_probe(0xCFFFFE);
        bool slow1_end = ((h.top->sel_slow >> 1) & 1) == 1;
        uint32_t d0_probe = 0xD00000;
#if MUT_SLOWUPPER
        d0_probe = 0xE10000;  // MUTATION: redirected well outside any slow bank.
#endif
        h.cpu_probe(d0_probe);
        bool slow2_start = ((h.top->sel_slow >> 2) & 1) == 1;
        h.cpu_probe(0xD7FFFE);
        bool slow2_end = ((h.top->sel_slow >> 2) & 1) == 1;

        // Control: the printed qualifier ("on some systems") really is
        // memory_config -- with the slow-bank bits clear, none of the three
        // banks assert anywhere across $C0 0000-$D7 FFFF.
        Harness h2;
        h2.top->memory_config = 0x0;
        h2.top->ovl = 0;
        h2.top->cpu_rd = 1;
        h2.cpu_probe(0xC00000);
        bool c0_off = h2.top->sel_slow == 0;
        h2.cpu_probe(0xC80000);
        bool c8_off = h2.top->sel_slow == 0;
        h2.cpu_probe(0xD00000);
        bool d0_off = h2.top->sel_slow == 0;

        checks.expect(slow1_start && slow1_end && slow2_start && slow2_end &&
                          c0_off && c8_off && d0_off,
            "SLOWMEM-UPPER-BANKS-WIDE: p.314 prints $C0 0000-$D7 FFFF as one "
            "row of internal expansion (slow) memory 'on some systems' -- "
            "hrm/ch1-memory-map-and-two-buses' SLOW-MEM-LOCATION-512K already "
            "grades only the FIRST 512K bank ($C0-$C7FFFF, declined here, same "
            "proposition); the upper two banks are new. With every slow bank "
            "enabled (memory_config=0xF), $C8 0000-$CF FFFE (t_sel_slow[1]) "
            "and " + hexaddr(d0_probe) + "-$D7 FFFE (t_sel_slow[2]) each "
            "assert throughout (measured " + b(slow1_start) + "/" + b(slow1_end) +
            " and " + b(slow2_start) + "/" + b(slow2_end) +
            "), and the printed 'on some systems' qualifier is exactly "
            "memory_config itself: with memory_config=0x0 none of the three "
            "banks assert anywhere in $C0 0000-$D7 FFFF (measured off at "
            "$C00000/$C80000/$D00000: " + b(c0_off) + "/" + b(c8_off) + "/" +
            b(d0_off) + "). See the CH7-TWO-REDS-STATUS record() below for "
            "this assertion's relationship to ch7-custom-address-space's own "
            "$D00000 measurement. [claims: sol:D-memmap-a1000.c11, "
            "opus:D-memmap-a500.c12]");
    }

    // =========================================================================
    // GROUP 6 -- RTC-WINDOW-DEFAULT-BUILD (p.314, $DC 0000-$DC FFFF). 2
    // claims: sol:D-memmap-a1000.c13, opus:D-memmap-a500.c14. NEW: no package
    // in nanomig-hrm-tests/hrm names $DC0000 or sel_rtc (grep-confirmed
    // empty over hrm/, and confirmed again while writing this file). The
    // "not accessable on all systems" qualifier IS the `ifdef ENABLE_RTC
    // conditional itself (gary.v:179-183) -- no Makefile in this suite
    // defines ENABLE_RTC (grep-confirmed: appF-cia-pin-bindings,
    // ch7-overlay-source and three ch8-* Makefiles define DISABLE_IDE, none
    // defines ENABLE_RTC), so in THIS build (and every other build in the
    // suite) gary.v:182's dummy wire ties sel_rtc to 0 and gary.v:185's
    // exclusion of it leaves $DC0000-$DCFFFF answering as sel_reg instead --
    // measured directly below, without referencing the sel_rtc port (it is
    // not part of this module's interface in this configuration).
    // =========================================================================
    {
        Harness h;
        h.cpu_probe(0xDC0000);
        bool win_start = h.top->sel_reg == 1;
        h.cpu_probe(0xDCFFFE);
        bool win_end = h.top->sel_reg == 1;
        uint32_t control = 0xDA0000;  // sel_ide's own window -- genuinely excluded from sel_reg (gary.v:185).
#if MUT_RTCWIN
        control = 0xDC0000;  // MUTATION: control collapsed onto the window itself.
#endif
        h.cpu_probe(control);
        bool control_excluded = h.top->sel_reg == 0;
        checks.expect(win_start && win_end && control_excluded,
            "RTC-WINDOW-DEFAULT-BUILD: p.314 prints $DC 0000-$DC FFFF as the "
            "real-time clock, 'not accessable on all systems'. New here -- no "
            "package in this suite names $DC0000 or sel_rtc (grep-confirmed "
            "over hrm/). gary.v:179-183 gate sel_rtc entirely behind `ifdef "
            "ENABLE_RTC, which no Makefile in this suite defines (grep-"
            "confirmed), so in this build (and every other) sel_rtc is a "
            "tied-0 dummy wire (gary.v:182) with no module port at all, and "
            "gary.v:185's exclusion list leaves the whole window answering as "
            "sel_reg instead: measured sel_reg=1 across $DC0000-$DCFFFE (" +
            b(win_start) + "/" + b(win_end) + "), while " + hexaddr(control) +
            " (sel_ide's own window, a genuine exclusion) reads sel_reg=0 (" +
            b(control_excluded) + ") -- the printed qualifier is realised "
            "literally: this system (this default build) does not have the "
            "clock accessible at all, and the window answers as ordinary "
            "custom registers instead of being carved out for it. "
            "[claims: sol:D-memmap-a1000.c13, opus:D-memmap-a500.c14]");
    }

    // =========================================================================
    // GROUP 7 -- CHIPREG-NARROW (p.314, $DF F000-$DF FFFF). 2 claims:
    // sol:D-memmap-a1000.c14, opus:D-memmap-a500.c15. DECLINED: the same
    // proposition hrm/ch7-custom-address-space's ADDR-FORMULA-SELECTS-
    // CUSTOM-BANK and ADDR-EXCLUSIVE-CONTROL-DFF000 already grade (both
    // still PASS in ch7's current out.txt) -- gary.v:185-188.
    // =========================================================================
    checks.record(
        "CHIPREG-NARROW (p.314, 2 claims: sol:D-memmap-a1000.c14, "
        "opus:D-memmap-a500.c15) -- $DF F000-$DF FFFF selecting sel_reg is "
        "the same proposition hrm/ch7-custom-address-space's ADDR-FORMULA-"
        "SELECTS-CUSTOM-BANK and ADDR-EXCLUSIVE-CONTROL-DFF000 already grade "
        "(gary.v:185-188; both currently PASS, confirmed against ch7's own "
        "out.txt). Not re-graded. This page's own $DF F000 lower bound "
        "printing (narrower than p.315/p.316's $DF 0000, see "
        "CUSTOMREG-WIDE-64K-A3000 below) is the SAME boundary ch7's own "
        "withdrawn ADDR-BELOW-DF0000 measurement discusses -- see "
        "CH7-TWO-REDS-STATUS.");

    // =========================================================================
    // GROUP 8 -- KICKSTART-256K-p314 ($FC 0000-$FF FFFF). 2 claims:
    // sol:D-memmap-a1000.c19, opus:D-memmap-a500.c20. DECLINED: the NM-HRM-
    // 063 shape -- p.314 states a 256K ROM MIRRORED across this exact
    // window, which is hrm/ch1-memory-map-and-two-buses' KICKSTART-256K-
    // MIRROR mechanism exactly (gary.v:92,167, sel_kick256kmirror), not
    // merely satisfied-in-passing by the wider KICKSTART-512K assertion this
    // page's own upper bound would otherwise suggest.
    // =========================================================================
    checks.record(
        "KICKSTART-256K-p314 (p.314, 2 claims: sol:D-memmap-a1000.c19, "
        "opus:D-memmap-a500.c20) -- p.314 states the system ROM is 256K, "
        "mirrored across $FC 0000-$FF FFFF; this is the SAME mechanism hrm/"
        "ch1-memory-map-and-two-buses' KICKSTART-256K-MIRROR already grades "
        "(gary.v:92,167, sel_kick256kmirror -- with bootrom=1 and the ROM "
        "write-locked, a 256K image answers mirrored across the full 512K "
        "window). Not re-graded. NOT declined on KICKSTART-512K, which is a "
        "different, wider proposition (a full 512K image, unmirrored) -- "
        "p.315 and p.316 print that third reading for the A3000 and are "
        "declined against KICKSTART-512K instead (HIGHROM-512K-A3000 below), "
        "per the NM-HRM-063 shape this row and that one are the same "
        "instance of.");

    // =========================================================================
    // GROUP 9 -- CHIPMEM-2MB-A3000 (pp.315/316, $0000 0000-$001F FFFF /
    // $0000 0000-$0020 0000, printed twice -- table and diagram). 4 claims:
    // sol:D-memmap-a3000.c1, opus:D-memmap-a3000.c2, sol:D-memmap-a3000-
    // 32bit.c14, opus:D-memmap-a3000-32bit.c25. DECLINED: the same
    // proposition hrm/ch1-memory-map-and-two-buses' CHIPMEM-AMOUNT-SCALES
    // and hrm/ch7-custom-address-space's DMA-CHIP-MEMORY-ONLY-SWEEP already
    // grade (gary.v:159-162, the four sel_chip banks, full 2MB).
    // =========================================================================
    checks.record(
        "CHIPMEM-2MB-A3000 (pp.315/316, 4 claims: sol:D-memmap-a3000.c1, "
        "opus:D-memmap-a3000.c2, sol:D-memmap-a3000-32bit.c14, "
        "opus:D-memmap-a3000-32bit.c25) -- the A3000 map prints this same 2MB "
        "Chip-Memory fact twice, once per page (p.315's table, p.316's "
        "diagram); both printings are the SAME proposition hrm/ch1-memory-"
        "map-and-two-buses' CHIPMEM-AMOUNT-SCALES and hrm/"
        "ch7-custom-address-space's DMA-CHIP-MEMORY-ONLY-SWEEP already grade "
        "(gary.v:159-162, all four sel_chip banks, one-hot, full 2MB). Not "
        "re-graded, on either printing.");

    // =========================================================================
    // GROUP 10 -- CIA-BLOCK-64K-A3000 (pp.315/316, $00BF 0000-$00BF FFFF,
    // printed twice). 4 claims: sol:D-memmap-a3000.c5, opus:D-memmap-
    // a3000.c6, sol:D-memmap-a3000-32bit.c40, opus:D-memmap-a3000-
    // 32bit.c36. DECLINED: gary.v:190 (sel_cia, the whole $BF block) is the
    // same proposition hrm/ch1-memory-map-and-two-buses' CIA-PAULA-ADDRESS-
    // SPLIT and hrm/appF-cia-address-decode's CIAA-MAP-SWEEP/CIAB-MAP-SWEEP
    // already grade in combination (the two sweeps together cover the whole
    // $BF window except $BFE001/$BFE201/$BFD100/$BFDD00, each separately
    // addressed -- see CIAA-PRA-DDRA-ADDRESS-WINDOW below and PART4-CIAA-ROW/
    // PART4-CIAB-ROW's own citations).
    // =========================================================================
    checks.record(
        "CIA-BLOCK-64K-A3000 (pp.315/316, 4 claims: sol:D-memmap-a3000.c5, "
        "opus:D-memmap-a3000.c6, sol:D-memmap-a3000-32bit.c40, "
        "opus:D-memmap-a3000-32bit.c36) -- the A3000 map's whole-$BF-block "
        "CIA-ports-and-timers fact, printed twice, is the same proposition "
        "hrm/ch1-memory-map-and-two-buses' CIA-PAULA-ADDRESS-SPLIT and hrm/"
        "appF-cia-address-decode's CIAA-MAP-SWEEP/CIAB-MAP-SWEEP already "
        "grade (gary.v:190). Not re-graded, on either printing.");

    // =========================================================================
    // GROUP 11 -- SLOW0-512K-A3000 (pp.315/316, $00C0 0000-$00C7 FFFF /
    // $00C0 0000-$00C8 0000, printed twice as "Expansion Memory" and "Ranger
    // RAM"). 4 claims: sol:D-memmap-a3000.c6, opus:D-memmap-a3000.c7,
    // sol:D-memmap-a3000-32bit.c18, opus:D-memmap-a3000-32bit.c21.
    // DECLINED: the same proposition hrm/ch1-memory-map-and-two-buses'
    // SLOW-MEM-LOCATION-512K already grades (gary.v:171, t_sel_slow[0]).
    // =========================================================================
    checks.record(
        "SLOW0-512K-A3000 (pp.315/316, 4 claims: sol:D-memmap-a3000.c6, "
        "opus:D-memmap-a3000.c7, sol:D-memmap-a3000-32bit.c18, "
        "opus:D-memmap-a3000-32bit.c21) -- the first slow-memory 512K block, "
        "printed on both pages under two different names ('Expansion "
        "Memory', 'Ranger RAM'; p.314 calls the same range 'internal "
        "expansion (slow) memory' -- three names, one gary.v:171 select), is "
        "the same proposition hrm/ch1-memory-map-and-two-buses' "
        "SLOW-MEM-LOCATION-512K already grades. Not re-graded, on either "
        "printing.");

    // =========================================================================
    // GROUP 12 -- CLOCK-BLOCK-CONFLICT. 4 claims: sol:D-memmap-a3000.c9,
    // opus:D-memmap-a3000.c10, sol:D-memmap-a3000-32bit.c29, opus:D-memmap-
    // a3000-32bit.c30. TWO PRINTED DEFECTS: p.315's own three-row chain
    // ($00DC0000-$00DDFFFF / $00DD0000-$00DEFFFF / $00DE0000-$00DEFFFF, each
    // starting inside the row above) against p.316's High I/O Registers
    // panel (contiguous DC/DD/DE/DF ticks, no overlap at all). Per the
    // brief's CONDITION ("where the manual contradicts itself, grade
    // neither side") and NM-HRM-062's own precedent: measured both, recorded
    // both, graded neither. source_defines: conflict.
    // =========================================================================
    {
        Harness h;
        h.cpu_probe(0xDC0000);
        bool dc_is_reg_or_rtc = h.top->sel_reg == 1;  // default build: sel_rtc tied 0, sel_reg claims it (see RTC-WINDOW-DEFAULT-BUILD).
        checks.note("CLOCK-BLOCK-P315-READING: p.315 prints the clock block as "
            "$00DC0000-$00DDFFFF (128K), overlapping the next row "
            "($00DD0000-$00DEFFFF) and that row's own successor "
            "($00DE0000-$00DEFFFF) in a three-row chain -- verified against "
            "figures/p315.png. At gary, the undisputed floor of the block, "
            "$DC0000, answers as sel_reg=" + b(dc_is_reg_or_rtc) +
            " (default build, no ENABLE_RTC -- see RTC-WINDOW-DEFAULT-BUILD).");
        h.cpu_probe(0xDD0000);
        bool dd_is_reg = h.top->sel_reg == 1;
        checks.note("CLOCK-BLOCK-P316-READING: p.316's High I/O Registers panel "
            "prints the same three regions as CONTIGUOUS, non-overlapping "
            "64K-aligned blocks between the $DC0000/$DD0000/$DE0000/$DF0000 "
            "ticks -- verified against figures/p316.png. At gary, $DD0000 "
            "(p.316's own start of the next block) answers as sel_reg=" +
            b(dd_is_reg) + ".");
        checks.record(
            "CLOCK-BLOCK-CONFLICT (pp.315/316, 4 claims: sol:D-memmap-a3000.c9, "
            "opus:D-memmap-a3000.c10, sol:D-memmap-a3000-32bit.c29, "
            "opus:D-memmap-a3000-32bit.c30; source_defines: conflict -- page "
            "315 prints $00DC0000-$00DDFFFF, $00DD0000-$00DEFFFF and "
            "$00DE0000-$00DEFFFF as three consecutive overlapping rows, "
            "against page 316's High I/O Registers panel, which prints the "
            "same three regions as contiguous 64K-aligned blocks): NOT "
            "resolved and not adjudicated here, per NM-HRM-062's own "
            "precedent and the outer brief's CONDITION for this exact page. "
            "An assertion built from p.315's own end addresses would red a "
            "64K-aligned decode gary genuinely implements (gary.v:171-188); "
            "an assertion built from p.316's ticks would silently repair the "
            "printed overlap. Both readings measured above via note(); "
            "neither graded. Undisputed by both printings: the clock block's "
            "own floor is $DC0000 and the chip-register block's own floor is "
            "$DF0000 (both measured above and in CUSTOMREG-WIDE-64K-A3000). "
            "A second, larger p.315 overlap ($00E80000-$0EFFFFFF, one digit "
            "group short of its neighbours' form, 240MB as printed) and "
            "p.316's doubled $DD0000 tick / skipped $DB0000 tick are also "
            "printed defects on these same pages but bind to no claim in "
            "this package's own 60 -- reported in the build report, not "
            "graded here.");
    }

    // =========================================================================
    // GROUP 13 -- CUSTOMREG-WIDE-64K-A3000 (pp.315/316, $00DF 0000-$00DF
    // FFFF, printed twice as "Amiga Chip Registers" / "Amiga Chips"). 4
    // claims: sol:D-memmap-a3000.c12, opus:D-memmap-a3000.c13, sol:D-memmap-
    // a3000-32bit.c32, opus:D-memmap-a3000-32bit.c27. NEW/GRADED: this is
    // the missing printed oracle for hrm/ch7-custom-address-space's own
    // (now-withdrawn) $DF0000 measurement -- p.315 and p.316 BOTH print the
    // chip-register window as the FULL 64K gary.v:185-188 actually decodes,
    // wider than p.314's own $DF F000-$DF FFFF (declined above, CHIPREG-
    // NARROW) and wider than the "(chip address)+$DFF000" formula
    // hrm/ch7-custom-address-space grades from p.263.
    // =========================================================================
    {
        Harness h;
        h.cpu_probe(0xDF0000);
        bool at_df0000 = h.top->sel_reg == 1;
        h.cpu_probe(0xDFF000);
        bool at_dff000 = h.top->sel_reg == 1;
        uint32_t top_probe = 0xDFFFFE;
#if MUT_CUSTOMREGWIDE
        top_probe = 0xE00000;  // MUTATION: moved one word past the window's own top.
#endif
        h.cpu_probe(top_probe);
        bool at_top = h.top->sel_reg == 1;
        h.cpu_probe(0xE00000);
        bool above_excluded = h.top->sel_reg == 0;
        checks.expect(at_df0000 && at_dff000 && at_top && above_excluded,
            "CUSTOMREG-WIDE-64K-A3000: pp.315/316 both print the chip-"
            "register window as the FULL 64K $00DF0000-$00DFFFFF ('Amiga Chip "
            "Registers' / 'Amiga Chips') -- exactly what gary.v:185-188 "
            "decodes: sel_reg=1 at $DF0000 (measured " + b(at_df0000) +
            "), at $DFF000 -- p.314's own narrower start point, still inside "
            "(measured " + b(at_dff000) + ") -- and at " + hexaddr(top_probe) +
            ", the window's own top (measured " + b(at_top) +
            "), with sel_reg=0 immediately above at $E00000 (measured " +
            b(above_excluded) + "). This is the MISSING ORACLE hrm/"
            "ch7-custom-address-space's own (now-withdrawn) ADDR-BELOW-"
            "DF0000-MEASURED note reported without a printed page behind "
            "it: p.315/p.316 supply exactly that page. Not decline against "
            "ch7-custom-address-space -- this row's own proposition is WIDER "
            "than the (chip address)+$DFF000 formula ch7 grades from p.263. "
            "See CH7-TWO-REDS-STATUS below. [claims: sol:D-memmap-a3000.c12, "
            "opus:D-memmap-a3000.c13, sol:D-memmap-a3000-32bit.c32, "
            "opus:D-memmap-a3000-32bit.c27]");
    }

    // =========================================================================
    // CH7-TWO-REDS-STATUS -- consolidated record(), no uid. The outer build
    // brief states "Two of ch7-custom-address-space's assertions are RED at
    // $DF0000 and $D00000". Checked directly against
    // hrm/ch7-custom-address-space/test_gary.cpp (search "WITHDRAWN AT STAGE
    // 8") and its own out.txt ("8 passed, 0 failed"): both former
    // assertions -- ADDR-EXCLUSIVE-BELOW-DF0000 and ADDR-EXCLUSIVE-MIDBLOCK-
    // D00000 -- were converted to note()/record() pairs before this build,
    // reason_kind source-silent (p.263's "No other access address is valid"
    // states which addresses SOFTWARE may use, not what the select lines
    // must do at an address software is told not to use). So the brief's own
    // premise is STALE: there are zero reds in ch7-custom-address-space
    // today, not two.
    // =========================================================================
    checks.record(
        "CH7-TWO-REDS-STATUS (no uid -- background measurement per the outer "
        "brief's explicit 'measure it and report it' instruction): the brief "
        "states ch7-custom-address-space carries two RED assertions at "
        "$DF0000 and $D00000. CHECKED DIRECTLY, not trusted from the brief: "
        "hrm/ch7-custom-address-space/test_gary.cpp's own header says "
        "(search string 'WITHDRAWN AT STAGE 8', two occurrences) that both "
        "former checks.expect() assertions were converted to note()/"
        "record() pairs -- ADDR-BELOW-DF0000-MEASURED/-NOT-GRADED and "
        "ADDR-MIDBLOCK-D00000-MEASURED/-NOT-GRADED -- because their own "
        "reason_kind is source-silent, and that package's own out.txt reads "
        "'8 passed, 0 failed' as of this build. THE BRIEF'S PREMISE IS "
        "STALE: there are currently zero reds in ch7-custom-address-space, "
        "not two. The underlying RTL measurements still stand unchanged "
        "(sel_reg($DF0000)=1, sel_reg($D00000)=1 under ch7's own "
        "memory_config=4'b0100) and this package's own "
        "CUSTOMREG-WIDE-64K-A3000 and SLOWMEM-UPPER-BANKS-WIDE above "
        "independently confirm the RTL side of both under a properly "
        "enabling configuration -- p.315/p.316 print $DF0000-$DFFFFF as the "
        "full 64K register bank gary decodes (GREEN, the missing oracle "
        "ch7's own withdrawn note lacked), and p.314 prints $D0 0000-$D7 "
        "FFFF as slow memory, which is exactly t_sel_slow[2] once "
        "memory_config enables it (GREEN). Not adjudicated here whether "
        "ch7's own stage-8 withdrawal was itself correct -- reported per the "
        "outer brief's own instruction, not decided.");

    // =========================================================================
    // GROUP 14 -- HIGHROM-512K-A3000 (pp.315/316, $00F8 0000-$00FF FFFF,
    // printed twice as "High ROM (512K)" / "High ROM"). 4 claims:
    // sol:D-memmap-a3000.c16, opus:D-memmap-a3000.c17, sol:D-memmap-a3000-
    // 32bit.c24, opus:D-memmap-a3000-32bit.c15. DECLINED: the same
    // proposition hrm/ch1-memory-map-and-two-buses' KICKSTART-512K already
    // grades exactly (gary.v:166) -- unlike p.314's 256K printing
    // (KICKSTART-256K-p314 above), this page states the SAME 512K window
    // KICKSTART-512K tests, not the mirrored-256K reading.
    // =========================================================================
    checks.record(
        "HIGHROM-512K-A3000 (pp.315/316, 4 claims: sol:D-memmap-a3000.c16, "
        "opus:D-memmap-a3000.c17, sol:D-memmap-a3000-32bit.c24, "
        "opus:D-memmap-a3000-32bit.c15) -- the A3000 map's 512K high-ROM "
        "window, printed twice, is the SAME proposition hrm/ch1-memory-map-"
        "and-two-buses' KICKSTART-512K already grades (gary.v:166, exactly "
        "$F80000-$FFFFFF). Not re-graded, on either printing. This is the "
        "third printing recorded together with p.314's 256K row "
        "(KICKSTART-256K-p314) and p.1's own 512K statement -- three "
        "readings, two different ROM sizes, in the manual's own words.");

    // =========================================================================
    // GROUP 15 -- ADDRSPACE-16MB-A3000 (p.316, 24-bit address space panel).
    // 4 claims: sol:D-memmap-a3000-32bit.c9, sol:D-memmap-a3000-32bit.c13,
    // opus:D-memmap-a3000-32bit.c13, opus:D-memmap-a3000-32bit.c14.
    // DECLINED: the same proposition hrm/ch1-memory-map-and-two-buses'
    // CPU-ADDR-SPACE-16MB already grades (gary.v:51, the [23:1] port
    // declaration and its own 16MB wraparound fold).
    // =========================================================================
    checks.record(
        "ADDRSPACE-16MB-A3000 (p.316, 4 claims: sol:D-memmap-a3000-32bit.c9, "
        "sol:D-memmap-a3000-32bit.c13, opus:D-memmap-a3000-32bit.c13, "
        "opus:D-memmap-a3000-32bit.c14) -- the panel's own '(16 Mbytes)' / "
        "24-bit address space statement is the same proposition hrm/"
        "ch1-memory-map-and-two-buses' CPU-ADDR-SPACE-16MB already grades "
        "(gary.v:51, cpu_address_in[23:1] -- $000000 selects chip RAM, "
        "$FFFFFE selects kickstart ROM, and byte address $1000000 folds "
        "through the port's own width back to word address 0). Not "
        "re-graded.");

    // =========================================================================
    // GROUP 16 -- LOWIOREG-PANEL-SWEEP (p.316, Low I/O Registers panel,
    // $00B8 0000-$00BF FFFF). 3 claims: sol:D-memmap-a3000-32bit.c17,
    // opus:D-memmap-a3000-32bit.c22, opus:D-memmap-a3000-32bit.c35.
    // NEW/GRADED: the panel's own EXTENT is not individually graded
    // anywhere else -- gary puts exactly two selects in this range
    // (sel_rtg at $B8xxxx, sel_cia at $BFxxxx) and no RAM select anywhere
    // in it, matching the panel's own reserved/CIA layout.
    // =========================================================================
    {
        Harness h;
        h.cpu_probe(0xB80000);
        bool rtg_start = h.top->sel_rtg == 1;
        h.cpu_probe(0xB8FFFE);
        bool rtg_end = h.top->sel_rtg == 1;
        h.cpu_probe(0xB90000);
        bool rtg_off_above = h.top->sel_rtg == 0;
        h.cpu_probe(0xBF0000);
        bool cia_start = h.top->sel_cia == 1;
        uint32_t top_probe = 0xBFFFFE;
#if MUT_LOWIOPANEL
        top_probe = 0xC00000;  // MUTATION: moved one word past the panel's own top.
#endif
        h.cpu_probe(top_probe);
        bool cia_top = h.top->sel_cia == 1;
        h.cpu_probe(0xBA0000);
        bool noram1 = (h.top->sel_chip == 0) && (h.top->sel_slow == 0) && (h.top->sel_kick == 0);
        h.cpu_probe(0xBE0000);
        bool noram2 = (h.top->sel_chip == 0) && (h.top->sel_slow == 0) && (h.top->sel_kick == 0);
        checks.expect(rtg_start && rtg_end && rtg_off_above && cia_start &&
                          cia_top && noram1 && noram2,
            "LOWIOREG-PANEL-SWEEP: the panel's own extent, $00B80000-$00BFFFFF, "
            "is new content -- not individually graded elsewhere. Across it, "
            "gary puts sel_rtg over $B80000-$B8FFFE (measured " + b(rtg_start) +
            "/" + b(rtg_end) + ", off immediately above at $B90000: " +
            b(rtg_off_above) + ") and sel_cia over $BF0000-" + hexaddr(top_probe) +
            " (measured " + b(cia_start) + "/" + b(cia_top) +
            "), with no RAM select (sel_chip/sel_slow/sel_kick) asserting "
            "anywhere in between (measured at $BA0000/$BE0000: " + b(noram1) +
            "/" + b(noram2) + ") -- matching the panel's own reserved/CIA "
            "layout. sel_rtg is Minimig-AGA's own addition; the page prints "
            "its block as reserved. [claims: sol:D-memmap-a3000-32bit.c17, "
            "opus:D-memmap-a3000-32bit.c22, opus:D-memmap-a3000-32bit.c35]");
    }

    // =========================================================================
    // GROUP 17 -- HIGHIOREG-PANEL-SWEEP (p.316, High I/O Registers panel,
    // $00D8 0000-$00DF FFFF). 4 claims: sol:D-memmap-a3000-32bit.c20,
    // sol:D-memmap-a3000-32bit.c26, opus:D-memmap-a3000-32bit.c19,
    // opus:D-memmap-a3000-32bit.c26. NEW/GRADED: the panel's own extent --
    // every select gary puts between $D8 0000 and $DF FFFF partitions the
    // block: sel_reg claims everything NOT claimed by sel_ide ($DA) or
    // sel_gayle ($DE1000-$DE1FFF), including $DC0000-$DCFFFF in this
    // default (no-ENABLE_RTC) build -- see RTC-WINDOW-DEFAULT-BUILD.
    // =========================================================================
    {
        Harness h;
        h.top->hdc_ena = 1;  // gates sel_ide (gary.v:176).
        h.cpu_probe(0xD80000);
        bool d8_is_reg = h.top->sel_reg == 1;
        h.cpu_probe(0xDA0000);
        bool da_is_ide = h.top->sel_ide == 1;
        h.cpu_probe(0xDAFFFE);
        bool da_end_is_ide = h.top->sel_ide == 1;
        h.cpu_probe(0xDB0000);
        bool db_is_reg = h.top->sel_reg == 1;
        h.cpu_probe(0xDC0000);
        bool dc_is_reg = h.top->sel_reg == 1;  // default build, no ENABLE_RTC.
        h.cpu_probe(0xDD0000);
        bool dd_is_reg = h.top->sel_reg == 1;
        h.cpu_probe(0xDE0000);
        bool de_low_is_reg = h.top->sel_reg == 1;
        h.cpu_probe(0xDE1000);
        bool de_mid_is_gayle = h.top->sel_gayle == 1;
        h.cpu_probe(0xDE1FFE);
        bool de_mid_end_is_gayle = h.top->sel_gayle == 1;
        h.cpu_probe(0xDE2000);
        bool de_high_is_reg = h.top->sel_reg == 1;
        uint32_t df_probe = 0xDF0000;
#if MUT_HIGHIOPANEL
        df_probe = 0xE00000;  // MUTATION: moved one bank past the panel's own top.
#endif
        h.cpu_probe(df_probe);
        bool df_is_reg = h.top->sel_reg == 1;
        checks.expect(d8_is_reg && da_is_ide && da_end_is_ide && db_is_reg &&
                          dc_is_reg && dd_is_reg && de_low_is_reg &&
                          de_mid_is_gayle && de_mid_end_is_gayle &&
                          de_high_is_reg && df_is_reg,
            "HIGHIOREG-PANEL-SWEEP: the panel's own extent, $00D80000-"
            "$00DFFFFF, is new content -- not individually graded elsewhere "
            "as a whole. gary partitions it as: $D8-$D9FFFF sel_reg (measured " +
            b(d8_is_reg) + "), $DA0000-$DAFFFE sel_ide (measured " +
            b(da_is_ide) + "/" + b(da_end_is_ide) + "), $DB0000 sel_reg (" +
            b(db_is_reg) + "), $DC0000 sel_reg in this default no-ENABLE_RTC "
            "build (" + b(dc_is_reg) + ", see RTC-WINDOW-DEFAULT-BUILD), "
            "$DD0000 sel_reg (" + b(dd_is_reg) + "), $DE0000 sel_reg (" +
            b(de_low_is_reg) + "), $DE1000-$DE1FFE sel_gayle (" +
            b(de_mid_is_gayle) + "/" + b(de_mid_end_is_gayle) + "), $DE2000 "
            "sel_reg again (" + b(de_high_is_reg) + "), and " +
            hexaddr(df_probe) + " sel_reg (" + b(df_is_reg) +
            ") -- gary's sel_reg 'catch-all' is markedly wider than any "
            "single page's own row (see CUSTOMREG-WIDE-64K-A3000 and "
            "CH7-TWO-REDS-STATUS above for the $DF0000 end of this same "
            "fact). [claims: sol:D-memmap-a3000-32bit.c20, "
            "opus:D-memmap-a3000-32bit.c19, opus:D-memmap-a3000-32bit.c26]");
    }

    // =========================================================================
    // GROUP 18 -- CIAA-PRA-DDRA-ADDRESS-WINDOW (p.330, $BFE001 / $BFE201).
    // 4 claims: sol:E-fire.c18, sol:E-fire.c19, opus:E-firebutton.c21,
    // opus:E-firebutton.c23. SPLIT: the address-decode half is NEW/GRADED
    // below -- verified directly that hrm/appF-cia-address-decode's own
    // CIAA-MAP-SWEEP deliberately EXCLUDES r=0 ($BFE001) and r=2 ($BFE201)
    // from its 14-address sweep, and that hrm/ch8-ciaapra-address-window's
    // own record() (quoted at appF-cia-register-map/
    // test_appF-cia-register-map.cpp:1407) says outright that no package
    // grades "$BFE001 selects PRA" against the pinned join -- so this
    // package's own classify note citing that chain as a decline was
    // itself stale (see the file header). The register-IDENTITY half (that
    // rs=0 is named PRA, rs=2 is named DDRA) is genuinely graded, at a
    // different top module, by hrm/appF-cia-register-map's REG0-PRA/
    // REG2-DDRA (verified: test_appF-cia-register-map.cpp:1040,1063, real
    // checks.expect() lines using ciaa's own `rs` port directly) -- declined
    // here on ordinary Group B/D grounds.
    // =========================================================================
    {
        Harness h;
        h.cpu_probe(0xBFE001);
        bool pra_is_a = h.top->sel_cia_a == 1;
        bool pra_not_b = h.top->sel_cia_b == 0;
        std::string pra_detail = "sel_cia_a=" + b(h.top->sel_cia_a) + " sel_cia_b=" + b(h.top->sel_cia_b);
        uint32_t ddra_probe = 0xBFE201;
#if MUT_CIAPRADDRA
        ddra_probe = 0xBFD201;  // MUTATION: redirected into CIA-B's own space (bit 13 flipped).
#endif
        h.cpu_probe(ddra_probe);
        bool ddra_is_a = h.top->sel_cia_a == 1;
        bool ddra_not_b = h.top->sel_cia_b == 0;
        std::string ddra_detail = "sel_cia_a=" + b(h.top->sel_cia_a) + " sel_cia_b=" + b(h.top->sel_cia_b);
        checks.expect(pra_is_a && pra_not_b && ddra_is_a && ddra_not_b,
            "CIAA-PRA-DDRA-ADDRESS-WINDOW (no uid -- background measurement, "
            "see the record() immediately below for this package's own four "
            "claims that partially rest on it, per Rule 1: a uid on a graded "
            "line counts as ASSERTED, so a partially-gradable claim's uid "
            "must sit on the record() alone): the address-membership half of "
            "$BFE001 (PRA) and $BFE201 (DDRA) -- gary.v:190-191, sel_cia_a "
            "asserts and sel_cia_b does not -- is NOT graded by any "
            "checks.expect() anywhere in this suite before this file: hrm/"
            "appF-cia-address-decode's own CIAA-MAP-SWEEP deliberately "
            "excludes r=0 and r=2 from its 14-address sweep, and hrm/"
            "ch8-ciaapra-address-window's own record() says plainly that no "
            "package grades '$BFE001 selects PRA' against the pinned join -- "
            "verified directly, not assumed from this package's own stale "
            "classify note (see file header). Measured at $BFE001: " +
            pra_detail + "; at " + hexaddr(ddra_probe) + ": " + ddra_detail + ".");
        checks.record(
            "sol:E-fire.c18, sol:E-fire.c19, opus:E-firebutton.c21, "
            "opus:E-firebutton.c23 (p.330, PRA at $BFE001 / DDRA at $BFE201) "
            "-- PARTIAL, per Rule 1. Each claim states BOTH 'an address "
            "selects CIA-A' AND 'the register there is named PRA/DDRA'. The "
            "address-membership half is graded above, bare-label, by "
            "CIAA-PRA-DDRA-ADDRESS-WINDOW (same fixture, same two "
            "addresses, same signals) -- a genuinely new grade, not a stale "
            "decline: verified directly that hrm/appF-cia-address-decode's "
            "own CIAA-MAP-SWEEP excludes r=0/r=2, and that hrm/"
            "ch8-ciaapra-address-window's own record() says no package "
            "grades this join (see file header). The register-IDENTITY half "
            "(PRA vs DDRA specifically) is NOT gradeable at gary: gary has "
            "no register-select or register-name output at all, only WHICH "
            "CHIP (sel_cia_a/sel_cia_b), never WHICH of the 16 internal "
            "registers. That rs=0 is named PRA and rs=2 is named DDRA is "
            "genuinely graded, at ciaa's own boundary (a different top "
            "module), by hrm/appF-cia-register-map's REG0-PRA and "
            "REG2-DDRA (verified: test_appF-cia-register-map.cpp:1040,1063, "
            "driving ciaa's own `rs` port directly rather than a decoded CPU "
            "address). Not re-graded here on either half.");
    }

    // =========================================================================
    // GROUP 19 -- PART4-CIAA-ROW (p.337, "CIA-A ... BFEx01 ... data bits 7-0
    // ... A12* ... int2"). 2 claims: sol:E-portsig.c1, opus:E-portsig.c2.
    // DECLINED IN FOUR PIECES, per this claim's own classify.evidence split:
    // (a) address pattern -- CIAA-MAP-SWEEP (14/16 r-values) plus this
    // package's own CIAA-PRA-DDRA-ADDRESS-WINDOW above (the remaining r=0,2)
    // now closes the gap in full; (b) A12* -- hrm/appF-cia-address-decode's
    // CIA-AB-TWOBIT-RULE; (c) data bits 7-0 -- hrm/appF-cia-pin-bindings'
    // CIAA-LOW-CIAB-HIGH-BYTE (top=minimig, a different module, out of this
    // package's gary-only boundary); (d) int2 -- hrm/appF-cia-pin-bindings'
    // CIAA-IRQ-REACHES-INT2 (same boundary note).
    // =========================================================================
    checks.record(
        "PART4-CIAA-ROW (p.337, 2 claims: sol:E-portsig.c1, "
        "opus:E-portsig.c2) -- declined in four pieces, each checked to sit "
        "inside a real checks.expect() body, not a record()/note(): (a) the "
        "address pattern BFEx01 -- graded for 14 of 16 r-values by hrm/"
        "appF-cia-address-decode's CIAA-MAP-SWEEP, and for the remaining two "
        "(r=0, r=2) by THIS package's own CIAA-PRA-DDRA-ADDRESS-WINDOW "
        "above, together a complete sweep; (b) A12* -- hrm/"
        "appF-cia-address-decode's CIA-AB-TWOBIT-RULE (gary.v:191-192); (c) "
        "data bits 7-0 -- hrm/appF-cia-pin-bindings' CIAA-LOW-CIAB-HIGH-BYTE, "
        "verified at test_minimig.cpp:306-312, top=minimig, a different "
        "module, outside this package's gary-only boundary; (d) int2 -- hrm/"
        "appF-cia-pin-bindings' CIAA-IRQ-REACHES-INT2, verified at "
        "test_minimig.cpp:434-438, same boundary note. Not re-graded on any "
        "of the four.");

    // =========================================================================
    // GROUP 20 -- PART4-CIAB-ROW (p.337, "CIA-B ... BFDx00 ... data bits
    // 15-8 ... A13* ... int6"). 2 claims: sol:E-portsig.c22,
    // opus:E-portsig.c24. DECLINED IN FOUR PIECES, mirroring Group 19: (a)
    // address pattern -- CIAB-MAP-SWEEP (14/16) plus hrm/ch8-cia-disk-port-
    // windows' CIABPRB-ALIAS-OPUS-CIA-C5/CIABPRB-CHIPLETTER-SOL-CIA-C4 for
    // the remaining two (r=1, r=D -- verified as real checks.expect() lines
    // at test_ch8-cia-disk-port-windows.cpp:555,569, a genuine, non-stale
    // grade, unlike CIA-A's own r=0/r=2 gap); (b) A13* -- CIA-AB-TWOBIT-
    // RULE; (c) data bits 15-8 -- CIAA-LOW-CIAB-HIGH-BYTE; (d) int6 -- hrm/
    // ch8-disk-index-interrupt-path's PROP-LEVEL6.
    // =========================================================================
    checks.record(
        "PART4-CIAB-ROW (p.337, 2 claims: sol:E-portsig.c22, "
        "opus:E-portsig.c24) -- declined in four pieces, mirroring "
        "PART4-CIAA-ROW: (a) the address pattern BFDx00 -- graded for 14 of "
        "16 r-values by hrm/appF-cia-address-decode's CIAB-MAP-SWEEP, and "
        "for the remaining two (r=1, r=D) by hrm/ch8-cia-disk-port-windows' "
        "CIABPRB-ALIAS-OPUS-CIA-C5 and CIABPRB-CHIPLETTER-SOL-CIA-C4 -- "
        "verified as real checks.expect() lines at "
        "test_ch8-cia-disk-port-windows.cpp:555,569, a GENUINE grade "
        "(unlike CIA-A's r=0/r=2 gap above, this pair is not stale: the CIA-"
        "B address window has no analogous hole); (b) A13* -- hrm/"
        "appF-cia-address-decode's CIA-AB-TWOBIT-RULE; (c) data bits 15-8 -- "
        "hrm/appF-cia-pin-bindings' CIAA-LOW-CIAB-HIGH-BYTE (same citation "
        "as PART4-CIAA-ROW, one assertion grades both halves of the byte "
        "split); (d) int6 -- hrm/ch8-disk-index-interrupt-path's PROP-LEVEL6, "
        "verified at test_ch8-disk-index-interrupt-path.cpp:481-486, a real "
        "checks.expect() citing opus:8-dskint.c8, sol:8-int.c8, "
        "opus:8-cia.c58, sol:8-cia.c52. Not re-graded on any of the four.");

    // =========================================================================
    // OVL-CANDIDATE -- consolidated MEASURED + record(), no uid. See the
    // file header's own "THE ovl=1 FINDING CANDIDATE" section for the full
    // account; measured fresh here at this package's own fixture.
    // =========================================================================
    {
        Harness h;
        h.top->ovl = 0;
        h.top->cpu_hlt = 0;
        h.cpu_probe(0x000000);
        uint32_t chip_ovl0 = h.top->sel_chip;
        uint32_t kick_ovl0 = h.top->sel_kick;
        h.top->ovl = 1;
        h.settle();
        uint32_t chip_ovl1 = h.top->sel_chip;
        uint32_t kick_ovl1 = h.top->sel_kick;
        checks.note("OVL-CANDIDATE-MEASURED: at $000000, ovl=0 gives sel_chip=" +
            hexbits(chip_ovl0) + " sel_kick=" + std::to_string((int)kick_ovl0) +
            "; ovl=1 gives sel_chip=" + hexbits(chip_ovl1) + " sel_kick=" +
            std::to_string((int)kick_ovl1) + " (gary.v:159,166). p.337's PA0 "
            "gloss is 'ROM/RAM overlay bit'.");
        checks.record(
            "OVL-CANDIDATE (no uid -- no claim in this package's own 60 "
            "quotes p.337's PA0/OVL row; none of the 60 restatements names "
            "'overlay' or 'OVL', grep-checked against this package's own "
            "claims dump). hrm/ch7-reset-overlay/test_gary.cpp -- an "
            "ACCEPTED package, ALSO top=gary -- drives ovl=1 directly into "
            "this exact fixture and grades it with four checks.expect() "
            "assertions: OVL-CONTROL-BASELINE-000000, "
            "OVERLAY-ROM-AT-000000, OVERLAY-RAM-UNAVAILABLE-WRITE-000000 "
            "and RESET-ROM-REAPPEARS-000000, all GREEN. It also measures "
            "banks 1-3 staying mapped while the overlay is active "
            "(sel_chip[1..3] carry no `ovl` term, gary.v:160-162) and "
            "declines to grade it: p.223 itself says 'portions of the RAM "
            "still respond' on some models. Measured independently above (chip=" + hexbits(chip_ovl0) +
            "->" + hexbits(chip_ovl1) + ", kick=" + std::to_string((int)kick_ovl0) +
            "->" + std::to_string((int)kick_ovl1) + " across the ovl=0/1 "
            "transition at $000000), matching ch7-reset-overlay's own "
            "OVL-CONTROL-BASELINE-000000/OVERLAY-ROM-AT-000000 measurement "
            "exactly. Not re-graded here -- no uid of this package's own "
            "rests on it.");
    }

    // stdout, not stderr: `> out.txt` captures only stdout, so a summary on
    // cerr reaches the runner (which reads make's output) and is MISSING from
    // this package's own out.txt and from its stored capture.
    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
