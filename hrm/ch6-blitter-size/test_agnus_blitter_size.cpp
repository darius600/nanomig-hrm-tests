// Test purpose: Verify the OCS BLTSIZE register at the agnus_blitter
//   boundary (packages.md item 8): the six-bit width field and the ten-bit
//   height field, zero in either field meaning the field's maximum (64 words
//   / 1024 lines), width counted in words versus height counted in
//   lines/rows, one BLTSIZE write configuring both dimensions at once, and
//   busy rising on the BLTSIZE write.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 169-173, 186, 191, 192, 200, 269, 308.
// Non-goals: ECS BLTSIZH/BLTSIZV, their own field widths (11-bit/15-bit) and
//   their `ecs` gating - PKG-BLT-ECS's territory (hrm/ch6-blitter-ecs),
//   which explicitly redirects its own OCS-BLTSIZE-zero-means-1024 numeric
//   claim back here (see that file's "NOT ASSERTED - REDIRECTED",
//   6-appc-big-blits.c22) while keeping the ECS field-width claims for
//   itself; line mode's specific use of BLTSIZE (width forced to 2, height
//   reused as line length) beyond citing the same "writes BLTSIZE last" busy
//   fact - PKG-BLT-LINE's territory; modulo application and address pointer
//   stepping at a row boundary - PKG-BLT-ADRGEN's territory (this file
//   counts destination WRITES to show width/height's operational meaning,
//   never address arithmetic); BUSY-NO-DMA-NEEDED and the pre-Fat-Agnus busy
//   timing alternative, zero/BZERO, int3 pulse timing - already
//   PKG-BLT-STATUS's job (hrm/ch6-blitter-status); minterm/data
//   correctness - PKG-BLT-MINTERM's job, used here strictly as plumbing
//   (C-passthrough) to get a countable word through the pipeline; register
//   read-back / write-only completeness for BLTSIZE - PKG-BLT-REGDECODE's
//   job; the manual's own word-to-bit/pixel arithmetic (1 word = 16 bits) -
//   this RTL exposes only a word-count field (width) and a line-count field
//   (height), never a bit or pixel count, so converting either is the
//   programmer's own arithmetic, not observable at this boundary (see NOT
//   ASSERTED below).
// Claim ids: ids such as `6-intro.c19` and `ch6-intro.c19` name one sentence
//   of the manual in this chapter's reading. They carry no reader prefix, and
//   they resolve in the claim files that hrm-testpipeline/acceptance/ch6.json
//   lists under `inventories`.
//
// NO PRIOR ART IS SUPERSEDED. docs/evidence/blitter-prior-art.md was read in full
// before writing this file, per the build brief's explicit instruction to
// say so rather than assume it. Its "eight blitter tests" table lists
// barrelshifter, fill, minterm, modulo, ptr, line_octant, mask and zero -
// none of the eight writes BLTSIZE for anything but incidental plumbing to
// start a blit, none asserts a single fact about BLTSIZE's own field widths,
// its zero-wrap encoding, or the width/height unit distinction. This is a
// genuinely new directory with no predecessor to confirm, extend or
// supersede.
//
// A MANUAL SELF-INCONSISTENCY, FLAGGED RATHER THAN SILENTLY RESOLVED: p.200
// ("Blitter Key Points") states "width is in words and height is in
// pixels" (6-blitter-key-points.c5, ch6-keypoints.c6) - verified verbatim
// against docs/references/amiga-hrm-3rd/ch6/p200.txt line 14, "pixels." is
// genuinely what is printed there, not an OCR artefact. p.171 states the
// same field, in the same chapter, as "Blit height is in lines with a zero
// representing 1024 lines" (6-dma-channels.c51 and neighbours) and
// p.269/p.308 both give the field's own unit as "height=vertical lines".
// This conflict was not among the six recorded in packages.md's "Source
// conflicts, all six" section - it is an additional one, found while
// grounding this package's assertions against the actual printed pages
// (per this task's "erst pruefen, dann behaupten" requirement) rather than
// trusting the classifier's restatement alone. Per the playbook's own rule
// ("FLAG CONFLICTS ... record both and mark it. Do not resolve it."), both
// readings are cited under SIZE-WIDTH-DEFINES-ROW-HEIGHT-COUNTS-ROWS below,
// which tests the one thing both wordings are trying to describe - a
// per-row unit that is NOT a pixel count at this boundary, since the RTL
// exposes only a line/row counter for height (agnus_blitter.v:208,
// `height_cnt`), never a pixel count. This does not silently pick "lines"
// over "pixels"; it says why the RTL can only ever agree with one of them.
//
// CLAIM COVERAGE. All 118 records of classify.package=="PKG-BLT-SIZE" and
// classify.status=="testable" in work/2026-09-08_fpga-090_classified.jsonl
// are covered below, either by an assertion or under NOT ASSERTED with a
// reason - verified by script (a claim-id set built from the same filter,
// diffed against every id cited anywhere in this file: 118 cited, 0 missing,
// 0 extra, 0 duplicated) before this header was finalized, so the count is
// not a hand tally. Both readers (sol/opus) cover much of the same ground -
// the appendix-A (p.269) and appendix-C (p.308) BLTSIZE bit tables in
// particular are stated by both readers, one claim per bit, on both pages,
// so many ids below are the same proposition stated up to four times. Every
// id is written out in full, never a shared-prefix shorthand - two packages
// before this one had to redo their citations for exactly that shortcut.
// ADDED 2026-09-25 (unit backlog, hrm-testpipeline/work/2026-09-25_unit-
// backlog): four further claims, routed here by that lot's routing pass,
// from outside the original 118-claim PKG-BLT-SIZE filter above - three
// were classified into sibling packages (ch6-line.c43 was
// classify.package=="PKG-BLT-LINE"; 6-appc-big-blits.c22 and .c24 were
// classify.package=="PKG-BLT-ECS"), one (ch6-dma.c45) was already inside
// the original 118 but, until today, only declined in this header's prose
// and never printed on a runtime line - a gap the coverage-counting rule
// ("HOW COVERAGE IS COUNTED ... by searching the text of lines the test
// actually printed") does not allow a header comment to close. This
// addition did not re-run the original 118-claim diff script, since it
// does not touch anything that script already checked; what was checked
// for the four added ids specifically, directly: `grep` on this file
// before any edit confirmed none of the four appeared anywhere in it as an
// id (a genuine gap, not already-covered ground under a different name),
// and `grep` after confirms all four now appear inside an `expect()` or
// `record()` call's own string argument, not only in a comment - two
// (ch6-line.c43, 6-appc-big-blits.c22) inside BLTSIZE-WRITE-SETS-BUSY's and
// SIZE-ZERO-HEIGHT-WRAPS-TO-1024's own already-mutated assertions, two
// (ch6-dma.c45, 6-appc-big-blits.c24) inside a new RECORDED: line that
// asserts nothing - see "NOT ASSERTED - UNIT CONVERSION" below for why
// those two are declined rather than graded. The routing file's own `how`
// field for each (hrm-testpipeline/work/2026-09-25_unit-backlog/build/
// ch6-blitter-size.jsonl) is what led here in each case.
//
// Assertion name -> covered ids:
//
//   TEST-DATA-DELIVERY: fixture check, no claim id - reproduces
//     hrm/ch6-blitter-status's TEST-DATA-DELIVERY through this file's own
//     Harness before anything data-dependent is asserted, per the build
//     brief.
//
//   BLTSIZE-WRITE-SETS-BUSY [reused fact, control precondition - see below]
//     (pp.169, 186, 191, 192, 200, 269): 6-intro.c19, ch6-intro.c19,
//     6-blitter-key-points.c1, ch6-keypoints.c2, 6-blitter-done-flag.c1,
//     ch6-done.c1, 6-line-mode.c102, ch6-linesum.c27, 6-appa-registers.c43,
//     6-appa-registers.c66, ch6-appa-bltsize.c7, ch6-line.c43 .
//     (ch6-line.c43, added 2026-09-25: p.191 "register should not be
//     written until the very end, when all other [registers have been
//     filled]" is the same "write BLTSIZE last" fact already cited above
//     from five other pages - the busy-on-write mechanism this assertion
//     measures IS the reason for the instruction, not a separate fact.
//     Matches this file's own Non-goals note above about PKG-BLT-LINE's
//     citing "the same 'writes BLTSIZE last' busy fact" rather than
//     building its own.)
//
//   SIZE-SINGLE-WRITE-SETS-BOTH-FIELDS (pp.170-171): ch6-dma.c10,
//     6-dma-channels.c44, ch6-dma.c43 .
//
//   SIZE-WIDTH-FIELD-BITS-LITERAL (pp.171, 269, 308): 6-appc-big-blits.c15,
//     6-appc-big-blits.c16, 6-appc-big-blits.c17, 6-appc-big-blits.c18,
//     6-appc-big-blits.c19, 6-appc-big-blits.c20, ch6-appc-bltsize.bit00,
//     ch6-appc-bltsize.bit01, ch6-appc-bltsize.bit02, ch6-appc-bltsize.bit03,
//     ch6-appc-bltsize.bit04, ch6-appc-bltsize.bit05,
//     6-appa-registers.c54, 6-appa-registers.c55, 6-appa-registers.c56,
//     6-appa-registers.c57, 6-appa-registers.c58, 6-appa-registers.c59,
//     ch6-appa-bltsize.bit00, ch6-appa-bltsize.bit01, ch6-appa-bltsize.bit02,
//     ch6-appa-bltsize.bit03, ch6-appa-bltsize.bit04, ch6-appa-bltsize.bit05,
//     6-dma-channels.c47, ch6-dma.c47, 6-appc-big-blits.c23,
//     ch6-appc-bltsize.w, 6-appa-registers.c62, ch6-appa-bltsize.w1,
//     6-dma-channels.c71, ch6-dma.c72, 6-dma-channels.c45, ch6-dma.c44 .
//     (the last four tie p.173's "linear region" note and p.171's "1 to 64
//     words" range statement to this assertion's own one-hot-plus-
//     representative-value sweep - see the assertion's own comment for why)
//
//   SIZE-HEIGHT-FIELD-BITS-LITERAL (pp.171, 269, 308): 6-appc-big-blits.c5,
//     6-appc-big-blits.c6, 6-appc-big-blits.c7, 6-appc-big-blits.c8,
//     6-appc-big-blits.c9, 6-appc-big-blits.c10, 6-appc-big-blits.c11,
//     6-appc-big-blits.c12, 6-appc-big-blits.c13, 6-appc-big-blits.c14,
//     ch6-appc-bltsize.bit06, ch6-appc-bltsize.bit07, ch6-appc-bltsize.bit08,
//     ch6-appc-bltsize.bit09, ch6-appc-bltsize.bit10, ch6-appc-bltsize.bit11,
//     ch6-appc-bltsize.bit12, ch6-appc-bltsize.bit13, ch6-appc-bltsize.bit14,
//     ch6-appc-bltsize.bit15,
//     6-appa-registers.c44, 6-appa-registers.c45, 6-appa-registers.c46,
//     6-appa-registers.c47, 6-appa-registers.c48, 6-appa-registers.c49,
//     6-appa-registers.c50, 6-appa-registers.c51, 6-appa-registers.c52,
//     6-appa-registers.c53,
//     ch6-appa-bltsize.bit06, ch6-appa-bltsize.bit07, ch6-appa-bltsize.bit08,
//     ch6-appa-bltsize.bit09, ch6-appa-bltsize.bit10, ch6-appa-bltsize.bit11,
//     ch6-appa-bltsize.bit12, ch6-appa-bltsize.bit13, ch6-appa-bltsize.bit14,
//     ch6-appa-bltsize.bit15,
//     6-dma-channels.c50, ch6-dma.c50, 6-appc-big-blits.c21,
//     ch6-appc-bltsize.h, 6-appa-registers.c60, ch6-appa-bltsize.h,
//     ch6-appa-bltsize.ld2, 6-dma-channels.c70, ch6-dma.c71,
//     6-dma-channels.c46, ch6-dma.c46 .
//     (the last six tie p.173's "linear region" note, p.192's line-mode use
//     of the h field as line length, and p.171's "1 to 1024 rows" range
//     statement to this assertion's own sweep)
//
//   SIZE-ZERO-WIDTH-WRAPS-TO-64 (p.171, p.269, p.308): 6-dma-channels.c48,
//     ch6-dma.c48, ch6-dma.c56, 6-appa-registers.c63, ch6-appa-bltsize.w2 .
//
//   SIZE-ZERO-HEIGHT-WRAPS-TO-1024 (pp.171, 308): 6-dma-channels.c51,
//     ch6-dma.c51, ch6-dma.c58b, 6-appc-big-blits.c22 .
//     (6-appc-big-blits.c22, added 2026-09-25: p.308's "h = Height (10 bit
//     height = 1024 lines  max)" is PKG-BLT-ECS's own package-boundary
//     redirect back to this file - see the Non-goals note above and
//     hrm/ch6-blitter-ecs's "NOT ASSERTED - REDIRECTED" entry for this same
//     id, which explains why: the OCS zero-means-1024 height encoding is
//     orthogonal to `ecs` and is exactly the maximum this assertion's own
//     wrap case reaches.)
//
//   SIZE-WIDTH-DEFINES-ROW-HEIGHT-COUNTS-ROWS (pp.171, 172, 200):
//     6-dma-channels.c49, ch6-dma.c49, ch6-dma.c55, ch6-dma.c57,
//     6-dma-channels.c60, ch6-dma.c62, 6-blitter-key-points.c4,
//     6-blitter-key-points.c5, ch6-keypoints.c5, ch6-keypoints.c6 .
//     (c5/c6 are the p.200 "height is in pixels" wording flagged above)
//
//   NOT-ASSERTED-UNIT-CONVERSION [recorded, not asserted - see below]
//     (pp.171, 308): ch6-dma.c45, 6-appc-big-blits.c24 . Printed as one
//     RECORDED: line, never a PASS/FAIL line - see "NOT ASSERTED - UNIT
//     CONVERSION" below for why both are declined.
//
// NOT ASSERTED - UNIT CONVERSION (absent_signal: a bit-count or pixel-count
//   port; reason_kind: absent - this RTL exposes only a word-count field
//   (width), never a bit count or a pixel count; the manual's own
//   word-to-bit and word-to-pixel arithmetic (x16) is the programmer's, not
//   observable here - the same treatment PKG-BLT-ECS's own sibling package
//   gives its own pixel-count claims (hrm/ch6-blitter-ecs's "NOT ASSERTED -
//   PIXEL/WORD UNIT CONVERSION", which independently declines this exact
//   page's width-in-pixels claim, below). Printed as one RECORDED: line at
//   runtime, naming both ids in full):
//   ch6-dma.c45 (p.171, "A width of 1 to 64 words is 16 to 1024 bits").
//   6-appc-big-blits.c24 (p.308, "w = Width  (6 bit width  = 1024 pixels
//     max)", restated "Original-chip-set BLTSIZE width has a 1024-pixel
//     maximum" - added 2026-09-25. The words half of the same field (1 to
//     64, reaching 64 only via the zero-wrap) is exactly what
//     SIZE-WIDTH-FIELD-BITS-LITERAL and SIZE-ZERO-WIDTH-WRAPS-TO-64 already
//     measure and is NOT what this id's own restatement states - its
//     restatement is the pixel figure alone, so per Prompt 7's rule ("an id
//     whose claim bundles an untested clause is withdrawn whole") it is
//     declined here rather than kept on either width assertion with the
//     pixel clause left silently uncounted.)
//
// FIXTURE. Started from the working harness in hrm/ch6-blitter-status/
// (the only agnus_blitter fixture in the tree proven to deliver data
// correctly) - TEST-DATA-DELIVERY below reproduces its TEST-DATA-DELIVERY
// assertion first, before anything data-dependent, per the build brief.
// Harness::cycle's DMA-grant handshake (the settling eval() before the
// clock edge, and capturing we/data_out AT the grant rather than polling
// afterwards) is the same mechanism as agnus_blitter_status, agnus_blitter_
// ecs, agnus_blitter_masks, agnus_blitter_shift_chain and agnus_blitter_
// line - all read before writing this file. `ecs` is tied to 0 for the
// whole file: every claim covered here is the OCS BLTSIZE encoding, which
// PKG-BLT-ECS's own BACKWARD-COMPAT-BLTSIZE-WORKS-WITH-ECS assertion
// already shows is unconditional on `ecs` anyway (agnus_blitter.v:500,
// 521-522, 555-556 have no `&& ecs` anywhere on the BLTSIZE decode), so
// nothing here needs to drive it high.
//
// Every write-counting assertion below (every one except
// BLTSIZE-WRITE-SETS-BUSY) is exactly the case the build brief calls out
// by name: busy/int3 signal completion BEFORE the pipeline has flushed the
// last word to memory (agnus_blitter.v:905, `done` fires while BLT_D/BLT_C
// is fetching/storing the SECOND-TO-LAST word, not the last one - the true
// last word is flushed one to two cycles later through BLT_E/BLT_F). Every
// count below is therefore taken only after run_until_done() PLUS
// Harness::settle()'s extra idle cycles, never immediately on busy's fall -
// this is a wrap/count claim, and the honest way to test it is to count
// what the blit actually does (the number of D writes), never to read a
// register back, so getting that count right down to the last flushed word
// matters more here than anywhere else in the file.
//
// Every mutation below was actually applied (edit -> rebuild -> run ->
// revert -> rebuild-clean, confirmed byte-identical to the baseline by
// `git diff --stat` after every revert) and the PASS/FAIL lines quoted are
// what the run printed, not predictions. Every mutation is a STIMULUS
// mutation to this test file's OWN inputs (a register value written to the
// DUT) - never to nanomig/, which this task forbids touching for any
// reason. Every mutation run's full total is quoted alongside the specific
// assertion(s) it moved, confirming nothing else in the file moved.
//
// MUTATION TEST-DATA-DELIVERY stimulus red: fed C word at index 2 changed
//   from 0x9ABC to 0x9ABD (expected constants left untouched) -> FAIL, the
//   third captured data_out no longer matches; totals 7 passed, 1 failed
//   (only this assertion).
// MUTATION BLTSIZE-WRITE-SETS-BUSY stimulus red: the register written to
//   start the blit changed from BLTSIZE to BLTCON1 (same data, same
//   timing) -> FAIL, busy never rises because BLTCON1's decode has no path
//   to the busy register (agnus_blitter.v:500-501 names only BLTSIZE and,
//   under ecs, BLTSIZH); totals 7 passed, 1 failed.
// MUTATION SIZE-SINGLE-WRITE-SETS-BOTH-FIELDS stimulus red: the height
//   value written changed from 2 to 3 (width stays 3, the expected count
//   in the assertion stays coded as the literal 6) -> FAIL, 9 destination
//   writes occur, not 6; totals 7 passed, 1 failed.
// MUTATION SIZE-WIDTH-FIELD-BITS-LITERAL stimulus red: the representative
//   width value 45 changed to 46 in the stimulus list (its paired expected
//   value in the same list left at 45) -> FAIL, that one probe now expects
//   45 writes and gets 46; every other probe in the same sweep still
//   passes, so the aggregate assertion fails; totals 7 passed, 1 failed.
// MUTATION SIZE-HEIGHT-FIELD-BITS-LITERAL stimulus red: the representative
//   height value 341 changed to 342 in the stimulus list (its paired
//   expected value left at 341) -> FAIL, that probe expects 341 writes and
//   gets 342; totals 7 passed, 1 failed.
// MUTATION SIZE-ZERO-WIDTH-WRAPS-TO-64 stimulus red: the width field
//   written changed from 0 to 1 (expected constant 64 left untouched) ->
//   FAIL, 1 destination write occurs, not 64 - the wrap-to-64 case is no
//   longer the case being driven at all; totals 7 passed, 1 failed.
// MUTATION SIZE-ZERO-HEIGHT-WRAPS-TO-1024 stimulus red: the height field
//   written changed from 0 to 1 (expected constant 1024 left untouched) ->
//   FAIL, 1 destination write occurs, not 1024; totals 7 passed, 1 failed.
// MUTATION SIZE-WIDTH-DEFINES-ROW-HEIGHT-COUNTS-ROWS stimulus red: the
//   height value written in the second (5-row) blit changed from 5 to 6
//   (width stays 6, the expected product stays coded as the literal 30) ->
//   FAIL, 36 destination writes occur, not 30; totals 7 passed, 1 failed.

