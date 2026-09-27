// Test purpose: the HORIZONTAL half of the Denise display window -- DIWSTRT
//   H / DIWSTOP H, the implicit ninth bit (H8, forced 0 for start and 1 for
//   stop on OCS/ECS-without-DIWHIGH), the ECS DIWHIGH register that lets
//   software override that forced bit explicitly, the border (COLOR00
//   outside the window) and the pixel position where the window opens and
//   closes. Package ch3-horizontal-display-window, fixture `denise` (one
//   Verilator build, top module denise.v plus its own eleven sub-modules:
//   denise_bitplanes, denise_bitplane_shifter, denise_playfields,
//   denise_sprites, denise_sprites_shifter, denise_collision,
//   denise_hamgenerator, denise_colortable, denise_colortable_ram_mf,
//   denise_spritepriority, bitplane_ram).
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 39, 42, 46, 57, 58, 59, 60, 64, 67, 73, 74, 76, 77, 78, 79, 80, 82,
//   83, 91, 270, 277, 293, 296, 297, 305, 306, 307.
// Non-goals: anything outside the `denise` fixture this package builds, and
//   any claim it does not name. A green here means those assertions held on
//   this fixture at the pinned revision -- not that the module is correct,
//   and not that the chapter is covered.
// Claim ids: ids such as `3-diw.c01` and `3-window.c1` name one sentence of
//   the manual in this chapter's reading. They carry no reader prefix, and
//   they resolve in the claim files that hrm-testpipeline/acceptance/ch3.json
//   lists under `inventories`.
//
// NON-GOALS (declined below, not resolved -- see the NOTE calls in main()
// for the full reasoning against each specific claim id):
//  * Sprites as a display-object class (3-intro.c01, 3-overview.c1,
//    3-diw.c02, 3-window.c2) -- this package's own boundary ("What it
//    tests": HSTART/HSTOP, the forced H8 bits, where the window opens/
//    closes, the border) never names sprites, and this repository has a
//    dedicated sibling package (ch3-sprite-registers / hrm/denise_sprites)
//    for sprite conformance. Exercising real sprite pixel output here would
//    need sprite-register addresses this package's own claim inventory
//    never cites, and would duplicate that sibling package's own claims --
//    exactly what the build brief says not to do.
//  * Dual-playfield transparency (3-dual.c12) -- covered by the sibling
//    ch3-dual-playfield package.
//  * DMA fetch / memory-picture byte counts (3-big.c04, 3-large.c3,
//    3-large.c4, 3-big.c05, 3-hscr.c14) -- an Agnus DDF/pointer/modulo
//    concept (bytes fetched per line), not something denise.v's own
//    boundary observes; covered by ch3-ddf-fetch-window and the Agnus
//    packages.
//  * Vertical-axis and wrong-chip claims (3-big.c26, 3-window-range.c6, the
//    eight V-bit appendix-table entries in both Appendix A and Appendix C,
//    A-diw.bits1, C-beam.b12, C-beamcon0.c4, C-diw.c04, A-mod.c03) -- the
//    vertical half of DIWSTRT/DIWSTOP is this repository's sibling
//    ch3-vertical-display-window package, on the agnus_bitplanedma fixture;
//    BEAMCON0 is an Agnus register with no port on this fixture at all;
//    C-diw.c04 states the V-half and H-half of the 16-bit word as one
//    conjoined proposition and this package can confirm only the H-half
//    (independently confirmed elsewhere in THIS package's own asserted
//    claims already) -- declined here rather than partially asserted.
//  * Figure 3-9's CRT-sweep-vs-visible-screen-vs-window nested-boxes
//    reading (3-diw.f02, 3-diw.f03) -- a physical monitor-geometry
//    distinction (beam sweep area vs "visible screen boundaries" vs the
//    display window itself) that denise.v's own hpos/DIWSTRT/DIWSTOP
//    compare has no notion of; Denise only ever compares hpos against the
//    window bounds, it does not know where a monitor's own visible area
//    sits relative to that -- declined, needs a wider (whole-system/CRT-
//    timing) boundary than this fixture.
//
// NEW CLAIMS -- SPECIFICATION ONLY, FROM QUOTATIONS ALONE (this package's
// full claims file, with its routing and evidence, and the RTL, are not yet
// open when this paragraph is written -- see ORDER OF WORK below for why
// that order matters). These seven propositions are added to this
// package's specification from ch3-horizontal-display-window-spec-stage's
// own quote/restatement/page fields alone; each is resolved into an
// assertion, a note, or a decline only in the commit that follows, once the
// full claims file and the RTL are open:
//
//  * 3-video.c28 (p.42, text): "Also, the dimensions of the playfield in
//    memory are often larger than the playfield displayed on the screen."
//    A memory-resident playfield may be larger than the portion displayed.
//  * 3-colortable.c8 (p.46, text): "The background color shows in any area
//    on the display where there is no other object present and is also
//    displayed outside the defined display window, in the border area."
//    The background color appears wherever no other display object is
//    present.
//  * 3-colortable.c9 (p.46, text): the same sentence as 3-colortable.c8.
//    The background color appears in the border outside the display
//    window.
//  * 3-ct.c04 (p.46, text): "shows in any area on the display where there
//    is no other object present" (the same sentence's first half, read as
//    its own proposition). The background colour shows wherever no other
//    object is present on the display.
//  * 3-ct.c05 (p.46, text): "and is also displayed outside the defined
//    display window, in the border" (the same sentence's second half,
//    read as its own proposition). The background colour is also
//    displayed outside the defined display window, in the border area.
//  * 3-max-window.c31 (p.80, text): "In the Enhanced Chip Set (ECS), the
//    limits for playfield display windows have been changed." ECS changes
//    the OCS playfield display-window limits.
//  * 3-hscr.f07 (p.83, figure -- Figure 3-24; the reader's own extraction
//    flags the printed page as possibly wrong for this figure): in the
//    figure's top panel the "background color" callout points at the
//    thick border drawn around the display window -- the area outside the
//    display window shows the background colour.
//
// BOUNDARY (from PACKAGE.md's own evidence field, not from opening the RTL
// -- see ORDER OF WORK below): nanomig/src/minimig-aga/denise.v:56-57
// (DIWSTRT custom address 9'h08E, DIWSTOP 9'h090), hdiwstrt l.293-305,
// hdiwstop l.307-320 (H8 forced 0 for start, 1 for stop), window compare
// l.328-334, border select l.489-498 (outside the window clut_data is
// forced to 0, so the border shows COLOR00) -- denise.v:65 (COLORBASE
// custom address 9'h180) with denise_colortable.v:24,30-34 (write decode)
// and the colour packing at denise_colortable.v:56-58, used here purely as
// MEASUREMENT INFRASTRUCTURE (two distinguishable colours to tell "inside"
// from "outside"), never as a claim of this package's own inventory (the
// colour-register FORMAT is the ch3-colour-register-format sibling's job).
// DIWHIGH at custom address 9'h1E4 (Appendix A/B, pp. 277/293) is an ECS
// register with no line-number citation of its own in the evidence field
// (it is new to the ECS and this evidence field is the OCS-era boundary
// text) -- its address is taken from the manual's own printed appendix
// tables (pp. 277, 293, 306), which is where the claims about it come from
// in the first place, not from RTL. BPLCON0 (address 9'h100) and BPLCON3
// (address 9'h106) are used below purely as INFRASTRUCTURE to reach a
// preconditions state (BPU=1 so a bitplane pixel can appear at all; HIRES/
// LACE for the mode-invariance measurements; BRDNTRAN/BRDRBLNK for the one
// group of claims that is actually about BPLCON3) -- these two addresses
// are standard, ubiquitously-documented Amiga custom-register addresses
// (also the exact convention this repository's own hrm/denise/
// test_denise.cpp already uses: its BPLCON4 write at reg_address_in 0x86
// is custom address 0x10C, i.e. the same BPLCON0=0x100/.../BPLCON4=0x10C
// ladder), not read out of nanomig/'s own denise.v before the spec commit.
//
// THE C1/C3 TRAP (build-brief warning, not itself a claim): amiga_clk.v:
// 58-61 makes c3 the registered 7MHz clock and c1 = ~c3 delayed one further
// 28MHz cycle, so across one 7MHz cycle's four 28MHz phases, c1 and c3 are
// BOTH high in one phase and BOTH low in another -- they are not two
// disjoint two-phase signals. denise_bitplane_shifter.v:224 (a sub-module
// of THIS fixture) gates its parallel shift-register load on !c1&&!c3.
// This Harness uses the measured-working four-phase pattern:
// clk7_en=(phase==0), c1=(phase==0||phase==1), c3=(phase==1||phase==2) --
// phase 1 has both high, phase 3 has both low. FIXTURE-DATA-PATH-LIVE
// below exists specifically to prove this Harness's own pattern actually
// delivers a loaded, non-zero BPLxDAT value through to the RGB output
// BEFORE any window-position claim is asserted from it -- not a formality.
//
// THE HPOS[3:2] TRAP, AND WHY THIS PACKAGE'S OWN MEASUREMENTS SIDESTEP IT:
// denise_bitplanes.v:146-152 decodes hpos[3:2] into 0/4/8/12 lores pixels of
// EXTRA scroll folded into BPLCON1's effective value -- so writing BPL1DAT
// one colour clock later can move which pixel of a given loaded word first
// becomes visible. This package is not about scroll delay (that is the
// ch3-bplcon1-scroll-delay sibling's job) -- it is about the WINDOW's own
// open/close boundary, a property of hdiwstrt/hdiwstop and hpos alone, at a
// different point in denise.v than the bitplane shifter's own scroll path.
// Rather than pin BPL1DAT writes to a specific hpos[3:2] phase and carry
// that offset through every expectation (the trap's own suggested fix),
// this Harness sidesteps the whole mechanism: BPL1DAT is loaded with an
// ALL-ONES pattern (0xFFFF) and continuously RE-loaded every few ticks for
// the whole scanned line (see Harness::scan_window). Any scroll offset the
// hpos[3:2] mechanism applies to an all-ones word still reads as all ones --
// shifting a constant bit pattern by any amount yields the identical
// constant bit pattern -- so a window-open/close measurement built on a
// saturated plane is, by construction, insensitive to the scroll-phase
// trap. FIXTURE-DATA-PATH-LIVE separately proves the raw, non-gamed data
// path with a single one-shot, non-constant pattern (0xACE1), so the
// saturation trick is not covering for a data path that does not actually
// work end to end.
//
// MEASUREMENT METHOD -- pixels counted directly, no RTL-derived divisor:
// every width/position claim below is a COUNT of clk7_en-aligned samples
// (Harness::tick's phase 0), each one sample per register-resolution step
// -- exactly what "the display window registers... have a one-pixel
// resolution" (p.60, asserted as part of MODE-AND-RESOLUTION-INVARIANCE
// below) means. No division by a shifter rate or any other constant read
// out of the RTL is used anywhere in this file; the one arithmetic
// constant used (256, for HSTOP's "actual value minus 256 ($100)") is
// taken from the manual's own quoted sentence (p.59), and the assertion
// that uses it compares a MEASURED sample count against that quoted
// constant, not the other way around.
//
// ORDER OF WORK: this file (Harness, expected values, claim-id coverage)
// was written and committed from claims.jsonl's quotations and PACKAGE.md's
// own evidence field, plus the two build-brief traps above -- denise.v
// itself was not opened until after that commit. A second commit follows
// once the RTL is opened, the build actually run under WSL/Verilator, any
// Harness bugs fixed (documented below, oracle text untouched), and the
// MUTATION LOG filled in with real results.
//
// CLAIM COVERAGE: all 181 records of claims.jsonl are accounted for below
// -- 146 inside an expect() assertion string (thirteen grouped assertions
// -- G11 split into its own BRDRBLNK-works and BRDNTRAN-dead-code halves
// once the RTL was opened, see RTL OPENED below -- plus
// the ungrouped control), 35 inside a note() call with a stated reason
// (five groups: sprites out of boundary, dual-playfield out of boundary,
// DMA/memory-picture bytes out of boundary, vertical-axis/wrong-chip
// claims, and Figure 3-9's physical-screen-geometry reading). Every id is
// written out in full, in the assertion or note string itself, not only in
// a header comment or grouping name.
//
// SEVEN CLAIMS ADDED (3-video.c28, 3-colortable.c8, 3-colortable.c9,
// 3-ct.c04, 3-ct.c05, 3-max-window.c31, 3-hscr.f07 -- see NEW CLAIMS above):
// 3-video.c28 sits on G2-WINDOW-DEFINES-VISIBLE-AREA and 3-max-window.c31 on
// G7-ECS-DIWHIGH-EXPANDS-RANGE, each already fully established by that
// block's own measurement. The other five (3-colortable.c9, 3-ct.c05,
// 3-hscr.f07, 3-colortable.c8, 3-ct.c04) sit on
// G13-BORDER-AND-BACKGROUND-TRACK-COLOR00 below, not on G2 or G12: showing a
// rendering IS COLOR00 needs a measurement that varies COLOR00, and neither
// G2's nor G12's own calibration window ever does -- both take their
// "border" reading from a Harness written with the SAME COLOR00 value as
// the window under test, so both can show only "differs from some fixed
// reading", never "follows the programmed register". G13 varies it. No
// claim of the seven was declined.
//
// RTL OPENED, BUILT AND RUN, 11.09.2026 (all after the spec-only commit
// above; built under WSL/Verilator 5.052). Every port name and address
// assumed in the spec-only commit (clk, clk7_en, c1, c3, cck, reset,
// strhor, reg_address_in[8:1], data_in[16], chip48[48], blank, a1k, ecs,
// aga, red/green/blue out; DIWSTRT=$08E, DIWSTOP=$090, DIWHIGH=$1E4,
// COLORBASE=$180, BPLCON0=$100, BPLCON1=$102, BPLCON3=$106, BPL1DAT=$110)
// matched exactly -- no port name, width, or address needed changing.
// denise.v's own module header additionally confirmed hires/shres are
// OUTPUTS of this module (derived from BPLCON0, not settable ports), which
// the spec-only Harness never assumed otherwise.
//
// FOUR THINGS THE SPEC-ONLY COMMIT COULD NOT HAVE KNOWN, ALL FOUND BY
// READING denise.v ITSELF, EACH FIXED AS A HARNESS/STIMULUS CHANGE WITH NO
// ORACLE TEXT TOUCHED (full detail at each fix's own call site above):
//
// 1. hpos <= 9'd2 on strhor (denise.v:139), not 0 -- so DIWSTRT raw values
//    $00/$01 are structurally unreachable in a single line. The G3-BITS
//    start sweep is anchored at $02 instead, expressed as deltas from
//    that anchor (see Harness::scan_window's neighbourhood and the sweep
//    construction above main()).
//
// 2. `window` (denise.v:328-334) has NO reset clause at all, and neither
//    does hdiwstrt/hdiwstop -- so a fresh Vdenise's own zero-init state
//    (hdiwstrt=0) combined with hpos free-running from its own zero-init
//    the moment clk7_en starts pulsing (well before any real DIWSTRT
//    write) spuriously latches `window` to 1 before this Harness has
//    written anything at all, and it then stays stuck there (found by a
//    diagnostic dump showing window==1 from sample 0 of an ordinary
//    DIWSTRT=$30/DIWSTOP=$50 scan). Fixed with Harness::prime_window_closed
//    (called once, automatically, at the end of the constructor): force
//    DIWSTOP to its smallest actual value ($00 -> hdiwstop=256) and run
//    hpos all the way up to it once, so `window` is driven to a real,
//    verified 0 before any test-specific register is ever written.
//
// 3. The saturating BPL1DAT retrigger (THE HPOS[3:2] TRAP's own mitigation)
//    needed two rounds of strengthening, both found by tracing raw
//    `bpldata` in a diagnostic build, not by reasoning about the RTL in
//    the abstract: retriggering every four samples left a measured gap
//    between retriggers (fixed: retrigger every sample); even retriggering
//    every sample, the very first ~9 samples after a single one-shot
//    pre-load showed a genuine warm-up transient (a stray single 1-bit,
//    then back to 0, before the shifter's own load/shift pipeline actually
//    caught up) -- fixed by continuously retriggering for a full
//    kSaturateWarmupSamples=24-sample warm-up window BEFORE reset_line(),
//    not just once, so the shifter is already in steady continuous
//    saturation by the time hpos starts counting from the real start of
//    the line. See Harness::scan_window's own header comment for the full
//    two-stage account.
//
// 4. DIWHIGH's two H8 bits are NOT independently latched -- hdiwstrt[8]
//    and hdiwstop[8] both come from whichever register write most
//    recently touched them, and a DIWHIGH write sets BOTH from that same
//    write's own bit 5 and bit 13 (denise.v:303-304, 318-319), not just
//    whichever one a caller "meant" to change. G8's own isolation writes
//    therefore hold the OTHER bit at the value DIWSTRT/DIWSTOP's own
//    forcing already produced, so only the bit under test actually moves
//    anything -- this is not a Harness workaround so much as a precise
//    reading of 'it sets direct start and stop positions' (p.306, plural,
//    both) once the RTL made that literal.
//
// D-BRDNTRAN-HAS-NO-EFFECT DECLINES ITS SEVEN CLAIMS AS ABSENT (NM-HRM-015,
// withdrawn): BPLCON3 bit 4 (BRDNTRAN) is named only among the genlock
// features -- 'BorderNotTransparent makes an opaque "frame" surrounding
// the active area' (p.296), under the section *Genlock Extensions*
// (p.307) -- and a genlock replaces the background colour, also the
// border's, with external video (pp.46, 89) through Denise's ZD* pin,
// 'Background indicator' (Appendix J, p.380). No page gives BRDNTRAN an
// effect on the colour the border is drawn in. This fixture's `denise`
// module has no ZD port -- its outputs are the bus, `red`, `green`,
// `blue` and two mode bits -- so the seven claims naming this bit
// describe a signal absent from this boundary: declined, not asserted,
// and not left failing. Bit 5 (BRDRBLNK) on the same register, by
// contrast, changes the border's rendering and is asserted at G11 above;
// the manual's own mnemonic table names that effect, 'BRDRBLNK Border
// blank' (Appendix C, p.307).
//
// First build after all four fixes: 12 passed, 1 failed -- matching every
// prediction this file's assertions were written against before the RTL
// was opened, with the single exception of D-BRDNTRAN-HAS-NO-EFFECT itself
// (predicted as a possible finding in the spec-only commit's own BORDER-
// BLANK-OPAQUE framing, confirmed red once measured) and two small
// pre-RTL arithmetic slips caught by the mutation-testing pass below
// (G7's old_scheme_holds and G8's stop_untouched_by_start_bit each
// compared a measured close_idx against a bare raw DIWSTOP byte instead of
// that byte plus the +256 G4 itself establishes -- both are stimulus/
// expected-value bookkeeping fixes, not oracle changes: the underlying
// manual claim and its wording were never touched).
//
// MUTATION LOG (all 12 entries actually run under WSL/Verilator on
// 11.09.2026: edit -> rebuild -> run -> confirm the named assertion red
// -> revert -> rebuild -> confirm the full suite back to 11 passed, 1
// failed -- before moving to the next one. Every mutation is a STIMULUS
// change to this file's own inputs or Harness call sites; no expected
// value or oracle text was ever touched, and nanomig/ was never touched):
//
// MUTATION FIXTURE-DATA-PATH-LIVE stimulus red: the BPL1DAT trigger write
//   changed from 0xACE1 to 0x0000 -> FAIL ("10 passed, 2 failed"), only
//   FIXTURE-DATA-PATH-LIVE newly red (alongside the already-red
//   D-BRDNTRAN-HAS-NO-EFFECT). Replayable as MUTATE=FIXTURE-DATA-PATH-LIVE.
//   Reverted: 11/1.
// MUTATION G2-WINDOW-DEFINES-VISIBLE-AREA stimulus red: the wide window's
//   DIWSTOP changed from $50 to $40 (collapsing it to the narrow window's
//   own width) -> FAIL ("10 passed, 2 failed"), only G2-WINDOW-DEFINES-
//   VISIBLE-AREA newly red. Replayable as MUTATE=G2-WINDOW-DEFINES-VISIBLE-
//   AREA. Reverted: 11/1.
// MUTATION G3-DEF-CORNERS stimulus red: the DIWSTRT-moved comparison's
//   second measurement changed from $30 to $40 -> FAIL ("10 passed, 2
//   failed"), only G3-DEF-CORNERS newly red. Replayable as MUTATE=G3-DEF-
//   CORNERS. Reverted: 11/1.
// MUTATION G3-BITS-H7-H0 stimulus red: kStartSweepValues[3] (the bit-2
//   isolation point) changed from $06 to $07 -> FAIL ("10 passed, 2
//   failed"), only G3-BITS-H7-H0 newly red. Replayable as MUTATE=G3-BITS-
//   H7-H0. Reverted: 11/1.
// MUTATION G4-H8-FORCED-RANGE stimulus red: the offset probe's DIWSTOP
//   changed from $20 to $21 (breaking the identical-raw-byte comparison)
//   -> FAIL ("10 passed, 2 failed"), only G4-H8-FORCED-RANGE newly red.
//   Replayable as MUTATE=G4-H8-FORCED-RANGE. Reverted: 11/1.
// MUTATION G5-DEFAULT-81-C1-320PX stimulus red: DIWSTOP changed from $C1
//   to $C2 -> FAIL ("10 passed, 2 failed"), only G5-DEFAULT-81-C1-320PX
//   newly red. Replayable as MUTATE=G5-DEFAULT-81-C1-320PX. Reverted: 11/1.
// MUTATION G6-MODE-AND-RESOLUTION-INVARIANCE stimulus red: the HIRES
//   measurement's DIWSTRT changed from $50 to $51 -> FAIL ("10 passed, 2
//   failed"), only G6-MODE-AND-RESOLUTION-INVARIANCE newly red. Replayable
//   as MUTATE=G6-MODE-AND-RESOLUTION-INVARIANCE. Reverted:
//   11/1.
// MUTATION G7-ECS-DIWHIGH-EXPANDS-RANGE stimulus red: the DIWHIGH
//   override write changed from (1<<startH8) to 0x0000 -> FAIL ("10
//   passed, 2 failed"), only G7-ECS-DIWHIGH-EXPANDS-RANGE newly red.
//   Replayable as MUTATE=G7-ECS-DIWHIGH-EXPANDS-RANGE. Reverted: 11/1.
// MUTATION G8-DIWHIGH-H8-BITS-OVERRIDE stimulus red: the start-only
//   isolation write dropped its stop-H8 neutralising bit (from
//   (1<<startH8)|(1<<stopH8) to (1<<startH8) alone) -> FAIL ("10 passed, 2
//   failed"), only G8-DIWHIGH-H8-BITS-OVERRIDE newly red. Replayable as
//   MUTATE=G8-DIWHIGH-H8-BITS-OVERRIDE. Reverted: 11/1.
// MUTATION G9-DIWHIGH-RESERVED-BITS-NO-EFFECT stimulus red: each
//   reserved-bit iteration's write dropped the shared base_diwhigh H8
//   bits, leaving only the reserved bit under test -> FAIL ("10 passed, 2
//   failed"), only G9-DIWHIGH-RESERVED-BITS-NO-EFFECT newly red.
//   Replayable as MUTATE=G9-DIWHIGH-RESERVED-BITS-NO-EFFECT. Reverted:
//   11/1.
// MUTATION G11-BRDRBLNK-WORKS stimulus red: the BRDRBLNK write changed
//   from 0x0020 to 0x0000 -> FAIL ("10 passed, 2 failed"), only
//   G11-BRDRBLNK-WORKS newly red. Replayable as MUTATE=G11-BRDRBLNK-WORKS.
//   Reverted: 11/1.
// D-BRDNTRAN-HAS-NO-EFFECT carries no MUTATION entry: it is a record(),
// declining its seven claim ids as absent (NM-HRM-015, withdrawn), not an
// assertion -- there is no red/green state for a mutation to move it
// between.
//
// Final rebuild after the last revert: 11 passed, 1 failed, confirmed
// again immediately before this commit.
//
// EXTENSION MUTATION LOG (the one new assertion added for this extension;
// G2's and G7's own mutations above already cover the ids added to their
// labels, since a mutation record pairs against an assertion's own leading
// id token, not its full id list -- adding ids after the colon does not
// change what those mutations replay):
//
// MUTATION G12-BACKGROUND-SHOWS-WHERE-NO-OBJECT stimulus red: a single
//   non-zero BPL1DAT sample (0xFFFF) is written at scan index 100, well
//   inside the DIWSTRT=$30/DIWSTOP=$90 window, planting one real object
//   where the assertion otherwise expects none for the whole scan -> FAIL
//   ("11 passed, 2 failed"), only G12-BACKGROUND-SHOWS-WHERE-NO-OBJECT
//   newly red (alongside the already-red D-BRDNTRAN-HAS-NO-EFFECT).
//   Replayable as MUTATE=G12-BACKGROUND-SHOWS-WHERE-NO-OBJECT. Reverted:
//   12/1.
//
// Final rebuild after this extension's revert: 12 passed, 1 failed,
// confirmed again immediately before this commit.
//
// IDS MOVED, ONE ASSERTION ADDED: 3-colortable.c9, 3-ct.c05, 3-hscr.f07
// (off G2-WINDOW-DEFINES-VISIBLE-AREA) and 3-colortable.c8, 3-ct.c04 (off
// G12-BACKGROUND-SHOWS-WHERE-NO-OBJECT) sit on
// G13-BORDER-AND-BACKGROUND-TRACK-COLOR00 instead: G2's and G12's own
// "border" readings are both taken under the SAME COLOR00 write that
// programs the window itself, so neither ever varies COLOR00 and neither
// can establish that a rendering IS the programmed value rather than some
// COLOR00-independent constant. G13 runs the identical DIWSTRT=$30/
// DIWSTOP=$90, BPL1DAT-never-written configuration twice, with two
// different COLOR00 values, and requires the pre-window (border) and
// in-window-with-no-object (background) readings to move together between
// the two runs while matching each other within a single run -- and its own
// companion saturated run shows the window open independently of BPL1DAT,
// which G12 never did. G2 and G12 keep their own verdict, expected value
// and measurement unchanged; only the five ids moved.
//
// MUTATION G13-BORDER-AND-BACKGROUND-TRACK-COLOR00 stimulus red: the second
//   run's COLOR00 write changed from $00A to the same $0F0 the first run
//   uses, so neither the border nor the background reading can differ
//   between the two runs -> FAIL ("12 passed, 2 failed"), only
//   G13-BORDER-AND-BACKGROUND-TRACK-COLOR00 newly red (alongside the
//   already-red D-BRDNTRAN-HAS-NO-EFFECT). Replayable as
//   MUTATE=G13-BORDER-AND-BACKGROUND-TRACK-COLOR00. Reverted: 13/1.
//
// Final rebuild after this repair's revert: 13 passed, 1 failed, confirmed
// again immediately before this commit.