#include <cstdint>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus_blitter.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

constexpr uint16_t BLTCON0  = to_reg(0x040);
constexpr uint16_t BLTCON1  = to_reg(0x042);
constexpr uint16_t BLTCDAT  = to_reg(0x070);
constexpr uint16_t BLTSIZE  = to_reg(0x058);
constexpr uint16_t IDLE_REG = 0x0FF;

// BLTCON0 use-channel bits, per agnus_blitter.v:252.
constexpr uint16_t USEC = 1u << 9;
constexpr uint16_t USED = 1u << 8;

// Minterm LF setting (agnus_blitter_minterm.v), used strictly as plumbing to
// get a countable word through the pipeline - minterm correctness itself is
// PKG-BLT-MINTERM's job.
constexpr uint16_t LF_C_PASSTHROUGH = 0xAA;  // D = C, for every A/B value

// BLTSIZE data word for OCS-style sizing: height in bits[15:6], width in
// bits[5:0] (agnus_blitter.v:521-525, 555-556).
constexpr uint16_t bltsize_word(uint16_t width, uint16_t height) {
    return static_cast<uint16_t>((height << 6) | width);
}

// Copied in mechanism from hrm/ch6-blitter-status (the DMA-grant
// second-eval() fix and the at-the-grant we/data_out capture point), read
// in full before writing this file.
class Harness {
public:
    Harness() : dut(new Vagnus_blitter) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->ecs = 0;
        dut->clkena = 1;
        dut->enadma = 1;
        dut->ackdma = 0;
        dut->data_in = 0;
        dut->reg_address_in = IDLE_REG;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
        data_writes.clear();
        tick_count = 0;
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void write_reg(uint16_t addr, uint16_t data) { cycle(true, addr, data); tick_bookkeeping(); }

    void run_cycles(int n) {
        for (int i = 0; i < n; ++i) tick();
    }

    // Runs until busy falls from 1 to 0 (a completed blit), or max_ticks is
    // exhausted. Returns true if a completed fall was observed.
    bool run_until_done(int max_ticks) {
        bool seen_busy = false;
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            const bool busy_now = static_cast<bool>(dut->busy);
            if (busy_now) seen_busy = true;
            if (seen_busy && !busy_now) return true;
        }
        return false;
    }

    // busy/int3 signal completion BEFORE the pipeline has flushed the LAST
    // destination word (agnus_blitter.v:905 - see file header). Run a few
    // idle cycles past busy's fall before trusting data_writes.size().
    void settle(int n = 6) { run_cycles(n); }

    bool busy() const { return dut->busy; }

    std::deque<uint16_t> c_words;
    std::vector<uint16_t> data_writes;
    int tick_count = 0;

private:
    Vagnus_blitter* dut;

    static uint16_t pop_or_zero(std::deque<uint16_t>& q) {
        if (q.empty()) return 0;
        uint16_t v = q.front();
        q.pop_front();
        return v;
    }

    bool last_we = false;
    uint16_t last_data_out = 0;

    void tick_bookkeeping() {
        if (last_we) data_writes.push_back(last_data_out);
        ++tick_count;
    }

    void tick() {
        cycle();
        tick_bookkeeping();
    }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->eval();
        last_we = false;
        last_data_out = 0;
        const bool granted = !cpu_write && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0;
            dut->reg_address_in = cpu_reg;
            dut->data_in = cpu_data;
        } else if (granted) {
            dut->ackdma = 1;
            const uint16_t reg = dut->reg_address_out;
            uint16_t v = 0;
            if (reg == BLTCDAT) v = pop_or_zero(c_words);
            // BLTDDAT (channel D) is the blitter writing OUT to the bus;
            // there is nothing to feed back for it. Channels A and B are
            // never enabled in this file (USEC|USED only), so their data
            // registers are never fetched.
            dut->reg_address_in = reg;
            dut->data_in = v;
            // Settle ackdma/reg_address_in/data_in against the
            // combinational network BEFORE the clock edge - the step the
            // salvaged diag.cpp harness skipped (see
            // hrm/ch6-blitter-status's header). we/data_out are
            // transient and only valid here.
            dut->eval();
            if (dut->we) {
                last_we = true;
                last_data_out = dut->data_out;
            }
        } else {
            dut->ackdma = 0;
            dut->reg_address_in = IDLE_REG;
            dut->data_in = 0;
        }
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }
};

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg)   { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

void start_blit(Harness& h, uint16_t width, uint16_t height) {
    h.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
    h.write_reg(BLTCON1, 0x0000);
    h.write_reg(BLTSIZE, bltsize_word(width, height));
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ---------------- TEST-DATA-DELIVERY (fixture check, no page) ----------------
    {
        Harness h;
        h.c_words = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        start_blit(h, 4, 1);
        h.run_until_done(200);
        h.settle();

        const std::vector<uint16_t> expected = {0x1234, 0x5678, 0x9ABC, 0xDEF0};
        checks.expect(h.data_writes == expected,
            "[fixture check] TEST-DATA-DELIVERY: four distinct C words fed into the "
            "fixture arrive at data_out, in order, at the we cycles they were fetched "
            "on - proves the DMA grant handshake actually delivers data and that the "
            "trailing-flush settle margin is sufficient to see the true last word. "
            "Reproduced from hrm/ch6-blitter-status's TEST-DATA-DELIVERY first, per "
            "the build brief, before anything data-dependent.");
    }

    // ---------------- BLTSIZE-WRITE-SETS-BUSY [reused fact, control] ----------------
    {
        // The same fact hrm/ch6-blitter-status already established as
        // BUSY-SET (agnus_blitter.v:500-501: busy is set purely off
        // reg_address_in matching BLTSIZE). Re-verified here only as the
        // precondition every write-counting assertion below depends on - if
        // BLTSIZE did not start the blit, none of the counts below would
        // ever move off zero. This adds nothing beyond BUSY-SET on its own;
        // see the file header for why it is asserted here anyway
        // (packages.md item 8 explicitly lists "busy rising on the BLTSIZE
        // write" as part of this package).
        Harness h;
        h.write_reg(BLTCON0, USEC | USED | LF_C_PASSTHROUGH);
        h.write_reg(BLTCON1, 0x0000);
        h.write_reg(BLTSIZE, bltsize_word(1, 1));
        h.run_cycles(1);

        checks.expect(h.busy(),
            "BLTSIZE-WRITE-SETS-BUSY [reused from PKG-BLT-STATUS's BUSY-SET, control "
            "precondition]: busy reads 1 the cycle after BLTSIZE is written - 'A "
            "\"blit\" is ... performed by initializing the blitter registers ... and "
            "then starting the blitter by writing the BLTSIZE register' (p.169), "
            "'Write BLTSIZE last; writing this register starts the blit' (p.200), "
            "'When the BLTSIZE register is written the blit is started' (p.186), "
            "'You must set the BLTSIZE register last as it starts the blit' (line "
            "mode, p.192), 'Writing to this register will start the blitter' (p.269). "
            "agnus_blitter.v:500-501. 6-intro.c19, ch6-intro.c19, "
            "6-blitter-key-points.c1, ch6-keypoints.c2, 6-blitter-done-flag.c1, "
            "ch6-done.c1, 6-line-mode.c102, ch6-linesum.c27, 6-appa-registers.c43, "
            "6-appa-registers.c66, ch6-appa-bltsize.c7, ch6-line.c43 .");
    }

    // ---------------- SIZE-SINGLE-WRITE-SETS-BOTH-FIELDS (pp.170-171) ----------------
    {
        // One BLTSIZE write, width=3 and height=2 (distinct, neither a
        // power of the other), starts a blit that produces exactly
        // width*height = 6 destination writes. If a second size register
        // were needed (as the claim's negative - "there is no second size
        // register in OCS mode" - implies is NOT the case), this single
        // write could not by itself determine a 2-dimensional count.
        Harness h;
        start_blit(h, 3, 2);
        const bool done = h.run_until_done(300);
        h.settle();

        checks.expect(done && h.data_writes.size() == 6,
            "SIZE-SINGLE-WRITE-SETS-BOTH-FIELDS: a single BLTSIZE write encoding "
            "width=3 and height=2 produces exactly 6 destination writes - 'All four "
            "DMA channels use a single blit size register, called BLTSIZE, used for "
            "both the width and height' (p.171), 'All four share a single blit size "
            "register' (p.170). One BLTSIZE decode loads both the width backup and "
            "the height counter, agnus_blitter.v:519-525,553-561; there is no second "
            "size register anywhere in this file. ch6-dma.c10, 6-dma-channels.c44, "
            "ch6-dma.c43 .");
    }

    // ---------------- SIZE-WIDTH-FIELD-BITS-LITERAL (pp.171, 269, 308) ----------------
    {
        // One-hot walk over the six width bits (height held at 1, so each
        // count is exactly the width value with no row multiplication),
        // plus three representative multi-bit values (7, 45, 63) to catch
        // any cross-bit interaction a purely one-hot walk cannot - together
        // this is the honest way to test "the six-bit width field is a
        // literal, unscrambled slice of BLTSIZE" by counting what the blit
        // actually does, never by reading a register back.
        const std::vector<uint16_t> widths = {1, 2, 4, 8, 16, 32, 7, 45, 63};
        bool all_ok = true;
        for (uint16_t w : widths) {
            Harness h;
            start_blit(h, w, 1);
            const bool done = h.run_until_done(2000);
            h.settle();
            const bool ok = done && h.data_writes.size() == w;
            if (!ok) {
                std::cout << "  [SIZE-WIDTH-FIELD-BITS-LITERAL] width=" << w
                          << " got " << h.data_writes.size() << " writes\n";
            }
            all_ok = all_ok && ok;
        }

        checks.expect(all_ok,
            "SIZE-WIDTH-FIELD-BITS-LITERAL: for width values {1,2,4,8,16,32,7,45,63} "
            "(height fixed at 1, so the count is the width value alone), the number "
            "of destination writes equals the width value written, exactly, in every "
            "case - 'The width is stored in the least significant six bits of the "
            "BLTSIZE register' (p.171), the appendix A (p.269) and appendix C (p.308) "
            "bit tables' 'w5 w4 w3 w2 w1 w0' one-hot layout, 'w=width=horizontal "
            "pixels (6 bits=64 words...)' (p.269), 'w = Width (6 bit width = 1024 "
            "pixels max)' (p.308). Setting BLTSIZE's vertical count (height) to 1, as "
            "this sweep does throughout, is exactly the 'linear ... region' case of "
            "p.173. `width[10:0] <= {4'b0000, ~|data_in[5:0], data_in[5:0]}` "
            "agnus_blitter.v:522, a literal 6-bit slice. "
            "6-appc-big-blits.c15, 6-appc-big-blits.c16, 6-appc-big-blits.c17, "
            "6-appc-big-blits.c18, 6-appc-big-blits.c19, 6-appc-big-blits.c20, "
            "ch6-appc-bltsize.bit00, ch6-appc-bltsize.bit01, ch6-appc-bltsize.bit02, "
            "ch6-appc-bltsize.bit03, ch6-appc-bltsize.bit04, ch6-appc-bltsize.bit05, "
            "6-appa-registers.c54, 6-appa-registers.c55, 6-appa-registers.c56, "
            "6-appa-registers.c57, 6-appa-registers.c58, 6-appa-registers.c59, "
            "ch6-appa-bltsize.bit00, ch6-appa-bltsize.bit01, ch6-appa-bltsize.bit02, "
            "ch6-appa-bltsize.bit03, ch6-appa-bltsize.bit04, ch6-appa-bltsize.bit05, "
            "6-dma-channels.c47, ch6-dma.c47, 6-appc-big-blits.c23, "
            "ch6-appc-bltsize.w, 6-appa-registers.c62, ch6-appa-bltsize.w1, "
            "6-dma-channels.c71, ch6-dma.c72, 6-dma-channels.c45, ch6-dma.c44 .");
    }

    // ---------------- SIZE-HEIGHT-FIELD-BITS-LITERAL (pp.171, 269, 308) ----------------
    {
        // Same method, over the ten height bits (width held at 1, so each
        // count is exactly the height value - one destination write per
        // row), plus three representative multi-bit values (7, 341, 1023).
        const std::vector<uint16_t> heights = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 7, 341, 1023};
        bool all_ok = true;
        for (uint16_t ht : heights) {
            Harness h;
            start_blit(h, 1, ht);
            const bool done = h.run_until_done(20000);
            h.settle();
            const bool ok = done && h.data_writes.size() == ht;
            if (!ok) {
                std::cout << "  [SIZE-HEIGHT-FIELD-BITS-LITERAL] height=" << ht
                          << " got " << h.data_writes.size() << " writes\n";
            }
            all_ok = all_ok && ok;
        }

        checks.expect(all_ok,
            "SIZE-HEIGHT-FIELD-BITS-LITERAL: for height values "
            "{1,2,4,8,...,512,7,341,1023} (width fixed at 1, so the count is the "
            "height value alone, one destination write per row), the number of "
            "destination writes equals the height value written, exactly, in every "
            "case - 'The height is stored in the upper ten bits of the BLTSIZE "
            "register' (p.171), the appendix A (p.269) and appendix C (p.308) bit "
            "tables' 'h9 h8 ... h0' one-hot layout, 'h=height=vertical lines (10 "
            "bits=1024 lines max)' (p.269), 'h = Height (10 bit height = 1024 lines "
            "max)' (p.308). Setting BLTSIZE's horizontal count (width) to 1, as this "
            "sweep does throughout, is exactly the 'linear ... region' case of p.173, "
            "and is the same field line mode repurposes as line length ('BLTSIZE "
            "bits 15-6 = dx + 1', p.192; 'the h field controls the line length (10 "
            "bits gives lines up to 1024 dots long)', p.269). "
            "`height_cnt[14:0] <= {4'b0000, ~|data_in[15:6], data_in[15:6]}` "
            "agnus_blitter.v:556, a literal 10-bit slice. "
            "6-appc-big-blits.c5, 6-appc-big-blits.c6, 6-appc-big-blits.c7, "
            "6-appc-big-blits.c8, 6-appc-big-blits.c9, 6-appc-big-blits.c10, "
            "6-appc-big-blits.c11, 6-appc-big-blits.c12, 6-appc-big-blits.c13, "
            "6-appc-big-blits.c14, "
            "ch6-appc-bltsize.bit06, ch6-appc-bltsize.bit07, ch6-appc-bltsize.bit08, "
            "ch6-appc-bltsize.bit09, ch6-appc-bltsize.bit10, ch6-appc-bltsize.bit11, "
            "ch6-appc-bltsize.bit12, ch6-appc-bltsize.bit13, ch6-appc-bltsize.bit14, "
            "ch6-appc-bltsize.bit15, "
            "6-appa-registers.c44, 6-appa-registers.c45, 6-appa-registers.c46, "
            "6-appa-registers.c47, 6-appa-registers.c48, 6-appa-registers.c49, "
            "6-appa-registers.c50, 6-appa-registers.c51, 6-appa-registers.c52, "
            "6-appa-registers.c53, "
            "ch6-appa-bltsize.bit06, ch6-appa-bltsize.bit07, ch6-appa-bltsize.bit08, "
            "ch6-appa-bltsize.bit09, ch6-appa-bltsize.bit10, ch6-appa-bltsize.bit11, "
            "ch6-appa-bltsize.bit12, ch6-appa-bltsize.bit13, ch6-appa-bltsize.bit14, "
            "ch6-appa-bltsize.bit15, "
            "6-dma-channels.c50, ch6-dma.c50, 6-appc-big-blits.c21, "
            "ch6-appc-bltsize.h, 6-appa-registers.c60, ch6-appa-bltsize.h, "
            "ch6-appa-bltsize.ld2, 6-dma-channels.c70, ch6-dma.c71, "
            "6-dma-channels.c46, ch6-dma.c46 .");
    }

    // ---------------- SIZE-ZERO-WIDTH-WRAPS-TO-64 (pp.171, 269, 308) ----------------
    {
        Harness h;
        start_blit(h, 0, 1);  // width field written as 0
        const bool done = h.run_until_done(2000);
        h.settle();

        checks.expect(done && h.data_writes.size() == 64,
            "SIZE-ZERO-WIDTH-WRAPS-TO-64: writing a width field of 0 (height fixed "
            "at 1) produces exactly 64 destination writes, counted, not read back "
            "from a register - 'If a value of zero is stored, a width count of 64 "
            "words is used' (p.171), 'Blit width is in words with a zero "
            "representing 64 words' (p.171), 'w = Width (6 bit width = 1024 pixels "
            "max)' / 'width=1024 pixels max)' (pp.269,308, the maximum reachable "
            "only via this wrap, corroborating SIZE-WIDTH-FIELD-BITS-LITERAL's "
            "1-to-63 sweep for the other end of the range). "
            "`~|data_in[5:0]` agnus_blitter.v:522 turns a written zero width into "
            "64 (the seventh bit of the internal 11-bit `width` register is set "
            "only when all six written bits are zero). 6-dma-channels.c48, "
            "ch6-dma.c48, ch6-dma.c56, 6-appa-registers.c63, ch6-appa-bltsize.w2, "
            "6-appa-registers.c64 .");
    }

    // ------------- NOT-ASSERTED-UNIT-CONVERSION (recorded, not asserted) -------------
    {
        // Both claims restate the width field's 1-to-64-word range (the fact
        // SIZE-WIDTH-FIELD-BITS-LITERAL and SIZE-ZERO-WIDTH-WRAPS-TO-64 above
        // already measure, by counting destination writes) in a unit this
        // module has no port for: BLTSIZE's width field is decoded here as a
        // word count alone (agnus_blitter.v:522); there is no bit-count port
        // and no pixel-count port anywhere on agnus_blitter, so "16 to 1024
        // bits" and "1024 pixels max" are the manual's own x16 arithmetic,
        // not something this boundary can observe. See this file's header,
        // "NOT ASSERTED - UNIT CONVERSION", for the full reasoning, including
        // why 6-appc-big-blits.c24 is withdrawn whole rather than kept on a
        // width assertion with its pixel clause silently uncounted, and the
        // independent, pre-existing precedent in hrm/ch6-blitter-ecs's own
        // "NOT ASSERTED - PIXEL/WORD UNIT CONVERSION" section for the
        // identical page-308 pixel claim.
        checks.record(
            "NOT ASSERTED - UNIT CONVERSION: absent_signal: a bit-count or "
            "pixel-count port on agnus_blitter (none exists - only the "
            "word-count `width` field is decoded, agnus_blitter.v:522); "
            "reason_kind: absent. ch6-dma.c45 (p.171, restated \"A width of "
            "1 to 64 words is 16 to 1024 bits\") and 6-appc-big-blits.c24 "
            "(p.308, 'w = Width  (6 bit width  = 1024 pixels max)', restated "
            "\"Original-chip-set BLTSIZE width has a 1024-pixel maximum\") "
            "both state the width field's range in a unit this module "
            "cannot deliver a signal for - declined here rather than graded "
            "on SIZE-WIDTH-FIELD-BITS-LITERAL or SIZE-ZERO-WIDTH-WRAPS-TO-64, "
            "whose own words-only claims stay exactly as they were.");
    }

    // ---------------- SIZE-ZERO-HEIGHT-WRAPS-TO-1024 (pp.171, 308) ----------------
    {
        Harness h;
        start_blit(h, 1, 0);  // height field written as 0
        const bool done = h.run_until_done(20000);
        h.settle();

        checks.expect(done && h.data_writes.size() == 1024,
            "SIZE-ZERO-HEIGHT-WRAPS-TO-1024: writing a height field of 0 (width "
            "fixed at 1, so the count is rows) produces exactly 1024 destination "
            "writes, counted, not read back from a register - 'with zero "
            "representing a height of 1024 rows' (p.171), 'Blit height is in lines "
            "with a zero representing 1024 lines' (p.171), 'h=height=vertical lines "
            "(10 bits=1024 lines max)' (p.269, the maximum reachable only via this "
            "wrap, corroborating SIZE-HEIGHT-FIELD-BITS-LITERAL's 1-to-1023 sweep "
            "for the other end of the range). `~|data_in[15:6]` "
            "agnus_blitter.v:556 turns a written zero height into 1024. "
            "6-dma-channels.c51, ch6-dma.c51, ch6-dma.c58b, 6-appa-registers.c61, "
            "6-appc-big-blits.c22 .");
    }

    // ---------------- SIZE-WIDTH-DEFINES-ROW-HEIGHT-COUNTS-ROWS (pp.171,172,200) ----------------
    {
        // Blit 1: width=6, height=1 -> the row length alone.
        Harness h1;
        start_blit(h1, 6, 1);
        const bool done1 = h1.run_until_done(500);
        h1.settle();
        const size_t row_words = h1.data_writes.size();

        // Blit 2: SAME width=6, height=5 -> if height genuinely counts
        // repetitions of a width-sized row (rather than, say, replacing the
        // row length, or being ignored), the total must be exactly
        // row_words * 5, not row_words + 5 - 1 or any other combination.
        Harness h2;
        start_blit(h2, 6, 5);
        const bool done2 = h2.run_until_done(500);
        h2.settle();

        checks.expect(done1 && done2 && row_words == 6 && h2.data_writes.size() == 30,
            "SIZE-WIDTH-DEFINES-ROW-HEIGHT-COUNTS-ROWS: with width held at 6, "
            "raising height from 1 to 5 raises the destination write count from 6 "
            "to exactly 30 (6*5), never anything else - the row length is set by "
            "width alone and height counts whole repetitions of that row. '(A row "
            "is defined by the width stored in BLTSIZE.)' (p.172). 'This is the "
            "only parameter in the blitter that is given in words' (p.171, of "
            "width), 'Blit width is in words ... Blit height is in lines' (p.171), "
            "'width is in words and height is in ' + 'pixels' (p.200 - see the file "
            "header's note on this wording differing from p.171's 'lines' for the "
            "identical field; both describe the same per-row unit this assertion "
            "measures, which the RTL exposes only as a row/line count, never a "
            "pixel count). `last_word = width_cnt[10:0]==1`, and "
            "`width_cnt_rld = enable & next_word & last_word | init & enable` "
            "agnus_blitter.v:528,539 - the width counter, not the height counter, "
            "is what reloads at a row boundary. 6-dma-channels.c49, ch6-dma.c49, "
            "ch6-dma.c55, ch6-dma.c57, 6-dma-channels.c60, ch6-dma.c62, "
            "6-blitter-key-points.c4, 6-blitter-key-points.c5, ch6-keypoints.c5, "
            "ch6-keypoints.c6 .");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