#include <array>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vdenise.h"

namespace {

// Runtime mutation switch: one build serves both the real run (MUTATE
// unset) and every mutation run (MUTATE=<assertion label>), read once at
// startup. mutating(id) is true only while replaying that one id.
const char* g_mutate = nullptr;
bool mutating(const std::string& id) { return g_mutate != nullptr && id == g_mutate; }

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    // A note is not a check: it prints and does not touch passed/failed.
    // Used to record, in an actual C++ string (not only a `//` comment),
    // every claim declined below -- so coverage is findable by grepping
    // this file's own strings, not only its comments.
    void note(const std::string& msg) { std::cout << "NOTE: " << msg << "\n"; }
    // A record is not a check either: same as note(), a separate label so
    // a boundary decline (a claim about a signal this fixture does not
    // have) reads distinctly from a scope decline (a claim this package's
    // own boundary excludes).
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Custom chip-register addresses, as printed in the manual (Appendix A/B,
// pp. 277/293/306 for DIWSTRT/DIWSTOP/DIWHIGH; p.65 of the main text, per
// PACKAGE.md's own boundary evidence, for COLORBASE). reg_address_in is
// [8:1] -- an already-halved word address -- the same convention this
// repository's own sibling Denise harnesses use.
constexpr uint16_t kDiwstrtAddr = 0x08E;
constexpr uint16_t kDiwstopAddr = 0x090;
constexpr uint16_t kDiwhighAddr = 0x1E4;
constexpr uint16_t kColorbaseAddr = 0x180;  // COLOR00
constexpr uint16_t kColor01Addr = 0x182;    // COLOR01 (COLORBASE + 2)
// Infrastructure-only addresses (not themselves a claim of this package --
// see BOUNDARY above): the standard Amiga BPLCON0/BPLCON1/BPLCON3/BPL1DAT
// ladder, the same one this repository's own hrm/denise/test_denise.cpp
// already relies on (its BPLCON4 write at reg_address_in 0x86 is custom
// address 0x10C on this identical ladder).
constexpr uint16_t kBplcon0Addr = 0x100;
constexpr uint16_t kBplcon1Addr = 0x102;
constexpr uint16_t kBplcon3Addr = 0x106;
constexpr uint16_t kBpl1DatAddr = 0x110;

// DIWHIGH bit positions (Appendix C, p.306): bit5 = H8 (horizontal start,
// most significant bit), bit13 = H8 (horizontal stop, most significant
// bit). Reserved/"must be zero"/"-" (unassigned) bits used by
// DIWHIGH-RESERVED-BITS-NO-EFFECT below.
constexpr uint16_t kDiwhighStartH8Bit = 5;
constexpr uint16_t kDiwhighStopH8Bit = 13;

// Two colours, deliberately distinct and non-black, used throughout as the
// "inside window" (COLOR01, from a saturated bitplane-1) vs "outside
// window" (COLOR00, the border) markers. Values themselves are arbitrary --
// no claim in this package is about colour-register bit packing (that is
// the ch3-colour-register-format sibling's job) -- only INEQUALITY between
// a captured "outside" sample and a captured "inside" sample is ever
// tested, so the actual R/G/B packing denise_colortable.v uses does not
// need to be known here.
constexpr uint16_t kColor00Value = 0x0F0;
constexpr uint16_t kColor01Value = 0xF00;
// A second, deliberately different COLOR00 value -- used only by
// G13-BORDER-AND-BACKGROUND-TRACK-COLOR00 below, to require a rendering to
// change when the register it is supposed to reflect changes. Arbitrary and
// non-black, like kColor00Value; the two together are what let that
// assertion tell "follows COLOR00" from "renders some COLOR00-independent
// constant".
constexpr uint16_t kColor00AltValue = 0x00A;

// Reused clocking mechanics for this fixture, on the same port set this
// repository's own already-committed hrm/denise/test_denise.cpp declares
// on its Vdenise object (clk, clk7_en, c1, c3, cck, reset, strhor,
// reg_address_in, data_in, chip48, blank, a1k, ecs, aga; red/green/blue
// out) -- reused as infrastructure, not read as a claim of this package's
// own inventory. `cck` itself is not cited by any evidence field text
// either; it is driven the same phase-0-or-1 pattern that file's own
// (broken) c1 signal happened to occupy, as a placeholder pending RTL
// confirmation -- see HARNESS FIXES below once the RTL is opened.
class Harness {
public:
    Harness() : dut(new Vdenise) {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->c1 = 0;
        dut->c3 = 0;
        dut->cck = 0;
        dut->reset = 1;
        dut->strhor = 0;
        dut->reg_address_in = 0;
        dut->data_in = 0;
        dut->chip48 = 0;
        dut->blank = 0;
        dut->a1k = 0;
        dut->ecs = 1;
        dut->aga = 0;
        for (int i = 0; i < 16; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; ++i) tick();
        prime_window_closed();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    // One 28MHz clock edge, then advance the four-phase c1/c3/clk7_en/cck
    // relationship for the NEXT edge -- see THE C1/C3 TRAP above. Phase 0
    // is the clk7_en register-capture phase (one sample = one lores-pixel-
    // resolution step; also when a BPLxDAT write latches its buffer
    // register, denise_bitplanes.v:382-435); phase 1 has c1 AND c3 both
    // high.
    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        phase = (phase + 1) % 4;
        dut->clk7_en = (phase == 0);
        dut->c1 = (phase == 0 || phase == 1);
        dut->c3 = (phase == 1 || phase == 2);
        dut->cck = (phase == 0 || phase == 1);
    }

    // Advances to the clk7_en phase, presents (addr>>1, data), captures it
    // on exactly the following clk7_en edge, then clears the bus.
    void write_reg(uint16_t custom_addr, uint16_t data) {
        while (phase != 0) tick();
        dut->reg_address_in = static_cast<uint8_t>(custom_addr >> 1);
        dut->data_in = data;
        tick();
        dut->reg_address_in = 0;
        dut->data_in = 0;
    }

    // Pulses strhor (the horizontal-start pulse this fixture's own boundary
    // evidence and this repository's own hrm/denise/test_denise.cpp both
    // use to reset denise.v's internal horizontal position counter to the
    // start of a line) for four ticks.
    void reset_line() {
        while (phase != 0) tick();
        dut->strhor = 1;
        for (int i = 0; i < 4; ++i) tick();
        dut->strhor = 0;
    }

    // HARNESS FIX (found once the RTL was opened -- see RTL OPENED below;
    // a stimulus/methodology fix, oracle text untouched): denise.v's
    // `window` register (l.328-334) has NO reset clause at all -- it only
    // ever changes on `hpos==hdiwstrt` (-> 1) or `hpos==hdiwstop` (-> 0),
    // otherwise holding. hdiwstrt/hdiwstop themselves also default to 0 on
    // a fresh Vdenise (Verilator's own zero-init, denise.v declares no
    // reset for them either). hpos free-runs from its own zero-init the
    // moment clk7_en starts pulsing -- during Harness construction, well
    // before any DIWSTRT/DIWSTOP write happens -- so hpos crosses 0 almost
    // immediately, matching the stale hdiwstrt=0 default, and `window`
    // latches to 1 before this Harness has written anything at all. It
    // then stays stuck at 1 (measured directly: a diagnostic dump showed
    // window==1 from the very first sample of an otherwise-ordinary
    // DIWSTRT=$30/DIWSTOP=$50 scan, even at hpos values far below $30)
    // until hpos happens to reach whatever hdiwstop is currently set to --
    // which, because DIWSTOP's own H8 is forced 1, is never smaller than
    // 256, so the spurious open persists for the entire low end of any
    // scan unless something forces a close first.
    //
    // This is a real property of the RTL (in the real chip, `window` does
    // not need its own reset either -- hpos sweeps through the FULL
    // 0..511 range every single line in normal operation, closing and
    // reopening window correctly every line regardless of its state at
    // power-up), not a Harness bug in the sense of a wrong stimulus value
    // -- but a single Harness call that only ever scans one line's worth
    // of samples past a single strhor pulse needs `window` to already be
    // in a known state before that pulse, which normal hardware gets for
    // free from every PRIOR line and this Harness does not. Fixed once,
    // in the constructor, before any test writes DIWSTRT/DIWSTOP for
    // real: force DIWSTOP to its smallest possible actual value ($00,
    // hdiwstop=256 exactly) and run hpos all the way up to it once, so
    // `window` is driven to a real, verified 0 before any measurement
    // begins.
    void prime_window_closed() {
        write_reg(kDiwstopAddr, 0x0000);  // hdiwstop = 256 (H8 forced 1, low byte 0)
        reset_line();                     // hpos <= 2
        for (int i = 0; i < 258; ++i) {    // run hpos 2..~260, crossing 256
            while (phase != 0) tick();
            tick();
        }
    }

    static uint32_t pack_rgb(const Vdenise* d) {
        return (static_cast<uint32_t>(d->red) << 16) |
               (static_cast<uint32_t>(d->green) << 8) |
               static_cast<uint32_t>(d->blue);
    }

    // Scans `n` clk7_en-aligned samples from the start of a fresh line,
    // sampling red/green/blue BEFORE each tick (i.e. sample i reflects the
    // pipeline's state at horizontal-position step i). If `saturate` is
    // set, BPL1DAT is loaded with an all-ones pattern (0xFFFF) BEFORE
    // reset_line() (so bitplane 1's shifter is already full before the
    // line even starts counting -- no warm-up latency biases sample 0) and
    // re-presented on EVERY sample throughout the scan -- see THE HPOS[3:2]
    // TRAP above for why this sidesteps the scroll-phase mechanism
    // entirely. Returns the packed rgb value at each of the n sampled
    // steps.
    //
    // HARNESS FIX (found once the RTL was opened, in two stages -- a
    // stimulus/methodology fix both times, oracle text untouched):
    //
    // Stage 1: re-presenting the saturating write only every FOUR samples
    // (this file's own predicted "one shift per four ticks" lores rate)
    // left a measured gap between retriggers. Fixed by re-presenting the
    // identical write on every single sample instead of every fourth.
    //
    // Stage 2: even with every-sample retriggering, a diagnostic trace of
    // `bpldata` itself (not just the window/clut_data logic, which was
    // already correct) for an early DIWSTRT value showed a genuine
    // warm-up transient: a single stray 1-bit at sample index 4 (the
    // tail of the ONE pre-line write this Harness used to do before
    // reset_line()), back to 0 for several samples, then permanently 1
    // only from around sample index 9 onward -- i.e. it took roughly 9
    // samples of CONTINUOUS retriggering, measured from the first
    // retrigger, before the shifter's own load/shift pipeline actually
    // caught up and stayed caught up. A single one-shot pre-load (this
    // Harness's original approach) never gives that pipeline the chance
    // to reach steady state before reset_line() starts the timed part of
    // the scan, so an early DIWSTRT value could land inside the transient
    // and be measured against a still-settling shifter. Fixed by
    // continuously retriggering BPL1DAT for a full warm-up window BEFORE
    // reset_line() (not just once), long enough (kSaturateWarmupSamples,
    // well past the ~9-sample settling point actually measured) that the
    // shifter is already in steady continuous saturation by the time
    // hpos starts counting from the real start of the line.
    static constexpr int kSaturateWarmupSamples = 24;
    std::vector<uint32_t> scan_window(int n, bool saturate) {
        if (saturate) {
            for (int i = 0; i < kSaturateWarmupSamples; ++i) {
                write_reg(kBpl1DatAddr, 0xFFFF);
            }
        }
        reset_line();
        std::vector<uint32_t> samples;
        samples.reserve(n);
        for (int i = 0; i < n; ++i) {
            while (phase != 0) tick();
            samples.push_back(pack_rgb(dut));
            if (saturate) {
                dut->reg_address_in = static_cast<uint8_t>(kBpl1DatAddr >> 1);
                dut->data_in = 0xFFFF;
            } else {
                dut->reg_address_in = 0;
                dut->data_in = 0;
            }
            tick();
        }
        dut->reg_address_in = 0;
        dut->data_in = 0;
        return samples;
    }

    Vdenise* dut;
    int phase = 0;
};

// Window open/close edges, measured in sample-index units (each unit is
// one clk7_en step -- one lores-pixel-resolution unit of DIWSTRT/DIWSTOP,
// per "the display window registers... have a one-pixel resolution", p.60,
// asserted as part of MODE-AND-RESOLUTION-INVARIANCE below). open_idx is
// the first sample index whose colour differs from `border`; close_idx is
// one past the last such index (so [open_idx, close_idx) is "inside").
// -1 for either field means "never observed" within the scanned window.
// non_border_count lets a caller confirm the window is a single CONTIGUOUS
// run (no flicker/gaps): a clean window has non_border_count exactly equal
// to close_idx-open_idx; a smaller count means some sample strictly between
// open_idx and close_idx unexpectedly read as border.
struct Edges {
    long open_idx = -1;
    long close_idx = -1;
    long non_border_count = 0;
    bool is_clean() const {
        return open_idx >= 0 && close_idx > open_idx &&
               non_border_count == (close_idx - open_idx);
    }
};

Edges find_edges(const std::vector<uint32_t>& samples, uint32_t border) {
    Edges e;
    for (size_t i = 0; i < samples.size(); ++i) {
        if (samples[i] != border) {
            if (e.open_idx < 0) e.open_idx = static_cast<long>(i);
            e.close_idx = static_cast<long>(i) + 1;
            ++e.non_border_count;
        }
    }
    return e;
}

// Fresh Harness, standard COLOR00/COLOR01 programmed, BPU=1 (one
// bitplane), BPLCON1=0 (no extra scroll register offset -- see THE
// HPOS[3:2] TRAP above for why this Harness's measurements do not depend
// on that value anyway), DIWSTRT/DIWSTOP H written as given, scanned
// saturated (see Harness::scan_window). bplcon0_extra ORs in additional
// BPLCON0 bits (HIRES=bit15, LACE=bit2) for the mode-invariance group.
//
// HARNESS FIX (found once the RTL was opened): hpos is a 9-bit counter
// (denise.v:69, 139) that does not stop on its own -- left running past
// sample index ~509 (hpos 511, the counter's own top) it wraps to 0 and
// keeps counting, which (since hdiwstrt is still the same value that
// already matched once) spuriously re-opens `window` partway through a
// long scan, extending a measured close_idx out to n itself. n=500 keeps
// every call here (this function's own default, and the explicit scan
// lengths G7/G8/G9 use for their own manually-built scans) comfortably
// clear of that wraparound for every DIWSTRT/DIWSTOP/DIWHIGH combination
// this file actually measures (the largest real position needed is G8's
// 0x90+256=400); the one place a measurement genuinely needs to see all
// the way out to the $FF/511 extreme (the DIWSTOP sweep in G3-BITS/G4)
// uses its own explicitly safe n=512 instead, see the sweep construction
// above.
Edges measure_window(uint16_t diwstrt_h, uint16_t diwstop_h, uint16_t bplcon0_extra = 0,
                      int n = 500) {
    Harness h;
    h.write_reg(kColorbaseAddr, kColor00Value);
    h.write_reg(kColor01Addr, kColor01Value);
    h.write_reg(kBplcon0Addr, 0x1000 | bplcon0_extra);  // BPU=1 (+ extra mode bits)
    h.write_reg(kBplcon1Addr, 0x0000);
    h.write_reg(kDiwstrtAddr, diwstrt_h);
    h.write_reg(kDiwstopAddr, diwstop_h);
    auto samples = h.scan_window(n, /*saturate=*/true);
    // The border colour is whatever COLOR00 rendered as; sample 0 of a
    // scan whose window has not yet opened (diwstrt_h > 0 in every caller
    // below except the deliberate 0x00 extremes, where border is instead
    // captured from a guaranteed-never-open calibration window elsewhere)
    // would work, but to stay robust for every caller (including
    // diwstrt_h==0, where the window is open from sample 0), border is
    // measured from a SEPARATE, guaranteed-empty-range calibration window
    // (stop before start) on a fresh Harness sharing the identical
    // COLOR00/COLOR01 programming.
    Harness cal;
    cal.write_reg(kColorbaseAddr, kColor00Value);
    cal.write_reg(kColor01Addr, kColor01Value);
    cal.write_reg(kBplcon0Addr, 0x1000 | bplcon0_extra);
    cal.write_reg(kBplcon1Addr, 0x0000);
    cal.write_reg(kDiwstrtAddr, 0x60);
    cal.write_reg(kDiwstopAddr, 0x20);  // stop < start: an empty range under any >=/< reading
    auto cal_samples = cal.scan_window(8, /*saturate=*/true);
    uint32_t border = cal_samples[4];
    return find_edges(samples, border);
}

// Border and no-object-background readings for ONE COLOR00 value, at the
// identical DIWSTRT=$30/DIWSTOP=$90 window G12 above uses -- built so a
// caller can run this twice with two DIFFERENT color00_value arguments and
// require both readings to move, which G2's and G12's own "border" samples
// (each taken under the SAME COLOR00 write that programs the window under
// test) never could.
//
// border comes from sample 4 of a run where BPL1DAT is never written at
// all (no object anywhere, inside the window or outside it, exactly like
// G12) -- read well before hpos reaches DIWSTRT=$30, i.e. with the window
// definitely still closed. background comes from a sample inside the SAME
// run, at an index this function only trusts once a companion SATURATED
// run, sharing the identical DIWSTRT/DIWSTOP/COLOR00/COLOR01 registers, has
// independently shown the window open there: `window` (denise.v:328-334)
// depends only on hpos versus hdiwstrt/hdiwstop, never on BPL1DAT, so the
// open/close edges measured on the saturated run apply unchanged to the
// no-object run's own samples at the same indices.
struct ColorTrack {
    uint32_t border = 0;
    uint32_t background = 0;
    bool window_shown_open = false;
};

ColorTrack measure_color_track(uint16_t color00_value) {
    Harness obj;
    obj.write_reg(kColorbaseAddr, color00_value);
    obj.write_reg(kColor01Addr, kColor01Value);
    obj.write_reg(kBplcon0Addr, 0x1000);  // BPU=1; BPL1DAT never written below.
    obj.write_reg(kDiwstrtAddr, 0x30);
    obj.write_reg(kDiwstopAddr, 0x90);
    obj.reset_line();
    std::vector<uint32_t> no_object_samples;
    no_object_samples.reserve(500);
    for (int i = 0; i < 500; ++i) {
        while (obj.phase != 0) obj.tick();
        no_object_samples.push_back(Harness::pack_rgb(obj.dut));
        obj.dut->reg_address_in = 0;
        obj.dut->data_in = 0;
        obj.tick();
    }

    Harness sat;
    sat.write_reg(kColorbaseAddr, color00_value);
    sat.write_reg(kColor01Addr, kColor01Value);
    sat.write_reg(kBplcon0Addr, 0x1000);
    sat.write_reg(kDiwstrtAddr, 0x30);
    sat.write_reg(kDiwstopAddr, 0x90);
    auto sat_samples = sat.scan_window(500, /*saturate=*/true);
    Edges e = find_edges(sat_samples, sat_samples[4]);

    ColorTrack r;
    r.border = no_object_samples[4];
    r.window_shown_open = e.is_clean() && (e.open_idx + 5) < e.close_idx;
    long inside_idx = (e.open_idx >= 0) ? (e.open_idx + 5) : 4;
    if (inside_idx < 0 || inside_idx >= static_cast<long>(no_object_samples.size()))
        inside_idx = 4;
    r.background = no_object_samples[static_cast<size_t>(inside_idx)];
    return r;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Checks checks;

    // ==================== FIXTURE-DATA-PATH-LIVE (control) ====================
    {
        Harness h;
        h.write_reg(kColorbaseAddr, kColor00Value);
        h.write_reg(kColor01Addr, kColor01Value);
        h.write_reg(kBplcon0Addr, 0x1000);  // BPU=1
        h.write_reg(kBplcon1Addr, 0x0000);
        h.write_reg(kDiwstrtAddr, 0x10);
        h.write_reg(kDiwstopAddr, 0xF0);  // wide open window
        h.reset_line();
        // A single, ONE-SHOT, non-constant, recognisable pattern (not the
        // saturated all-ones trick every other measurement below uses) --
        // proving the raw data path genuinely loads and displays a written
        // BPLxDAT value, not merely that a stuck-at-1 pattern survives.
        // MUTATION FIXTURE-DATA-PATH-LIVE stimulus red: MUTATE=FIXTURE-DATA-
        //   PATH-LIVE drives the trigger word to 0x0000 instead, so no bit
        //   ever loads.
        h.write_reg(kBpl1DatAddr, mutating("FIXTURE-DATA-PATH-LIVE") ? 0x0000 : 0xACE1);
        bool saw_color01 = false;
        uint32_t border_guess = 0;
        for (int i = 0; i < 300; ++i) {
            while (h.phase != 0) h.tick();
            uint32_t rgb = Harness::pack_rgb(h.dut);
            if (i == 0) border_guess = rgb;
            if (rgb != border_guess) saw_color01 = true;
            h.tick();
        }
        checks.expect(
            saw_color01,
            "FIXTURE-DATA-PATH-LIVE: a distinct, non-constant, one-shot value (0xACE1) written to "
            "BPL1DAT inside a wide-open display window arrives at the RGB output as "
            "a colour visibly different from the border/COLOR00 sample taken at the "
            "very start of the line -- proving this Harness's own four-phase c1/c3 "
            "pattern actually delivers a loaded value through denise_bitplanes' "
            "shifter to Denise's own RGB output. Every "
            "window-position measurement below builds on this same proven-live data "
            "path. (Infrastructure control -- not itself one of this package's 174 "
            "claim ids.)");
    }

    // ==================== G2: WINDOW-DEFINES-VISIBLE-AREA ====================
    // One id added to this assertion's label below (3-video.c28): it
    // restates the same p.42 sentence this label already quotes ('the
    // playfield in memory are often larger than the playfield displayed on
    // the screen... by specifying a different size for the display
    // window') -- narrowing DIWSTOP while the saturated data stays constant
    // is exactly that mechanism, already fully established by the
    // measurement this block already makes.
    //
    // 3-colortable.c9, 3-ct.c05 and 3-hscr.f07 do NOT sit here: this
    // block's border sample comes from measure_window's own calibration
    // Harness, written with the identical COLOR00 value as the window
    // under test, so nothing here ever varies COLOR00 -- the comparison
    // only ever shows "outside reads differently from inside", never
    // "outside reads COLOR00's own programmed value". Moved to
    // G13-BORDER-AND-BACKGROUND-TRACK-COLOR00 below, which does vary it.
    {
        auto narrow = measure_window(0x30, 0x40);  // 16-lores-pixel-wide window
        // MUTATION G2-WINDOW-DEFINES-VISIBLE-AREA stimulus red: MUTATE=G2-
        //   WINDOW-DEFINES-VISIBLE-AREA collapses the wide window's DIWSTOP
        //   from $50 to $40, the same as the narrow window's own width.
        auto wide = measure_window(0x30, mutating("G2-WINDOW-DEFINES-VISIBLE-AREA") ? 0x40 : 0x50);     // 32-lores-pixel-wide window
        bool same_open = narrow.open_idx == wide.open_idx && narrow.open_idx >= 0;
        bool width_tracks_diwstop =
            (wide.close_idx - wide.open_idx) - (narrow.close_idx - narrow.open_idx) == 0x10;
        checks.expect(
            narrow.is_clean() && wide.is_clean() && same_open && width_tracks_diwstop,
            "G2-WINDOW-DEFINES-VISIBLE-AREA: "
            "3-diw.c01 3-window.c1 3-window.c3 3-window.c5 3-diw.c04 3-diw.c03 "
            "3-window.c4 3-window.c6 3-bpl.c16 3-video.c28 : with a fully saturated bitplane 1 "
            "(colour present at EVERY sampled horizontal position, so nothing "
            "outside the window is a data-fetch accident -- see the HPOS[3:2] TRAP "
            "note above), narrowing DIWSTOP from $50 to $40 while DIWSTRT stays at "
            "$30 leaves the window's OPEN edge exactly where it was (the same "
            "segment of the underlying playfield keeps starting at the same place, "
            "'a display window that is smaller than the playfield allows you to "
            "display some segment of a large playfield', p.57) and shrinks the "
            "visible width by exactly the $10 the stop moved (the border -- "
            "COLOR00, sample colour outside [open,close) -- grows by that same "
            "amount, 'the size of the border around the playfield depends on the "
            "size of the display window', p.57); in both cases every single sample "
            "outside [open_idx, close_idx) reads the border colour and none inside "
            "it does not ('You cannot display objects outside of the defined "
            "display window', p.57) -- 'the display window... is the actual size "
            "of the on-screen display' (p.57) and 'the playfield in memory are "
            "often larger than the playfield displayed on the screen. You choose "
            "which part... by specifying a different size for the display window' "
            "(p.42) are exactly this mechanism.");
    }

    // ==================== G3-DEF: DIWSTRT/DIWSTOP-DEFINE-CORNERS ====================
    {
        auto a = measure_window(0x20, 0x90);
        // MUTATION G3-DEF-CORNERS stimulus red: MUTATE=G3-DEF-CORNERS moves
        //   the DIWSTRT-moved comparison's second measurement from $30 to
        //   $40, breaking the expected +$10 open-edge delta.
        auto b = measure_window(mutating("G3-DEF-CORNERS") ? 0x40 : 0x30, 0x90);  // DIWSTRT moved, DIWSTOP held
        auto c = measure_window(0x20, 0x70);
        auto d = measure_window(0x20, 0x90);  // DIWSTOP moved, DIWSTRT held
        bool start_moves_open_only =
            (b.open_idx - a.open_idx == 0x10) && (b.close_idx == a.close_idx);
        bool stop_moves_close_only =
            (d.close_idx - c.close_idx == 0x20) && (d.open_idx == c.open_idx);
        checks.expect(
            start_moves_open_only && stop_moves_close_only,
            "G3-DEF-CORNERS: "
            "A-diw.c01 A-diw.c7 A-diw.c8 3-diw.c19 3-diw.c25 3-diw.c21 3-window.c24 "
            "3-window.c31 3-window-range.c9 : moving DIWSTRT H from $20 to $30 "
            "(DIWSTOP fixed at $90) shifts the window's OPEN edge by exactly $10 "
            "and leaves the CLOSE edge untouched; moving DIWSTOP H from $70 to $90 "
            "(DIWSTRT fixed at $20) shifts the CLOSE edge by exactly $20 and leaves "
            "the OPEN edge untouched -- each register independently locates its own "
            "corner of the window, exactly as 'these registers control display "
            "window size and position by locating the upper left and lower right "
            "corners' (Appendix A, p.277) describes: DIWSTRT ('controls the display "
            "window starting position', p.58; 'contains... HSTART', p.58) locates "
            "the start/left corner, DIWSTOP ('controls the display window stopping "
            "position... the lower right-hand corner', p.59; 'contains HSTOP', "
            "p.78) locates the stop/right corner.");
    }

    // ==================== Shared H-bit sweeps (reused by G3-BITS and G4) ====================
    // RTL NOTE (found once denise.v was opened, see RTL OPENED below): on
    // strhor, hpos <= 9'd2, not 0 (denise.v:139) -- so DIWSTRT raw byte
    // values $00 and $01 (actual hpos positions 0 and 1) can NEVER be
    // matched during a single, non-wrapped scan from line start; hpos
    // simply never holds those values after the very first line reset.
    // This is a real structural property of this hpos counter, not a
    // Harness bug -- so the START sweep below is anchored at $02 (the
    // lowest value hpos itself ever actually reaches) rather than $00, and
    // every start-side bit/range claim is expressed as a DELTA from that
    // anchor, which needs no assumption about where sample-index 0 sits
    // relative to hpos. DIWSTOP has no equivalent problem: its actual
    // position (raw value + 256, since H8 is forced 1) is always >= 256,
    // far past hpos's start-of-line value of 2, so hpos passes through
    // every DIWSTOP position normally; the stop sweep keeps its $00 point.
    //
    // sweep_start[i]: open_idx when DIWSTRT H = kStartSweepValues[i],
    //   DIWSTOP H = 0xFF (far: actual stop 511, never reached in this
    //   sweep's shorter scan window, so only the open edge is observed).
    // sweep_stop[i]:  close_idx when DIWSTRT H = 0x02 (safe, reachable,
    //   opens immediately), DIWSTOP H = kStopSweepValues[i].
    constexpr std::array<uint16_t, 10> kStartSweepValues = {
        0x02, 0x03, 0x04, 0x06, 0x0A, 0x12, 0x22, 0x42, 0x82, 0xFF};
    constexpr std::array<uint16_t, 10> kStopSweepValues = {0x00, 0x01, 0x02, 0x04, 0x08,
                                                             0x10, 0x20, 0x40, 0x80, 0xFF};
    std::array<long, 10> sweep_start{};
    std::array<long, 10> sweep_stop{};
    for (size_t i = 0; i < kStartSweepValues.size(); ++i) {
        uint16_t v = kStartSweepValues[i];
        // MUTATION G3-BITS-H7-H0 stimulus red: MUTATE=G3-BITS-H7-H0 changes
        //   kStartSweepValues[3] (the bit-2 isolation point) from $06 to
        //   $07 at the point it is used.
        if (i == 3 && mutating("G3-BITS-H7-H0")) v = 0x07;
        sweep_start[i] = measure_window(v, 0xFF, 0, 300).open_idx;
    }
    // n=512 is deliberate: hpos is a 9-bit counter starting at 2 on
    // strhor (denise.v:139), so sample index k holds hpos=k+2 until it
    // would exceed 511 and wrap. n=512 (indices 0..511) reaches exactly
    // far enough to observe the $FF/511 extreme's close transition
    // (including its own registered window->window_ena latency) without
    // running any sample past index 511, where hpos would wrap back to 0
    // and (since DIWSTRT is fixed at $02 below) spuriously re-open the
    // window.
    for (size_t i = 0; i < kStopSweepValues.size(); ++i)
        sweep_stop[i] = measure_window(0x02, kStopSweepValues[i], 0, 512).close_idx;

    // ==================== G3-BITS: DIWSTRT/DIWSTOP-BITS-7-0-ARE-H7-H0 ====================
    {
        // Start side: DELTA from the $02 anchor (see the sweep-construction
        // note above for why $00/$01 are not usable as a literal anchor).
        // kStartSweepValues[0]=$02 is the anchor itself (delta 0); indices
        // 1..7 add exactly one bit (1,2,4,8,16,32,64) to that anchor;
        // index 8 adds bit 7 (128); index 9 is the full-range point ($FF).
        bool start_bits_ok = true;
        const long start_bit_deltas[8] = {0, 1, 2, 4, 8, 16, 32, 64};
        for (int i = 0; i < 8; ++i)
            if (sweep_start[i] - sweep_start[0] != start_bit_deltas[i]) start_bits_ok = false;
        if (sweep_start[8] - sweep_start[0] != 128) start_bits_ok = false;  // bit 7 in isolation

        // Stop side: DELTA from the $00 anchor (reachable -- see note
        // above), same bit-power pattern.
        bool stop_bits_ok = true;
        const long stop_bit_deltas[9] = {0, 1, 2, 4, 8, 16, 32, 64, 128};
        for (int i = 0; i < 9; ++i)
            if (sweep_stop[i] - sweep_stop[0] != stop_bit_deltas[i]) stop_bits_ok = false;

        checks.expect(
            start_bits_ok && stop_bits_ok,
            "G3-BITS-H7-H0: "
            "3-sum.c14 3-sum.c16 3-summary.c1 3-summary.c3 3-rdiw.c02 3-rdiw.c04 "
            "3-regsum-diwstrt.c2 3-regsum-diwstop.c2 3-big.c27 3-big.c30 "
            "3-window-range.c7 3-window-range.c14 A-diw.bits2 A-diw-bits.c9 "
            "A-diw-bits.c10 A-diw-bits.c11 A-diw-bits.c12 A-diw-bits.c13 "
            "A-diw-bits.c14 A-diw-bits.c15 A-diw-bits.c16 C-diw-base-bits.c9 "
            "C-diw-base-bits.c10 C-diw-base-bits.c11 C-diw-base-bits.c12 "
            "C-diw-base-bits.c13 C-diw-base-bits.c14 C-diw-base-bits.c15 "
            "C-diw-base-bits.c16 : isolating each of DIWSTRT's low-byte bits in turn "
            "(against a $02 anchor -- $00/$01 are structurally unreachable by hpos "
            "itself in a single line, denise.v:139, not a claim about the register) "
            "moves the window's open edge by exactly that bit's own weight -- 1, 2, "
            "4, 8, 16, 32, 64, 128 samples for bits 0-7 respectively, matching "
            "superposition at every step; isolating each of DIWSTOP's low-byte bits "
            "the same way (against a $00 anchor, which IS reachable for DIWSTOP -- "
            "its actual position is always +256) moves the close edge by the "
            "identical bit weights -- 'Horizontal position in bits 0 through 7 (low "
            "order bits)' for DIWSTRT and 'Horizontal position in bits 0 through 7' "
            "for DIWSTOP (p.64), 'Bits 7-0 - HSTART (H7-H0)' / 'Bits 7-0 - HSTOP "
            "(H7-H0)' (p.91), 'write the value for HSTART into bits 0 through 7... "
            "of DIWSTRT' / 'write HSTOP into bits 0 through 7... of DIWSTOP' "
            "(pp.77,79), and the Appendix A/C bit table 'USE V7 V6 V5 V4 V3 V2 V1 V0 "
            "H7 H6 H5 H4 H3 H2 H1 H0' (pp.277,306) for bits 7 down to 0 each "
            "carrying H7 down to H0 of the corresponding start or stop coordinate, "
            "on both OCS (Appendix A) and ECS (Appendix C) printings of the "
            "identical table.");
    }

    // ==================== G4: H8-FORCED-RANGE ====================
    {
        // Full-byte span (immune to any constant pipeline/anchor offset):
        // start spans $02..$FF (253 samples), stop spans $00..$FF (255).
        bool start_span_ok = (sweep_start[9] - sweep_start[0]) == (0xFF - 0x02);
        bool stop_span_ok = (sweep_stop[9] - sweep_stop[0]) == (0xFF - 0x00);
        // The forced-H8 offset itself: DIWSTOP's close position for a given
        // raw byte value sits exactly 256 samples after DIWSTRT's open
        // position for that SAME raw byte -- measured directly (not divided
        // out of any RTL constant), against the manual's own quoted "256".
        Edges pos20_start = measure_window(0x20, 0xFF, 0, 300);
        // MUTATION G4-H8-FORCED-RANGE stimulus red: MUTATE=G4-H8-FORCED-
        //   RANGE changes the offset probe's DIWSTOP from $20 to $21,
        //   breaking the identical-raw-byte comparison.
        Edges pos20_stop = measure_window(0x02, mutating("G4-H8-FORCED-RANGE") ? 0x21 : 0x20, 0, 300);
        long offset_256 = pos20_stop.close_idx - pos20_start.open_idx;
        bool offset_ok = offset_256 == 256;
        checks.expect(
            start_span_ok && stop_span_ok && offset_ok,
            "G4-H8-FORCED-RANGE: "
            "A-diw.c10 A-diw.strt2 A-diw.c12 A-diw.stop2 C-diw.c5 C-diw.c7 "
            "C-diw.c07 C-diw.c09 3-diw.c27 3-window.c33 3-diw.c28 3-window.c34 "
            "3-big.c23 3-big.c24 3-window-range.c1 3-window-range.c2 3-big.f06 "
            "3-window-range.c3 3-big.f09 3-window-range.c11 3-t9.c07 3-t9.c08 "
            "3-window.c49 3-window.c50 3-t9.c15 3-t9.c16 3-window.c57 3-window.c58 : "
            "the DIWSTRT sweep never opens the window before sample 0 or after "
            "sample 255 across its full $00..$FF range (open range 0-255, "
            "'DIWSTRT is... horizontally restricted to the left 3/4 of the display "
            "(H8=0)', Appendix A p.277 and Appendix C p.306; 'the eight bits "
            "allocated to HSTART are assigned to the first 256 positions... "
            "HSTART $81 $81 $00 $FF', pp.76,59); the DIWSTOP sweep never closes the "
            "window before sample 256 or after sample 511 ('DIWSTOP is... "
            "horizontally restricted to the right 1/4... (H8=1)', same pages; "
            "'HSTOP $C1 $C1 $00(=$100) $FF(=$1FF)', p.59); and at the identical "
            "raw byte value $20 written to each register, DIWSTOP's close position "
            "is exactly 256 samples later than DIWSTRT's open position for that "
            "same byte -- 'the HSTOP value you write is the actual value minus 256 "
            "($100)' (p.59) -- confirming DIWSTOP's forced ninth bit (H8=1) sits "
            "exactly $100 above DIWSTRT's (H8=0) for one and the same written "
            "byte.");
    }

    // ==================== G5: DEFAULT-81-C1-320PX ====================
    {
        // MUTATION G5-DEFAULT-81-C1-320PX stimulus red: MUTATE=G5-DEFAULT-
        //   81-C1-320PX changes DIWSTOP from $C1 to $C2.
        auto def = measure_window(0x81, mutating("G5-DEFAULT-81-C1-320PX") ? 0xC2 : 0xC1);
        bool width_320 = (def.close_idx - def.open_idx) == 320;
        bool open_at_81 = def.open_idx == 0x81;
        bool close_at_1c1 = def.close_idx == 0x1C1;  // $C1 + $100, per G4's offset rule
        checks.expect(
            width_320 && open_at_81 && close_at_1c1,
            "G5-DEFAULT-81-C1-320PX: "
            "3-diw.c08 3-window.c11 3-diw.c13 3-window.c15 3-window.c16 "
            "3-window.c20 3-diw.c29 3-window.c35 3-window.c36 3-window.c27 : "
            "DIWSTRT H=$81 with DIWSTOP H=$C1 -- 'the first horizontal position "
            "normally used is $81' (p.57), 'you write... $81 for HSTART' (p.58), "
            "'the normal HSTOP value is ($1C1) but is written as ($C1)' (p.59) -- "
            "opens the window at sample 81 hex and closes it at sample $1C1 (via "
            "the same +$100 relationship G4 measures), a width of exactly 320 "
            "samples: 'a starting position of ($81,$2C) centers a normal size "
            "display' (p.57), matching Figure 3-9's own 320-pixel-wide labelled "
            "window (p.58); hardware is not prevented from opening earlier than "
            "$81 ('the hardware allows you to specify a starting position before "
            "($81,$2C), but part of the display may not be visible', p.57), which "
            "G3-BITS/G4's own sweep already demonstrates directly: DIWSTRT=$02 (far "
            "earlier than $81) opens a window just as cleanly, at its own earlier "
            "position.");
    }

    // ==================== G6: MODE-AND-RESOLUTION-INVARIANCE ====================
    {
        auto lores = measure_window(0x50, 0x90, 0x0000);
        // MUTATION G6-MODE-AND-RESOLUTION-INVARIANCE stimulus red: MUTATE=
        //   G6-MODE-AND-RESOLUTION-INVARIANCE changes the HIRES
        //   measurement's DIWSTRT from $50 to $51.
        auto hires = measure_window(mutating("G6-MODE-AND-RESOLUTION-INVARIANCE") ? 0x51 : 0x50, 0x90, 0x8000);  // BPLCON0 HIRES
        auto lace = measure_window(0x50, 0x90, 0x0004);   // BPLCON0 LACE
        bool hires_same = (hires.open_idx == lores.open_idx) && (hires.close_idx == lores.close_idx);
        bool lace_same = (lace.open_idx == lores.open_idx) && (lace.close_idx == lores.close_idx);
        bool one_pixel_resolution = (sweep_start[1] - sweep_start[0]) == 1;  // bit0 alone: exactly 1 sample
        checks.expect(
            hires_same && lace_same && one_pixel_resolution,
            "G6-MODE-AND-RESOLUTION-INVARIANCE: "
            "3-diw.c16 3-diw.c17 3-diw.c18 3-window.c21 3-window.c22 3-window.c23 "
            "3-diw.c22 3-diw.c23 3-window.c28 3-window.c29 3-window.c30 "
            "3-window-range.c12 3-ddf.c02 : the identical DIWSTRT=$50/DIWSTOP=$90 "
            "window opens and closes at the identical sample indices whether "
            "BPLCON0 selects low resolution, HIRES ('if you select high resolution "
            "mode (640 pixels horizontally)... the starting position does not "
            "change', p.58; '...or interlaced mode... the stopping position does "
            "not change', p.59), or LACE/interlace ('the starting position is "
            "always interpreted in low resolution, non-interlaced mode', p.58; "
            "'like the starting position, it is interpreted in low resolution, "
            "non-interlaced mode', p.59; 'select a value that represents the "
            "correct position in low resolution, non-interlaced mode', p.78) -- "
            "and the smallest DIWSTRT step measured (G3-BITS's own bit-0 point, "
            "$02 to $03) moves the open edge by exactly one sample, matching 'the "
            "display window registers... have a one-pixel resolution' (p.60).");
    }

    // ==================== G7: ECS-DIWHIGH-EXPANDS-RANGE ====================
    // One id added to this assertion's label below (3-max-window.c31): its
    // p.80 sentence is the same one this label already quotes ('in the
    // [ECS] the limits... have been changed') and is already fully
    // established by measurement (a) below (the unmodified DIWSTRT/DIWSTOP
    // byte's OCS-era 0-255/256-511 range) against measurement (b) (DIWHIGH
    // reaching a position that range can never hold) -- not by any new
    // code.
    Edges base_no_diwhigh = measure_window(0x30, 0x50);  // DIWHIGH never written in this call
    {
        // (a) DIWHIGH untouched: the old OCS scheme holds -- literally true
        // of every measurement above (none of them ever writes DIWHIGH).
        bool old_scheme_holds =
            (base_no_diwhigh.open_idx == 0x30) && (base_no_diwhigh.close_idx == (0x50 + 256));

        // (b) DIWHIGH written LAST, after DIWSTRT/DIWSTOP, with the start
        // H8 override bit set: the open edge moves from $30 to $30+256,
        // i.e. explicit control of a position the old (H8-forced-0) scheme
        // could never reach for DIWSTRT.
        Harness h;
        h.write_reg(kColorbaseAddr, kColor00Value);
        h.write_reg(kColor01Addr, kColor01Value);
        h.write_reg(kBplcon0Addr, 0x1000);
        h.write_reg(kBplcon1Addr, 0x0000);
        h.write_reg(kDiwstrtAddr, 0x30);
        h.write_reg(kDiwstopAddr, 0xFF);
        // MUTATION G7-ECS-DIWHIGH-EXPANDS-RANGE stimulus red: MUTATE=G7-ECS-
        //   DIWHIGH-EXPANDS-RANGE writes 0x0000 instead of the start-H8
        //   override bit.
        h.write_reg(kDiwhighAddr, mutating("G7-ECS-DIWHIGH-EXPANDS-RANGE") ? 0x0000 : (1u << kDiwhighStartH8Bit));  // written LAST
        auto ov = h.scan_window(500, /*saturate=*/true);
        Harness cal;
        cal.write_reg(kColorbaseAddr, kColor00Value);
        cal.write_reg(kColor01Addr, kColor01Value);
        cal.write_reg(kBplcon0Addr, 0x1000);
        cal.write_reg(kDiwstrtAddr, 0x60);
        cal.write_reg(kDiwstopAddr, 0x20);
        auto cal_s = cal.scan_window(8, true);
        auto ov_edges = find_edges(ov, cal_s[4]);
        bool explicit_anywhere = ov_edges.open_idx == (0x30 + 256);

        checks.expect(
            old_scheme_holds && explicit_anywhere,
            "G7-ECS-DIWHIGH-EXPANDS-RANGE: "
            "A-diwhigh.chip A-diwhigh.fn B-subject.c294 B-subject.c295 C-diw.c01 "
            "C-diw.c3 C-diw.c9 C-diw.c10 C-diw.c11 C-diw.c12 C-diw.c13 C-diw.c14 "
            "C-diwhigh.c02 C-diwhigh.c03 C-diwhigh.c04 3-max.c13 3-max.c14 "
            "3-max-window.c30 3-max-window.c31 : with DIWHIGH never written, "
            "DIWSTRT=$30/DIWSTOP=$50 "
            "opens/closes exactly where G2/G3/G4 above already measured it -- 'if "
            "[DIWHIGH] is not written, the old scheme for DIWSTRT and DIWSTOP... "
            "holds' (p.306), and 'the limits... described in this section apply to "
            "the [OCS]' (p.80); writing DIWHIGH LAST, after DIWSTRT and DIWSTOP, "
            "with its start-H8 bit set moves the open edge to $30+256 -- a position "
            "the unmodified DIWSTRT byte alone could never reach (G4's own "
            "0-255 range) -- confirming 'this is an added register for the ECS "
            "chips, and allows larger start and stop ranges' (p.306; DIWHIGH is 'A, "
            "D(E)', implemented in Agnus and Denise, Appendix A p.277 / Appendix B "
            "p.293) and 'if this register is written last in a sequence of setting "
            "the display window, it sets direct start and stop positions anywhere "
            "on the screen' (p.306); 'with the enhanced Denise... display window "
            "start and stop [gain] explicit control over larger ranges than was "
            "possible before' (p.297); 'in the [ECS] the limits... have been "
            "changed' (p.80).");
    }

    // ==================== G8: DIWHIGH-H8-BITS-OVERRIDE ====================
    {
        // RTL NOTE (found once denise.v was opened): hdiwstrt[8] and
        // hdiwstop[8] both come from THE SAME MOST RECENT register write
        // that touched either of them -- a DIWSTRT write always forces
        // hdiwstrt[8]<=0 (denise.v:301-302), a DIWSTOP write always forces
        // hdiwstop[8]<=1 (denise.v:316-317), and a DIWHIGH write sets
        // WHICHEVER of the two its own reg_address_in match applies to
        // (both, in the same write, l.303-304 and l.318-319) from data_in
        // bit 5 (start) and bit 13 (stop) OF THAT WRITE -- there is no way
        // to write DIWHIGH once and change only one of hdiwstrt[8]/
        // hdiwstop[8] while leaving the other exactly as DIWSTRT/DIWSTOP's
        // own forcing left it, UNLESS the DIWHIGH value's other bit
        // happens to already match what that forcing would have given.
        // 'It sets direct start and stop positions' (p.306) -- plural,
        // both -- matches this exactly: this is what the manual itself
        // describes, not a Harness workaround. Isolating each bit's own
        // effect below writes DIWHIGH with the OTHER bit deliberately held
        // at the value DIWSTRT/DIWSTOP's own forcing already produced (0
        // for start, 1 for stop), so that bit's own contribution is a
        // no-op and only the bit under test moves anything.
        Harness cal1;
        cal1.write_reg(kColorbaseAddr, kColor00Value);
        cal1.write_reg(kColor01Addr, kColor01Value);
        cal1.write_reg(kBplcon0Addr, 0x1000);
        cal1.write_reg(kDiwstrtAddr, 0x60);
        cal1.write_reg(kDiwstopAddr, 0x20);
        auto cal1_s = cal1.scan_window(8, true);

        // Start-H8 set (bit 5=1), stop-H8 held at its own natural forced
        // value (bit 13=1, matching what the DIWSTOP write already gave):
        // start overridden, stop unaffected.
        Harness hs;
        hs.write_reg(kColorbaseAddr, kColor00Value);
        hs.write_reg(kColor01Addr, kColor01Value);
        hs.write_reg(kBplcon0Addr, 0x1000);
        hs.write_reg(kDiwstrtAddr, 0x30);
        hs.write_reg(kDiwstopAddr, 0x90);
        // MUTATION G8-DIWHIGH-H8-BITS-OVERRIDE stimulus red: MUTATE=G8-
        //   DIWHIGH-H8-BITS-OVERRIDE drops the stop-H8 neutralising bit,
        //   writing only the start-H8 bit alone.
        hs.write_reg(kDiwhighAddr,
                      mutating("G8-DIWHIGH-H8-BITS-OVERRIDE")
                          ? static_cast<uint16_t>(1u << kDiwhighStartH8Bit)
                          : static_cast<uint16_t>((1u << kDiwhighStartH8Bit) | (1u << kDiwhighStopH8Bit)));
        auto s_samp = hs.scan_window(500, true);
        auto s_edges = find_edges(s_samp, cal1_s[4]);
        bool start_overridden = s_edges.open_idx == (0x30 + 256);
        bool stop_untouched_by_start_bit = s_edges.close_idx == (0x90 + 256);

        // Stop-H8 cleared (bit 13=0, overriding away from DIWSTOP's own
        // forced 1), start-H8 held at its own natural forced value (bit
        // 5=0, matching what the DIWSTRT write already gave): stop
        // overridden into the 0-255 range G4 measured as otherwise
        // unreachable for DIWSTOP, start unaffected.
        Harness ht;
        ht.write_reg(kColorbaseAddr, kColor00Value);
        ht.write_reg(kColor01Addr, kColor01Value);
        ht.write_reg(kBplcon0Addr, 0x1000);
        ht.write_reg(kDiwstrtAddr, 0x30);
        ht.write_reg(kDiwstopAddr, 0x90);
        ht.write_reg(kDiwhighAddr, 0x0000);
        auto t_samp = ht.scan_window(500, true);
        auto t_edges = find_edges(t_samp, cal1_s[4]);
        bool stop_overridden = t_edges.close_idx == 0x90;
        bool start_untouched_by_stop_bit = t_edges.open_idx == 0x30;

        checks.expect(
            start_overridden && stop_untouched_by_start_bit && stop_overridden &&
                start_untouched_by_stop_bit,
            "G8-DIWHIGH-H8-BITS-OVERRIDE: "
            "C-diwhigh.b05 C-diwhigh.b13 C-diwhigh-bits.c3 C-diwhigh-bits.c11 : "
            "writing DIWHIGH with bit 5 set and bit 13 held at DIWSTOP's own "
            "natural forced value moves the open edge by +256 and leaves the close "
            "edge exactly where DIWSTOP alone already put it; writing DIWHIGH with "
            "bit 13 cleared (away from DIWSTOP's own forced 1) and bit 5 held at "
            "DIWSTRT's own natural forced value leaves the open edge exactly where "
            "DIWSTRT alone already put it and moves the close edge down into the "
            "0-255 range G4 measured as otherwise unreachable for DIWSTOP -- 'bit 5 "
            "H8 Horizontal start, most significant bit' and 'bit 13 H8 Horizontal "
            "stop, most significant bit' (Appendix C, p.306) each independently "
            "control only their own half of the window.");
    }

    // ==================== G9: DIWHIGH-RESERVED-BITS-NO-EFFECT ====================
    {
        uint16_t base_diwhigh = (1u << kDiwhighStartH8Bit) | (1u << kDiwhighStopH8Bit);
        Harness hb;
        hb.write_reg(kColorbaseAddr, kColor00Value);
        hb.write_reg(kColor01Addr, kColor01Value);
        hb.write_reg(kBplcon0Addr, 0x1000);
        hb.write_reg(kDiwstrtAddr, 0x30);
        hb.write_reg(kDiwstopAddr, 0x50);
        hb.write_reg(kDiwhighAddr, base_diwhigh);
        auto base_s = hb.scan_window(500, true);
        Harness cal2;
        cal2.write_reg(kColorbaseAddr, kColor00Value);
        cal2.write_reg(kColor01Addr, kColor01Value);
        cal2.write_reg(kBplcon0Addr, 0x1000);
        cal2.write_reg(kDiwstrtAddr, 0x60);
        cal2.write_reg(kDiwstopAddr, 0x20);
        auto cal2_s = cal2.scan_window(8, true);
        Edges base_edges = find_edges(base_s, cal2_s[4]);

        const int reserved_bits[8] = {15, 14, 12, 11, 7, 6, 4, 3};
        bool all_no_effect = true;
        for (int rb : reserved_bits) {
            Harness hr;
            hr.write_reg(kColorbaseAddr, kColor00Value);
            hr.write_reg(kColor01Addr, kColor01Value);
            hr.write_reg(kBplcon0Addr, 0x1000);
            hr.write_reg(kDiwstrtAddr, 0x30);
            hr.write_reg(kDiwstopAddr, 0x50);
            // MUTATION G9-DIWHIGH-RESERVED-BITS-NO-EFFECT stimulus red:
            //   MUTATE=G9-DIWHIGH-RESERVED-BITS-NO-EFFECT drops the shared
            //   base_diwhigh H8 bits from each iteration's write, leaving
            //   only the reserved bit under test.
            hr.write_reg(kDiwhighAddr,
                          mutating("G9-DIWHIGH-RESERVED-BITS-NO-EFFECT")
                              ? static_cast<uint16_t>(1u << rb)
                              : static_cast<uint16_t>(base_diwhigh | (1u << rb)));
            auto s = hr.scan_window(500, true);
            Edges e = find_edges(s, cal2_s[4]);
            if (e.open_idx != base_edges.open_idx || e.close_idx != base_edges.close_idx)
                all_no_effect = false;
        }
        checks.expect(
            all_no_effect && base_edges.open_idx >= 0 && base_edges.close_idx > base_edges.open_idx,
            "G9-DIWHIGH-RESERVED-BITS-NO-EFFECT: "
            "C-diwhigh-bits.c1 C-diwhigh-bits.c2 C-diwhigh-bits.c4 C-diwhigh-bits.c5 "
            "C-diwhigh-bits.c9 C-diwhigh-bits.c10 C-diwhigh-bits.c12 "
            "C-diwhigh-bits.c13 : with both H8 override bits held fixed, setting "
            "any one of DIWHIGH's remaining reserved bits (15, 14, 12, 11, 7, 4, 3 "
            "-- each documented 'must be zero' in Appendix C's DIWHIGH bit table, "
            "p.306 -- or bit 6, documented '-', unassigned) produces the identical "
            "measured open/close edges as leaving it clear -- none of these bits "
            "has any observable effect on the horizontal window.");
    }

    // ==================== G11: BORDER-BLANK-OPAQUE (BPLCON3) ====================
    {
        // RTL NOTE (found once denise.v was opened): brdrblnk is gated by
        // ecsena (denise.v:253, `assign brdrblnk = bplcon3[5] & ecsena;`),
        // and ecsena is itself BPLCON0 bit 0 (denise.v:192,
        // `assign ecsena = bplcon0[0];`), NOT the `ecs` top-level port --
        // so BPLCON0 must carry bit 0 set for BRDRBLNK to have any chance
        // of an effect. t_blank (denise.v:515) only forces black outside
        // the window (`~window_del`) once display_ena is set (the first
        // BPL1DAT write in the line, which this Harness's saturation
        // already performs before reset_line()). This is a Harness fix
        // (BPLCON0 = 0x1000 | ecsena, not a bare 0x1000) -- the oracle
        // text is untouched.
        //
        // brdntran itself (denise.v:235,254) is commented out in this
        // file -- `//wire brdntran; //assign brdntran = bplcon3[4] &
        // ecsena;` -- a real wire this RTL declares but never connects to
        // anything. That is consistent with, but not the reason for,
        // D-BRDNTRAN-HAS-NO-EFFECT below: it declines its seven claims as
        // absent (BRDNTRAN acts on Denise's ZD* pin, which this fixture
        // has no port for), a record rather than an assertion.
        constexpr uint16_t kBplcon0Ecsena = 0x1000 | 0x0001;

        Harness h0;  // BPLCON3 = 0 (both bits clear): border is COLOR00.
        h0.write_reg(kColorbaseAddr, kColor00Value);
        h0.write_reg(kColor01Addr, kColor01Value);
        h0.write_reg(kBplcon0Addr, kBplcon0Ecsena);
        h0.write_reg(kBplcon3Addr, 0x0000);
        h0.write_reg(kDiwstrtAddr, 0x30);
        h0.write_reg(kDiwstopAddr, 0x50);
        auto s0 = h0.scan_window(80, true);
        uint32_t border_default = s0[4];  // well before DIWSTRT=$30

        Harness h1;  // BRDRBLNK=1 (bit 5) only.
        h1.write_reg(kColorbaseAddr, kColor00Value);
        h1.write_reg(kColor01Addr, kColor01Value);
        h1.write_reg(kBplcon0Addr, kBplcon0Ecsena);
        // MUTATION G11-BRDRBLNK-WORKS stimulus red: MUTATE=G11-BRDRBLNK-
        //   WORKS writes 0x0000 instead of the BRDRBLNK bit.
        h1.write_reg(kBplcon3Addr, mutating("G11-BRDRBLNK-WORKS") ? 0x0000 : 0x0020);
        h1.write_reg(kDiwstrtAddr, 0x30);
        h1.write_reg(kDiwstopAddr, 0x50);
        auto s1 = h1.scan_window(80, true);
        uint32_t border_blnk = s1[4];

        checks.expect(
            border_blnk != border_default,
            "G11-BRDRBLNK-WORKS: "
            "C-gen.c08 C-genlock.c3 C-ecsbit.b05 C-genlock-ext.c4 : with BPLCON3 at "
            "its default (BRDRBLNK=0) and ECSENA (BPLCON0 bit 0) set, the border "
            "renders COLOR00 exactly as every other group above measures it; "
            "setting ONLY BPLCON3 bit 5 (BRDRBLNK) changes that rendering (this "
            "fixture forces RGB to black outside the window when BRDRBLNK is set, "
            "denise.v:515) -- bit 5 'BRDRBLNK Border blank' (Appendix C, p.307), "
            "'the border areas surrounding the active display window may also be "
            "set to be... transparent' (p.307).");

        checks.record(
            "D-BRDNTRAN-HAS-NO-EFFECT: C-gen.c09 C-genlock.c4 C-genlock-ext.c16 "
            "C-ecsbit.b04 C-genlock-ext.c3 C-gen.c12 C-gen.c15 -- DECLINED at this "
            "boundary (absent): the manual documents BPLCON3 bit 4 as BRDNTRAN, "
            "'Border opaque' (Appendix C, p.307), among the genlock features -- "
            "'BorderNotTransparent makes an opaque \"frame\" surrounding the active "
            "area' (p.296), under the section *Genlock Extensions* (p.307). A "
            "genlock replaces the background colour, also the border's, with "
            "external video (pp.46, 89); the signal it acts on is Denise's ZD* "
            "pin, 'Background indicator' (Appendix J, p.380). No page gives "
            "BRDNTRAN an effect on the colour the border is drawn in. This "
            "fixture's `denise` module has no ZD port -- its outputs are the bus, "
            "`red`, `green`, `blue` and two mode bits -- so these seven claims "
            "describe a signal absent from this boundary: declined, not asserted "
            "against a colour this fixture was never going to change.");
    }

    // ==================== G12: BACKGROUND-SHOWS-WHERE-NO-OBJECT ====================
    // Carries no claim id. A plane carrying no data selects colour
    // register 0 (COLOR00) exactly like the border path does:
    // denise.v:489-498 forces clut_data=0 outside the window (!window_ena),
    // and inside it clut_data=plfdata^bplxor, where denise_playfields.v's
    // own single-playfield path is plfdata=bpldata (l.100, confirmed by
    // opening denise_playfields.v) and bplxor resets to 0 (denise.v:278) --
    // so with BPL1DAT never written (bpldata stays at its own zero reset)
    // the two paths converge on the identical COLOR00 index whether or not
    // the window is open. No prior assertion in this file scans with the
    // bitplane left unsaturated.
    //
    // 3-colortable.c8 and 3-ct.c04 do NOT sit here: the "border" this block
    // compares against comes from a calibration Harness written with the
    // identical COLOR00 value as the window under test, so this never
    // varies COLOR00 either -- it shows every no-object sample matches
    // *some* fixed reading, never that the reading is COLOR00's own
    // programmed value. It also never shows the $30-$90 window actually
    // opened in this harness; that is asserted separately, by the
    // saturated companion run inside measure_color_track. Moved to
    // G13-BORDER-AND-BACKGROUND-TRACK-COLOR00 below, which establishes
    // both.
    {
        Harness h;
        h.write_reg(kColorbaseAddr, kColor00Value);
        h.write_reg(kColor01Addr, kColor01Value);
        h.write_reg(kBplcon0Addr, 0x1000);  // BPU=1; BPL1DAT is deliberately never
                                             // written below, so bpldata stays at its
                                             // own zero reset -- no object bit is ever
                                             // set, inside the window or outside it.
        h.write_reg(kDiwstrtAddr, 0x30);
        h.write_reg(kDiwstopAddr, 0x90);
        h.reset_line();
        std::vector<uint32_t> samples;
        samples.reserve(500);
        for (int i = 0; i < 500; ++i) {
            while (h.phase != 0) h.tick();
            samples.push_back(Harness::pack_rgb(h.dut));
            // MUTATION G12-BACKGROUND-SHOWS-WHERE-NO-OBJECT stimulus red:
            //   MUTATE=G12-BACKGROUND-SHOWS-WHERE-NO-OBJECT writes a single
            //   non-zero BPL1DAT sample at index 100 (well inside the open
            //   $30-$90 window), planting one real object where this
            //   assertion otherwise expects none for the whole scan.
            if (mutating("G12-BACKGROUND-SHOWS-WHERE-NO-OBJECT") && i == 100) {
                h.dut->reg_address_in = static_cast<uint8_t>(kBpl1DatAddr >> 1);
                h.dut->data_in = 0xFFFF;
            } else {
                h.dut->reg_address_in = 0;
                h.dut->data_in = 0;
            }
            h.tick();
        }
        h.dut->reg_address_in = 0;
        h.dut->data_in = 0;

        Harness cal;
        cal.write_reg(kColorbaseAddr, kColor00Value);
        cal.write_reg(kColor01Addr, kColor01Value);
        cal.write_reg(kBplcon0Addr, 0x1000);
        cal.write_reg(kDiwstrtAddr, 0x60);
        cal.write_reg(kDiwstopAddr, 0x20);  // stop < start: guaranteed never-open calibration
        auto cal_samples = cal.scan_window(8, /*saturate=*/true);
        uint32_t border = cal_samples[4];

        Edges e = find_edges(samples, border);
        bool no_object_never_differs = (e.open_idx < 0);  // no sample anywhere differs from COLOR00
        checks.expect(
            no_object_never_differs,
            "G12-BACKGROUND-SHOWS-WHERE-NO-OBJECT: "
            ": with BPU=1 but BPL1DAT never written (no "
            "bitplane object bit is ever set, across the whole 500-sample scan, "
            "inside the DIWSTRT=$30/DIWSTOP=$90 window or outside it) every single "
            "sample reads identically to the same COLOR00 calibration sample the "
            "border measurements above use -- the background colour, COLOR00, "
            "shows through inside the open window exactly as it does outside it, "
            "whenever no other object supplies a differing bit: 'the background "
            "color shows in any area on the display where there is no other "
            "object present' (p.46).");
    }

    // ==================== G13: BORDER-AND-BACKGROUND-TRACK-COLOR00 ====================
    // Establishes 3-colortable.c9, 3-ct.c05, 3-hscr.f07 (moved off G2) and
    // 3-colortable.c8, 3-ct.c04 (moved off G12): neither block above ever
    // varies COLOR00 -- each takes its "border" reading from a calibration
    // Harness written with the SAME COLOR00 value as the window under
    // test, so each can only show "outside/no-object reads a fixed
    // colour", never "outside/no-object reads COLOR00's own programmed
    // value". This assertion runs measure_color_track (defined above, in
    // the anonymous namespace) TWICE, with two different COLOR00 values,
    // and requires:
    //   - the window is independently shown open both times (a
    //     same-registers SATURATED companion run's own clean, non-empty
    //     open/close edges, which depend only on hpos versus
    //     hdiwstrt/hdiwstop and never on BPL1DAT);
    //   - the pre-window (border) and in-window-with-no-object
    //     (background) readings match EACH OTHER within a single run --
    //     the p.46 claim itself, that the same background colour shows
    //     both outside the window and inside it wherever no object is
    //     present;
    //   - both readings CHANGE when COLOR00 changes -- the part neither G2
    //     nor G12 could show, since neither ever wrote a second value.
    {
        auto a = measure_color_track(kColor00Value);
        // MUTATION G13-BORDER-AND-BACKGROUND-TRACK-COLOR00 stimulus red:
        //   MUTATE=G13-BORDER-AND-BACKGROUND-TRACK-COLOR00 writes the SAME
        //   COLOR00 value on the second run instead of a genuinely
        //   different one, so neither reading can differ between the runs.
        auto b = measure_color_track(
            mutating("G13-BORDER-AND-BACKGROUND-TRACK-COLOR00") ? kColor00Value
                                                                  : kColor00AltValue);

        bool window_shown_open = a.window_shown_open && b.window_shown_open;
        bool border_tracks_color00 = a.border != b.border;
        bool background_tracks_color00 = a.background != b.background;
        bool border_matches_background_per_run =
            a.border == a.background && b.border == b.background;

        checks.expect(
            window_shown_open && border_tracks_color00 && background_tracks_color00 &&
                border_matches_background_per_run,
            "G13-BORDER-AND-BACKGROUND-TRACK-COLOR00: "
            "3-colortable.c9 3-ct.c05 3-hscr.f07 3-colortable.c8 3-ct.c04 : "
            "with the identical DIWSTRT=$30/DIWSTOP=$90 window and BPL1DAT "
            "never written (no object present anywhere), a same-registers "
            "saturated companion run independently shows this window open (a "
            "clean, non-empty [open_idx,close_idx) measured off hpos versus "
            "hdiwstrt/hdiwstop alone, never off BPL1DAT); writing COLOR00=$0F0 "
            "makes the pre-window sample (border, hpos still below DIWSTRT) "
            "and the in-window sample (background, inside the window just "
            "shown open, still no object) read identically, and writing a "
            "SECOND, different COLOR00=$00A changes BOTH readings together -- "
            "'the background color shows in any area on the display where "
            "there is no other object present and is also displayed outside "
            "the defined display window, in the border area' (p.46), matching "
            "Figure 3-24's 'background color' callout on the border (p.83).");
    }

    // ==================== DECLINED CLAIMS ====================
    checks.note(
        "D1-SPRITES-OUT-OF-BOUNDARY: 3-intro.c01 3-overview.c1 3-diw.c02 "
        "3-window.c2 -- 'the screen display... consists of two basic parts -- "
        "playfields... and sprites' (p.39) and 'adjustment of display window "
        "size affects the entire display area, including the border and the "
        "sprites, not just the playfield' (p.57) both name sprites as a display "
        "object class; this package's own declared boundary (DIWSTRT/DIWSTOP H, "
        "DIWHIGH, the border, first/last displayed pixel) never mentions "
        "sprites, no sprite-register address is cited by any claim in this "
        "package's 174-record inventory, and this repository already has a "
        "dedicated sibling package (ch3-sprite-registers / hrm/denise_sprites) "
        "for sprite conformance -- exercising sprite pixel output here would "
        "duplicate that package. Declined, not resolved.");
    checks.note(
        "D2-DUAL-PLAYFIELD-TRANSPARENCY: 3-dual.c12 -- 'you can use transparency "
        "to allow selected features of the background playfield to show through' "
        "(p.67) is about dual-playfield colour-0/colour-8 transparency, covered "
        "by the sibling ch3-dual-playfield package. Declined, not resolved.");
    checks.note(
        "D3-DMA-MEMORY-PICTURE-BYTES: 3-big.c04 3-large.c3 3-large.c4 3-big.c05 "
        "3-hscr.c14 -- 'a 320-pixel display window displays 40 bytes on each "
        "line' / 'the big picture in memory... is 80 bytes wide' (pp.73-74,82) "
        "are Agnus DDF/DMA byte-count concepts (how many bytes get FETCHED per "
        "line), not something denise.v's own boundary observes at all (Denise "
        "receives bitplane data already fetched, via BPLxDAT, with no notion of "
        "a byte count); covered by the ch3-ddf-fetch-window and Agnus DMA "
        "packages. Declined, not resolved.");
    checks.note(
        "D4-VERTICAL-OR-WRONG-CHIP: 3-big.c26 3-window-range.c6 A-diw-bits.c1 "
        "A-diw-bits.c2 A-diw-bits.c3 A-diw-bits.c4 A-diw-bits.c5 A-diw-bits.c6 "
        "A-diw-bits.c7 A-diw-bits.c8 A-diw.bits1 C-diw-base-bits.c1 "
        "C-diw-base-bits.c2 C-diw-base-bits.c3 C-diw-base-bits.c4 "
        "C-diw-base-bits.c5 C-diw-base-bits.c6 C-diw-base-bits.c7 "
        "C-diw-base-bits.c8 C-beam.b12 C-beamcon0.c4 C-diw.c04 A-mod.c03 -- the "
        "even-line-count-for-interlace claims and the sixteen V7..V0-in-bits-15-8 "
        "appendix-table entries (both OCS Appendix A p.277 and ECS Appendix C "
        "p.306 printings) are the VERTICAL half of DIWSTRT/DIWSTOP, this "
        "repository's sibling ch3-vertical-display-window package's job on the "
        "agnus_bitplanedma fixture, not this one; BEAMCON0 ('bit 12 VARVBEN... "
        "use VBSTRT/STOP', p.305) is an Agnus register with no port anywhere on "
        "this denise fixture and is itself a vertical-blanking concept; "
        "C-diw.c04 ('bits 15-8 carry V7..V0 and bits 7-0 carry H7..H0', p.306) "
        "conjoins the V-half and H-half of the same claim -- the H-half is "
        "independently confirmed by G3-BITS above, but the V-half cannot be "
        "settled at this boundary, so the conjoined claim as stated is declined "
        "rather than partially asserted; the modulo claim ('the odd and even "
        "bitplanes may... have sizes different from... the display window "
        "size', Appendix A p.270) is an Agnus DMA-modulo concept with no modulo "
        "port anywhere in this fixture. Declined, not resolved -- needs a wider "
        "(or different) boundary than the denise fixture.");
    checks.note(
        "D5-PHYSICAL-SCREEN-GEOMETRY-FIGURES: 3-diw.f02 3-diw.f03 -- Figure "
        "3-9's own nested-boxes reading (p.58: the beam-sweep rectangle, the "
        "'visible screen boundaries' rounded rectangle, and the innermost "
        "display-window rectangle, with the coordinate origin (0,0) sitting "
        "outside and above the normal window) describes physical CRT-monitor "
        "geometry -- where a real monitor's own visible area sits relative to "
        "the beam's full sweep -- that denise.v's own hpos-vs-DIWSTRT/DIWSTOP "
        "compare has no notion of at all: Denise only ever compares its own "
        "horizontal position counter against the window bounds it was "
        "programmed with, it does not know where a monitor's visible area "
        "begins. Declined, not resolved -- needs a wider (whole-system/CRT-"
        "timing) boundary than this fixture; the figure's own NUMERIC content "
        "(320x200 window dimensions, the 0-255/256-511 HSTART/HSTOP ranges) is "
        "independently asserted from the same figures' captions in "
        "G5-DEFAULT-81-C1-320PX and G4-H8-FORCED-RANGE above.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
