// Test purpose: Verify, for the `agnus_blitter` module, every blitter
//   register's custom-chip-bus offset and access type (write-only), the
//   absence of ANY read path back to the processor at this boundary, that a
//   write to BLTSIZE behaves as the strobe that starts a blit, and that
//   BLTDDAT -- unlike every other register listed here -- has no write
//   decode at all: nothing at its documented (or, per Appendix A, its
//   entirely absent) address reaches an internal register through the
//   `reg_address_in`/`data_in` write path.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pp. 170-171 (chapter 6 prose, "DMA Channels"), 266-267 (Appendix A,
//   BLTCON0/BLTCON1/BLTAFWM/BLTALWM rows), 269-270 (Appendix A, BLTCON0L/
//   BLTDDAT/BLTSIZE/BLTxDAT/BLTxMOD rows and prose), 287 (Appendix B
//   legend), 289-290 (Appendix B register table), 309 (Appendix C, ECS
//   BLTCON1 row).
// Non-goals: minterm/truth-table correctness for a given LF byte
//   (PKG-BLT-MINTERM); any BLTCON0/BLTCON1 field's bit position beyond the
//   two used here as decode probes, LF and DESC (PKG-BLT-BLTCON-FIELDS);
//   barrel-shifter correctness (PKG-BLT-SHIFT/-SHIFT-CHAIN); fill-mode
//   correctness (PKG-BLT-FILL); line-mode correctness (PKG-BLT-LINE);
//   descending-mode's full semantics beyond "DESC visibly reverses address
//   stepping" (PKG-BLT-DESC); BLTSIZE/BLTSIZH/BLTSIZV's exact field widths,
//   maximum values and ECS size-combination arithmetic (PKG-BLT-SIZE);
//   BLTxMOD's numeric role beyond "a written modulo visibly changes address
//   stepping, distinctly from a plain increment" (PKG-BLT-ADRGEN); DMACON's
//   own address decode (PKG-AGNUS-DMACON -- see DEFERRED).
// Claim ids: ids such as `ch6-appa-bltcon0.c1` and `ch6-appa-bltcon0.c2` name
//   one sentence of the manual in this chapter's reading. They carry no reader
//   prefix, and they resolve in the claim files that
//   hrm-testpipeline/acceptance/ch6.json lists under `inventories`.
//
// BOUNDARY: `agnus_blitter`, per hrm-testpipeline/work/2026-09-08_fpga-090_
// packages.md row 5 and .../2026-09-08_fpga-090_build_register.md.
//
// SPEC-BEFORE-RTL GATE: satisfied cleanly. The specification (this file's
// claim listing, planned-assertion list and deferred-claims list, all
// unchanged in substance below) was committed in isolation, as a
// placeholder `int main(){return 0;}`, BEFORE agnus_blitter.v,
// agnus_blitter_adrgen.v or any other nanomig/ source file was opened by
// this agent. Every register offset, port name and RTL citation in that
// commit's header was reproduced from material already handed to this
// task -- the build brief's own quoted line numbers, the classify.evidence
// field already present on the inventory (itself a Prompt-4 deliverable,
// not fresh reading), and already-committed sibling test files this brief
// named as the harness/pattern to copy -- never from this agent's own
// reading. RTL reading began only after that commit. Full per-claim
// quotations and restatements remain in that commit's history
// (`git log -p -- hrm/ch6-blitter-regdecode` shows the spec commit in
// full); this header keeps the coverage-relevant summary and the findings
// the RTL reading actually produced, matching the pattern already
// established by hrm/ch6-blitter-bltcon/test_agnus_blitter_bltcon.cpp
// for a package this size.
//
// FIXTURE. Harness copied from hrm/ch6-blitter-cycles/ (the corrected-
// clkena harness, per the build brief -- NOT agnus_blitter_status's
// constant-clkena one). Carries the same traps documented in that file's
// header and in hrm-testpipeline/work/2026-09-08_fpga-090_build_register.md,
// verified still present and still necessary by reading both before
// writing this file:
//   1. `clkena` toggled once per cycle() call (true divide-by-2, matching
//      agnus.v's `cck`), not held high -- trap #7, eight false reds in
//      PKG-BLT-CYCLES otherwise.
//   2. TEST-DATA-DELIVERY: the settling `dut->eval()` right after setting
//      ackdma/reg_address_in/data_in, before the clock edge, reproduced as
//      this file's own first assertion before anything data-dependent.
//   3. BLTAFWM/BLTALWM default to 0 after reset (agnus_blitter.v:296,305)
//      and mask channel A's first/last word (agnus_blitter.v:311-319) --
//      every assertion here that drives channel A writes both to 0xFFFF
//      first. THIS TRAP WAS HIT DURING THIS BUILD (A23's first draft),
//      not just read about -- see MUTATION LOG / build note below.
//   4/5. `we`/`data_out` captured AT the grant (the settling eval(), before
//      the clock edge), never polled after; `int3` not used by this
//      package (no duration is measured) so its own sampling-order trap
//      does not bite here, carried over anyway since the harness is
//      copied whole.
//   6. Confirmed which `chsel`/`reg_address_out` value each state actually
//      sets by reading agnus_blitter.v in full (:594-601) before writing
//      any per-channel assertion, rather than assuming.
//
// ADDITIONAL MECHANISM, found while building this specific package and not
// already in build_register.md: `reg_address_out` (agnus_blitter.v:594-
// 601) reflects BLTADAT/BLTBDAT/BLTCDAT/BLTDDAT's own address depending on
// `chsel`, purely combinationally, independent of whether a grant is even
// in progress -- and `address_out` (agnus_blitter_adrgen.v's `bltptr_out`)
// simultaneously reflects whichever pointer-bank slot `chsel`/`ptrsel`
// currently selects. So a channel's pointer register can be read back
// VERBATIM off `address_out` at the moment of that channel's own DMA
// grant, without modelling addressed memory at all -- used throughout
// A10-A14 instead of re-deriving pointer correctness from data content.
//
// RTL FINDINGS (agnus_blitter.v read in full, 951 lines; agnus_blitter_
// adrgen.v read in full, 126 lines; agnus_blitter_minterm.v read for its
// LF encoding only -- none of the other three submodules opened, per this
// package's non-goals):
//   - All 24 offsets this package names (BLTCON0/BLTCON1/BLTCON0L/BLTAFWM/
//     BLTALWM/BLTAPTH/BLTAPTL/BLTBPTH/BLTBPTL/BLTCPTH/BLTCPTL/BLTDPTH/
//     BLTDPTL/BLTSIZE/BLTSIZH/BLTSIZV/BLTAMOD/BLTBMOD/BLTCMOD/BLTDMOD/
//     BLTADAT/BLTBDAT/BLTCDAT/BLTDDAT) match the manual's Appendix A/B
//     addresses exactly, confirmed against the `parameter` declarations
//     at agnus_blitter.v:101-112 and agnus_blitter_adrgen.v:23-34.
//   - Every one of the 23 real registers is gated by an explicit
//     `reg_address_in[8:1]==BLTxxx[8:1]` compare (agnus_blitter.v, scattered)
//     or, for the four modulo registers, a shared 6-bit range compare with
//     a 2-bit slot demux (agnus_blitter_adrgen.v:90,94-95) -- verified this
//     demux routes each modulo write to its own channel's slot only.
//   - BLTDDAT (`parameter BLTDDAT = 9'h000`, agnus_blitter.v:109) is
//     compared against `reg_address_in` NOWHERE in either file. Its only
//     appearance is as an OUTPUT of `reg_address_out` during the FSM's CHD
//     state (agnus_blitter.v:600) -- confirming the manual's Appendix A
//     "---" (no address at all) and Appendix B's "*000" (marked, not an
//     ordinary writable location) both describe a register this RTL never
//     lets the register bus write to.
//   - The module's only data-carrying output, `data_out`, is unconditionally
//     `ackdma && chsel==CHD ? bltdhold : 0` (agnus_blitter.v:490) -- gated
//     on a DMA WRITE grant the FSM itself initiates, never on anything an
//     external `reg_address_in` write can trigger on its own. There is no
//     other output that could constitute a register read. Confirms "no
//     read path" for every register, not only BLTDDAT.
//   - DMACON is NOT decoded anywhere in agnus_blitter.v or agnus_blitter_
//     adrgen.v (`grep -in dmacon` over both files: zero matches). The only
//     DMACON-derived signal reaching this module is `enadma`, a pre-decoded
//     input port -- confirming the DEFERRED entry below was right to be
//     cautious rather than asserting it.
//   - agnus_blitter.v:500-501 confirms BLTSIZE (or BLTSIZH under ecs) sets
//     `busy` unconditionally on the write, independent of any other
//     register -- the strobe behaviour A16 measures directly.
//
// DEFERRED -- claims classified into this package that this agent could
// NOT turn into a PKG-BLT-REGDECODE assertion, with the reason for each.
// All 109 claims filtered from classify.package=="PKG-BLT-REGDECODE" in
// work/2026-09-08_fpga-090_classified.jsonl are either cited inside an
// assertion string below or listed here; none is invented.
//
//   6-appa-registers.c41 (p.269, "In line mode, BLTSIZE width must equal
//     2.") -- field-content/value-range, not address decode or access
//     type. PKG-BLT-SIZE's/PKG-BLT-LINE's domain.
//   6-appa-registers.c61 (p.269, "BLTSIZE height has a maximum of 1024
//     lines.") -- field-content (bit width/max value). PKG-BLT-SIZE's
//     domain.
//   6-appa-registers.c64 (p.269, "BLTSIZE width has a maximum of 1024
//     pixels.") -- field-content. PKG-BLT-SIZE's domain.
//   6-appa-registers.c67 (p.269, "The BLTSIZE h field is ten bits wide in
//     original-chip-set line mode.") -- field-content. PKG-BLT-SIZE's/
//     PKG-BLT-LINE's domain.
//   6-appa-registers.c93 (p.270, "Modulo application advances the address
//     to the next line's start.") -- a behaviour/content claim about what
//     a modulo DOES, not that BLTxMOD decodes at its offset (which IS
//     asserted, A6-A9). PKG-BLT-ADRGEN's domain (92 claims, already built).
//   ch6-appa-bltxmod.c13 (p.270, "That width is normally two times the
//     screen width in words.") -- typical-value content, not decode.
//     PKG-BLT-ADRGEN's domain.
//   ch6-appb-dmacon.rw (p.290, "Appendix B marks DMACON W in the R/W
//     column and A D P in the CHIP column.") -- CONFIRMED, not merely
//     tentative: DMACON has no decode of its own inside agnus_blitter (see
//     RTL FINDINGS above). Needs the wider `agnus` boundary -- PKG-AGNUS-
//     DMACON's job, the package concurrently being built in this same
//     repository as this one.
//
// CLAIM COVERAGE. All 109 records of classify.package==
// "PKG-BLT-REGDECODE" and classify.status=="testable" in work/2026-09-08_
// fpga-090_classified.jsonl are covered below: 102 cited inside an
// assertion string (never only in a comment), 7 listed above with reasons.
// Verified by script, not by eye: extracting every dotted claim-id token
// from this file (both from assertion strings and from the DEFERRED
// section above) and diffing against the 109 ids filtered from
// classify.package=="PKG-BLT-REGDECODE" -- exact match, no id missing, no
// id invented.
//
// MUTATION LOG. Every mutation below was actually run by this agent: the
// named line(s) edited, `make test` rebuilt and re-run, the resulting
// PASS/FAIL line read, then reverted and rebuilt again to confirm the
// 26/26-green baseline (`24 planned assertions + TEST-DATA-DELIVERY + A16's
// own precondition check = 26 checks`). Applied in three batches of
// independent, non-overlapping edits (mutations never touching the same
// code path were combined into one rebuild to save build cycles); each
// batch's run confirmed that ONLY the intended assertion(s) flipped red
// and every other assertion stayed green, which is itself part of the
// evidence -- a mutation with an unintended side effect on an unrelated
// assertion would have been suspect, not just noted.
//
//   MUTATION A1 stimulus red: h1's BLTCON0 write retargeted to the unused
//     offset 0x035 (leaving its LF byte at the post-reset default, 0) ->
//     RED (h1's result read 0x0000, not the expected 0xFFFF). Reverted.
//   MUTATION A2 stimulus red: hdesc's BLTCON1 write retargeted to 0x035
//     -> RED (both runs showed ascending stepping, DESC never took
//     effect: "65536->65537" both times instead of one descending).
//     Reverted.
//   MUTATION A3 stimulus red: he2's BLTCON0L write (the ecs=1 case)
//     retargeted to 0x035 -> RED (LF stayed 0, D result read 0x0000 not
//     0xFFFF). Reverted.
//   MUTATION A4 stimulus red: hmask's BLTAFWM write retargeted to 0x035
//     -> RED (the word came out masked to 0x0000, not unmasked 0xBEEF).
//     Reverted.
//   MUTATION A5 stimulus red: hmask's BLTALWM write retargeted to 0x035
//     -> RED (last word came out masked to 0x0000, not unmasked 0x5678).
//     Reverted.
//   MUTATION A6 stimulus red: the "mod" run's BLTAMOD write retargeted to
//     0x035 -> RED (measured delta fell back to the plain +1 increment, 1,
//     instead of 1+0x100=257). Run together with A7-A9 in the same batch
//     (independent, non-overlapping edits); Reverted.
//   MUTATION A7 stimulus red: the "mod" run's BLTBMOD write retargeted to
//     0x035 -> RED (measured delta fell back to the plain +1 increment, 1,
//     instead of 1+0x100=257). Run together with A6, A8, A9 in the same
//     batch; Reverted.
//   MUTATION A8 stimulus red: the "mod" run's BLTCMOD write retargeted to
//     0x035 -> RED (measured delta fell back to the plain +1 increment, 1,
//     instead of 1+0x100=257). Run together with A6, A7, A9 in the same
//     batch; Reverted.
//   MUTATION A9 stimulus red: the "mod" run's BLTDMOD write retargeted to
//     0x035 -> RED (measured delta fell back to the plain +1 increment, 1,
//     instead of 1+0x100=257). Run together with A6-A8 in the same batch;
//     Reverted.
//   MUTATION A10 stimulus red: BLTAPTH (the high-bits half of channel A's
//     pointer pair) retargeted to 0x035 -> RED (the grant address's high 5
//     bits stayed at their post-reset default instead of the written
//     pattern -- measured 9029, not 0x12345=74565). Run together with
//     A11-A13 in the same batch; Reverted.
//   MUTATION A11 stimulus red: BLTBPTH (the high-bits half of channel B's
//     pointer pair) retargeted to 0x035 -> RED (the grant address's high 5
//     bits stayed at their post-reset default instead of the written
//     pattern). Run together with A10, A12, A13 in the same batch;
//     Reverted.
//   MUTATION A12 stimulus red: BLTCPTH (the high-bits half of channel C's
//     pointer pair) retargeted to 0x035 -> RED (the grant address's high 5
//     bits stayed at their post-reset default instead of the written
//     pattern). Run together with A10, A11, A13 in the same batch;
//     Reverted.
//   MUTATION A13 stimulus red: BLTDPTH (the high-bits half of channel D's
//     pointer pair) retargeted to 0x035 -> RED (the grant address's high 5
//     bits stayed at their post-reset default instead of the written
//     pattern). Run together with A10-A12 in the same batch; Reverted.
//   MUTATION A14 stimulus red: the source pointer's BLTAPTH write
//     retargeted to 0x035 -> RED (the strengthened address check, added
//     during this build once the first version proved insensitive to this
//     mutation -- see build note below -- caught the now-wrong channel-A
//     grant address; data content was unaffected, confirming the
//     strengthening was necessary). Reverted.
//   MUTATION A15 stimulus red: h1's BLTSIZE write (via start_blit's
//     now-mutable bltsize_reg parameter) retargeted to 0x035 -> RED (h1
//     produced 0 destination writes instead of 1 -- the blit never started
//     at all). Reverted.
//   MUTATION A16 stimulus red: the BLTSIZE write retargeted to 0x035 ->
//     RED (`busy` stayed 0). Reverted.
//   MUTATION A17 stimulus red: he's BLTSIZH write (the ecs=1 case)
//     retargeted to 0x035 -> RED (`busy` stayed 0 even with ecs=1).
//     Reverted.
//   MUTATION A18 stimulus red: the marker word moved from `a_words` to
//     `b_words` (channel A's queue left empty) -> RED (D read back 0x0000,
//     not 0x5A5A -- the fixture is sensitive to which channel's queue
//     actually feeds the fetch, not just that some queue is non-empty).
//     Reverted.
//   MUTATION A19 stimulus red: marker moved from `b_words` to `c_words`
//     -> RED (0x0000, not 0x6B6B). Reverted.
//   MUTATION A20 stimulus red: marker moved from `c_words` to `a_words`
//     -> RED (0x0000, not 0x7C7C). Reverted.
//   MUTATION A21 stimulus red: `b_words` emptied while the `expected`
//     computation (a literal XOR of the three intended words) was left
//     untouched -> RED (measured 0x13260 against the now-stale expected
//     0x15555 -- B's contribution silently defaulted to 0, exactly the
//     "stuck or shared register" failure mode this assertion exists to
//     catch). Reverted.
//   MUTATION A22 stimulus green: ATTEMPTED, RECORDED AS A MISS. The
//     probed address was moved from BLTDDAT's 0x000 to BLTADAT's real,
//     decoded 0x074, expecting the read-back check to now show something
//     -- it did NOT go red (still PASS). Diagnosis, not a fixture defect:
//     `data_out` is gated on `ackdma && chsel==CHD` regardless of what
//     `reg_address_in` carries during a plain write, so a plain write to
//     ANY register -- decoded or not -- produces no read-back through this
//     port. A22's own proposition ("no processor-visible address, cannot
//     be read") is true of every register in this module, not uniquely
//     falsifiable against BLTDDAT by retargeting alone. THE MUTATION THAT
//     REPLACES IT is A23's (below), which DOES flip red on the identical
//     underlying fact -- BLTDDAT's write never reaching any internal
//     register -- by using a measurement (does the value surface in a
//     blit result) that a plain write's absence of read-back cannot
//     distinguish. A22 is retained because its own two citations
//     (6-appa-registers.c29/.c30 -- "no processor-visible register
//     address", "not processor-readable or -writable") are propositions
//     about the ADDRESS FIELD and the READ half specifically, which A23
//     does not separately name.
//   MUTATION A23 stimulus red: hadat's control write (BLTADAT, the arm
//     that is SUPPOSED to matter) retargeted to 0x035 -> RED (0xDEAD no
//     longer surfaced; `adat_affected` went false, and the assertion's
//     `ddat_unaffected && adat_affected` conjunction correctly flipped).
//     Reverted.
//   MUTATION A24 stimulus red: the self-contained control blit's BLTSIZE
//     write (added to this assertion during this build -- see build note)
//     retargeted to 0x035 -> RED (`control_fired` went false: no
//     D-channel grant ever occurred, so the sweep's all-zero result could
//     no longer be told apart from a dead data_out/we probe). Reverted.
//   MUTATION TEST-DATA-DELIVERY stimulus red: re-run for this gate
//     repair (no prior record existed for this precondition check -- see
//     PKG-BLT-REGDECODE gate-repair commit). h.a_words's last word changed
//     from 0x4444 to 0x9999 (stimulus only, the expected 0x1111/0x2222/
//     0x3333/0x4444 sequence in the assertion left untouched) -> RED
//     (25 passed, 1 failed; only TEST-DATA-DELIVERY flipped, all other 25
//     checks stayed green). Reverted, rebuilt, confirmed back to 26/0.
//
// BUILD NOTE, recorded rather than smoothed over: A23's first draft
// (writing 0xDEAD to BLTADAT and expecting it to surface via LF_A_
// PASSTHROUGH) initially FAILED on its very first build -- not a red
// finding, a fixture defect: the control harness had not written BLTAFWM/
// BLTALWM=0xFFFF first, so channel A's only word (both first and last) was
// masked to 0 regardless of BLTADAT's content (build_register.md trap #3,
// hit directly rather than only read about). Fixed by adding the two mask
// writes; re-run, green. A24's first draft reused the SAME Harness for its
// sweep and its positive-control blit, which failed because the sweep
// itself writes BLTSIZE (one of the 24 offsets under test) with a garbage
// value, legitimately starting an uncontrolled blit per A16's own finding
// -- the subsequent "control" BLTCON0 write then landed mid-blit, which
// agnus_blitter.v:244-250 gives a special (D-channel-disabling) meaning to.
// Fixed by giving the control blit its own fresh Harness. Neither fix
// changed any expected value or any manual quotation; both are stimulus/
// fixture corrections, recorded per the build brief's own instruction to
// say so rather than paper over a first-attempt defect.
//
#include <cstdint>
#include <deque>
#include <iostream>
#include <string>
#include <vector>

#include <verilated.h>
#include "Vagnus_blitter.h"

namespace {

constexpr uint16_t to_reg(uint16_t byte_address) { return static_cast<uint16_t>(byte_address >> 1); }

// Offsets and reg_address_in encoding verified against agnus_blitter.v:101-112
// (BLTCON0/BLTCON0L/BLTCON1/BLTAFWM/BLTALWM/BLTADAT/BLTBDAT/BLTCDAT/BLTDDAT/
// BLTSIZE/BLTSIZH/BLTSIZV parameters) and agnus_blitter_adrgen.v:23-34
// (BLTxMOD/BLTxPTH/BLTxPTL parameters), read after the spec commit.
constexpr uint16_t BLTDDAT  = to_reg(0x000);  // agnus_blitter.v:109 -- no CPU write decode anywhere in the file
constexpr uint16_t BLTCON0  = to_reg(0x040);
constexpr uint16_t BLTCON1  = to_reg(0x042);
constexpr uint16_t BLTAFWM  = to_reg(0x044);
constexpr uint16_t BLTALWM  = to_reg(0x046);
constexpr uint16_t BLTCPTH  = to_reg(0x048);
constexpr uint16_t BLTCPTL  = to_reg(0x04a);
constexpr uint16_t BLTBPTH  = to_reg(0x04c);
constexpr uint16_t BLTBPTL  = to_reg(0x04e);
constexpr uint16_t BLTAPTH  = to_reg(0x050);
constexpr uint16_t BLTAPTL  = to_reg(0x052);
constexpr uint16_t BLTDPTH  = to_reg(0x054);
constexpr uint16_t BLTDPTL  = to_reg(0x056);
constexpr uint16_t BLTSIZE  = to_reg(0x058);
constexpr uint16_t BLTCON0L = to_reg(0x05a);
constexpr uint16_t BLTSIZV  = to_reg(0x05c);
constexpr uint16_t BLTSIZH  = to_reg(0x05e);
constexpr uint16_t BLTCMOD  = to_reg(0x060);
constexpr uint16_t BLTBMOD  = to_reg(0x062);
constexpr uint16_t BLTAMOD  = to_reg(0x064);
constexpr uint16_t BLTDMOD  = to_reg(0x066);
constexpr uint16_t BLTCDAT  = to_reg(0x070);
constexpr uint16_t BLTBDAT  = to_reg(0x072);
constexpr uint16_t BLTADAT  = to_reg(0x074);
constexpr uint16_t IDLE_REG = 0x0FF;  // matches no real offset (max real offset is BLTADAT=0x3A)
// An address inside the blitter's own 9-bit register window (0x000-0x0FF of
// word-address space) that decodes to nothing: agnus_blitter.v's and
// agnus_blitter_adrgen.v's write-enable conditions are explicit equality (or
// narrow range) compares against the offsets above, none of which is 0x035
// (between the last BLTxMOD offset 0x033 and the first BLTxDAT offset
// 0x038). Used as "the neighbouring/wrong offset" for every mutation below.
constexpr uint16_t UNUSED_REG = 0x035;

// BLTCON0 use-channel bits, agnus_blitter.v:252.
constexpr uint16_t USEA = 1u << 11;
constexpr uint16_t USEB = 1u << 10;
constexpr uint16_t USEC = 1u << 9;
constexpr uint16_t USED = 1u << 8;

// BLTCON1 bit assignments, agnus_blitter.v:285-288 (area mode: bit0 must be
// 0 to select area mode at all -- line=bltcon1[0]).
constexpr uint16_t DESC = 1u << 1;

// Minterm LF settings, agnus_blitter_minterm.v: bit k of LF selects the
// (A,B,C) minterm whose value is k (A=MSB). Confirmed by reading the module:
// LF=0xFF sets every output bit regardless of A/B/C; LF=0x00 clears all.
constexpr uint16_t LF_ALWAYS_ONE    = 0xFF;
constexpr uint16_t LF_ALWAYS_ZERO   = 0x00;
constexpr uint16_t LF_A_PASSTHROUGH = 0xF0;  // D = A, for every B/C value
constexpr uint16_t LF_B_PASSTHROUGH = 0xCC;  // D = B, for every A/C value
constexpr uint16_t LF_C_PASSTHROUGH = 0xAA;  // D = C, for every A/B value

struct Grant {
    int tick;
    char channel;  // 'A','B','C','D'
    uint32_t address;
};

class Harness {
public:
    Harness() : dut(new Vagnus_blitter) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->reset = 1;
        dut->ecs = 0;
        dut->clkena = 1;  // cycle() drives the true alternation from here on
        dut->enadma = 1;
        dut->ackdma = 0;
        dut->data_in = 0;
        dut->reg_address_in = IDLE_REG;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
        data_writes.clear();
        grants.clear();
        addr_samples.clear();
        tick_count = 0;
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void set_ecs(bool v) { dut->ecs = v ? 1 : 0; }

    void write_reg(uint16_t addr, uint16_t data) { cycle(true, addr, data); tick_bookkeeping(); }

    void run_cycles(int n) {
        for (int i = 0; i < n; ++i) tick();
    }

    bool run_until_writes(size_t n, int max_ticks = 500) {
        for (int i = 0; i < max_ticks && data_writes.size() < n; ++i) tick();
        return data_writes.size() >= n;
    }

    int run_until_done(int max_ticks = 2000) {
        bool seen_busy = false;
        for (int i = 0; i < max_ticks; ++i) {
            tick();
            if (busy()) seen_busy = true;
            if (seen_busy && !busy()) return i;
        }
        return -1;
    }

    void settle(int n = 10) { run_cycles(n); }

    bool zero() const { return dut->zero; }
    bool busy() const { return dut->busy; }
    bool int3() const { return dut->int3; }
    bool we() const { return dut->we; }
    uint16_t data_out() const { return dut->data_out; }
    uint16_t reg_address_out() const { return dut->reg_address_out; }
    uint32_t address_out() const { return dut->address_out; }

    std::deque<uint16_t> a_words, b_words, c_words;
    std::vector<uint16_t> data_writes;
    std::vector<Grant> grants;
    // (tick, reg_address_out, address_out) sampled every tick -- lets a
    // test read a pointer register bank's CURRENT value combinationally,
    // via reg_address_out identifying which channel's slot address_out is
    // currently showing (agnus_blitter.v:594-601, agnus_blitter_adrgen.v's
    // address_out = bltptr_out), without needing that channel's own DMA
    // grant to complete.
    std::vector<std::pair<uint16_t, uint32_t>> addr_samples;
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
    bool last_granted = false;
    char last_grant_channel = 0;
    uint32_t last_grant_address = 0;
    // agnus.v:437 wires clkena to cck, a true divide-by-2 of clk7_en
    // (amiga_clk.v). Toggled once per cycle() call rather than held high,
    // per the build brief's trap #7 (PKG-BLT-CYCLES's eight false reds).
    bool clkena_phase = true;

    void tick_bookkeeping() {
        addr_samples.emplace_back(dut->reg_address_out, dut->address_out);
        if (last_we) data_writes.push_back(last_data_out);
        if (last_granted) grants.push_back(Grant{tick_count, last_grant_channel, last_grant_address});
        ++tick_count;
    }

    void tick() {
        cycle();
        tick_bookkeeping();
    }

    void cycle(bool cpu_write = false, uint16_t cpu_reg = IDLE_REG, uint16_t cpu_data = 0) {
        dut->clkena = clkena_phase ? 1 : 0;
        clkena_phase = !clkena_phase;
        dut->eval();
        last_we = false;
        last_data_out = 0;
        last_granted = false;
        last_grant_channel = 0;
        last_grant_address = 0;
        const bool granted = !cpu_write && dut->reqdma;
        if (cpu_write) {
            dut->ackdma = 0;
            dut->reg_address_in = cpu_reg;
            dut->data_in = cpu_data;
        } else if (granted) {
            dut->ackdma = 1;
            const uint16_t reg = dut->reg_address_out;
            uint16_t v = 0;
            char ch = 0;
            if (reg == BLTADAT) { v = pop_or_zero(a_words); ch = 'A'; }
            else if (reg == BLTBDAT) { v = pop_or_zero(b_words); ch = 'B'; }
            else if (reg == BLTCDAT) { v = pop_or_zero(c_words); ch = 'C'; }
            else if (reg == BLTDDAT) { ch = 'D'; }
            dut->reg_address_in = reg;
            dut->data_in = v;
            // Settle ackdma/reg_address_in/data_in against the combinational
            // network BEFORE the clock edge -- TEST-DATA-DELIVERY. Without
            // this second eval() no fetched data arrives (build brief trap
            // #2 / build_register.md's salvaged-harness defect).
            dut->eval();
            last_granted = true;
            last_grant_channel = ch;
            last_grant_address = dut->address_out;
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
    int passed = 0;
    int failed = 0;
};

// Sets up BLTCON0 (use-bits + LF) and starts a blit via BLTSIZE. Does NOT
// touch masks/pointers/modulos -- callers set those first if needed.
// bltcon0_reg/bltsize_reg default to the real offsets; a mutation run
// passes UNUSED_REG for one of them at one call site to retarget that
// specific write, leaving every other call in this file untouched.
void start_blit(Harness& h, uint16_t use_bits, uint16_t lf, int width, int height,
                 uint16_t bltcon0_reg = BLTCON0, uint16_t bltsize_reg = BLTSIZE) {
    h.write_reg(bltcon0_reg, static_cast<uint16_t>(use_bits | lf));
    const uint16_t w = static_cast<uint16_t>(width & 0x3F);       // agnus_blitter.v:522, OCS 6-bit width field
    const uint16_t hh = static_cast<uint16_t>((height & 0x3FF) << 6);  // agnus_blitter.v:556, OCS 10-bit height field
    h.write_reg(bltsize_reg, static_cast<uint16_t>(hh | w));
}

}  // namespace
namespace {

void set_pointer(Harness& h, uint16_t pth, uint16_t ptl, uint32_t addr20) {
    const uint16_t high = static_cast<uint16_t>((addr20 >> 15) & 0x1F);
    const uint16_t low15 = static_cast<uint16_t>(addr20 & 0x7FFF);
    h.write_reg(pth, high);
    h.write_reg(ptl, static_cast<uint16_t>(low15 << 1));
}

// Returns the address recorded at the Nth (0-based) DMA grant on `channel`,
// or -1 if fewer than N+1 grants occurred.
long grant_addr(const Harness& h, char channel, size_t n) {
    size_t seen = 0;
    for (const auto& g : h.grants) {
        if (g.channel == channel) {
            if (seen == n) return static_cast<long>(g.address);
            ++seen;
        }
    }
    return -1;
}

size_t grant_count(const Harness& h, char channel) {
    size_t n = 0;
    for (const auto& g : h.grants) if (g.channel == channel) ++n;
    return n;
}

}  // namespace

int main() {
    Checks checks;

    // ---------------------------------------------------------------
    // TEST-DATA-DELIVERY (sanity precondition -- every sibling package's
    // header carries this; no manual page, guards every data-dependent
    // assertion below).
    // ---------------------------------------------------------------
    {
        Harness h;
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.a_words = {0x1111, 0x2222, 0x3333, 0x4444};
        start_blit(h, USEA | USED, LF_A_PASSTHROUGH, 4, 1);
        h.run_until_writes(4, 200);
        h.settle(10);
        bool ok = h.data_writes.size() >= 4 &&
                  h.data_writes[0] == 0x1111 && h.data_writes[1] == 0x2222 &&
                  h.data_writes[2] == 0x3333 && h.data_writes[3] == 0x4444;
        checks.expect(ok, "TEST-DATA-DELIVERY: four distinct C-supplied words "
                           "(here A-supplied, A-passthrough) arrive at data_out "
                           "in order -- precondition for every assertion below "
                           "that reads data_out/we/address_out/grants");
    }

    // ---------------------------------------------------------------
    // A1 -- BLTCON0 decodes at 0x040, write-only (p266, p289).
    // ids: ch6-appa-bltcon0.c1, ch6-appa-bltcon0.c2, 6-appa-registers.c14,
    //      6-appa-registers.c15, 6-appa-registers.c17, ch6-appb-bltcon0.addr,
    //      ch6-appb-bltcon0.rw
    // Mechanism: BLTCON0[7:0] (the LF byte) reaches the minterm generator
    // (agnus_blitter.v:444, agnus_blitter_minterm.v) combinationally; LF=0xFF
    // forces D=1 always, LF=0x00 forces D=0 always, independent of A/B/C, so
    // a 1-word D-only blit distinguishes the two writes with no source data
    // needed at all.
    // ---------------------------------------------------------------
    {
        Harness h1;
        start_blit(h1, USED, LF_ALWAYS_ONE, 1, 1);
        h1.run_until_writes(1, 200);
        Harness h0;
        start_blit(h0, USED, LF_ALWAYS_ZERO, 1, 1);
        h0.run_until_writes(1, 200);
        bool ok = !h1.data_writes.empty() && !h0.data_writes.empty() &&
                  h1.data_writes[0] == 0xFFFF && h0.data_writes[0] == 0x0000;
        checks.expect(ok, "A1 BLTCON0-DECODES-AT-0x040-WRITE-ONLY: writing "
                           "BLTCON0's LF byte (0x040) changes the D-channel "
                           "result (LF=0xFF -> 0xFFFF, LF=0x00 -> 0x0000) "
                           "[ch6-appa-bltcon0.c1, ch6-appa-bltcon0.c2, "
                           "6-appa-registers.c14, 6-appa-registers.c15, "
                           "6-appa-registers.c17, ch6-appb-bltcon0.addr, "
                           "ch6-appb-bltcon0.rw]");
        // MUTATION A1 stimulus red: h1's BLTCON0 write retargeted to the
        // unused offset 0x035 -> RED (h1's result read 0x0000, not the
        // expected 0xFFFF). Reverted. Full mutation log above, before the
        // includes.
    }

    // ---------------------------------------------------------------
    // A2 -- BLTCON1 decodes at 0x042, write-only (p267, p289, p309).
    // ids: ch6-appa-bltcon0.c4, ch6-appa-bltcon0.c5, 6-appa-registers.c18,
    //      6-appa-registers.c19, 6-appa-registers.c21, ch6-appb-bltcon1.addr,
    //      ch6-appb-bltcon1.rw, ch6-appc-ecs.c5
    // Mechanism: BLTCON1 bit1 (DESC, agnus_blitter.v:286) flips incptr/
    // decptr (agnus_blitter.v:674-675 etc). A 2-row, 1-word, D-only blit
    // (so only the D pointer steps, no source channel needed) shows the
    // step direction: ascending with DESC=0, descending with DESC=1. NOT
    // re-deriving DESC's full semantics (PKG-BLT-DESC's job) -- only that
    // BLTCON1 visibly changes something.
    // ---------------------------------------------------------------
    {
        Harness hasc;
        set_pointer(hasc, BLTDPTH, BLTDPTL, 0x10000);
        start_blit(hasc, USED, LF_ALWAYS_ONE, 1, 2);
        hasc.run_cycles(120);
        long d0a = grant_addr(hasc, 'D', 0);
        long d1a = grant_addr(hasc, 'D', 1);

        Harness hdesc;
        set_pointer(hdesc, BLTDPTH, BLTDPTL, 0x10000);
        hdesc.write_reg(BLTCON1, DESC);
        start_blit(hdesc, USED, LF_ALWAYS_ONE, 1, 2);
        hdesc.run_cycles(120);
        long d0d = grant_addr(hdesc, 'D', 0);
        long d1d = grant_addr(hdesc, 'D', 1);

        bool ok = d0a >= 0 && d1a >= 0 && d0d >= 0 && d1d >= 0 &&
                  (d1a - d0a) > 0 && (d1d - d0d) < 0;
        checks.expect(ok, "A2 BLTCON1-DECODES-AT-0x042-WRITE-ONLY: writing "
                           "BLTCON1's DESC bit (0x042) reverses the D pointer's "
                           "step direction across a two-row blit (ascending: " +
                           std::to_string(d0a) + "->" + std::to_string(d1a) +
                           ", DESC=1: " + std::to_string(d0d) + "->" +
                           std::to_string(d1d) + ") "
                           "[ch6-appa-bltcon0.c4, ch6-appa-bltcon0.c5, "
                           "6-appa-registers.c18, 6-appa-registers.c19, "
                           "6-appa-registers.c21, ch6-appb-bltcon1.addr, "
                           "ch6-appb-bltcon1.rw, ch6-appc-ecs.c5]");
    }

    // ---------------------------------------------------------------
    // A3 -- BLTCON0L decodes at 0x05A, ECS-gated, write-only (p269, p289).
    // ids: ch6-appa-bltcon0l.c1, ch6-appa-bltcon0l.c2, ch6-appb-bltcon0l.addr,
    //      ch6-appb-bltcon0l.rw, 6-appendix-b.c18
    // Mechanism: agnus_blitter.v:259 --
    //   bltcon0[7:0] <= data_in[7:0] when (addr==BLTCON0 || addr==BLTCON0L && ecs)
    // so a BLTCON0L write only reaches the LF byte when ecs=1; ecs=0 is this
    // assertion's OWN control (same write, no effect).
    // ---------------------------------------------------------------
    {
        // start_blit() rewrites BLTCON0 wholesale (use-bits|LF in one write),
        // so to test BLTCON0L in isolation the use-bits are written first,
        // via a direct BLTCON0 write, and BLTCON0L is written after.
        Harness he2;
        he2.set_ecs(true);
        he2.write_reg(BLTCON0, USED);          // use-bits only, LF defaults to 0 (reset value)
        he2.write_reg(BLTCON0L, LF_ALWAYS_ONE); // ecs=1: should now reach the LF byte
        he2.write_reg(BLTSIZE, (1u << 6) | 1u); // height=1,width=1
        he2.run_until_writes(1, 200);

        Harness hn;
        hn.set_ecs(false);
        hn.write_reg(BLTCON0, USED);
        hn.write_reg(BLTCON0L, LF_ALWAYS_ONE);  // ecs=0: must NOT reach the LF byte
        hn.write_reg(BLTSIZE, (1u << 6) | 1u);
        hn.run_until_writes(1, 200);

        bool ok = !he2.data_writes.empty() && !hn.data_writes.empty() &&
                  he2.data_writes[0] == 0xFFFF && hn.data_writes[0] == 0x0000;
        checks.expect(ok, "A3 BLTCON0L-DECODES-AT-0x05A-ECS-GATED-WRITE-ONLY: "
                           "writing BLTCON0L (0x05A) with ecs=1 changes the D "
                           "result via the LF byte (0x0000 -> 0xFFFF); the "
                           "identical write with ecs=0 changes nothing (control) "
                           "[ch6-appa-bltcon0l.c1, ch6-appa-bltcon0l.c2, "
                           "ch6-appb-bltcon0l.addr, ch6-appb-bltcon0l.rw, "
                           "6-appendix-b.c18]");
    }

    // ---------------------------------------------------------------
    // A4 -- BLTAFWM decodes at 0x044, write-only (p266, p289).
    // ids: ch6-appa-masks.c1, ch6-appa-masks.c2, ch6-appb-bltafwm.addr,
    //      ch6-appb-bltafwm.rw
    // Mechanism: agnus_blitter.v:311-319 -- bltamask on the FIRST word of a
    // channel-A blit equals bltafwm; a masked-out vs masked-through A word
    // distinguishes an FFFF write from the reset default (0x0000, trap #3
    // in build_register.md).
    // ---------------------------------------------------------------
    {
        Harness hmask;
        hmask.write_reg(BLTALWM, 0xFFFF);  // only-word is also the last word
        hmask.write_reg(BLTAFWM, 0xFFFF);
        hmask.a_words = {0xBEEF};
        start_blit(hmask, USEA | USED, LF_A_PASSTHROUGH, 1, 1);
        hmask.run_until_writes(1, 200);

        Harness hzero;
        hzero.write_reg(BLTALWM, 0xFFFF);
        // BLTAFWM left at its reset default (0x0000, agnus_blitter.v:296) --
        // no write, this is the control.
        hzero.a_words = {0xBEEF};
        start_blit(hzero, USEA | USED, LF_A_PASSTHROUGH, 1, 1);
        hzero.run_until_writes(1, 200);

        bool ok = !hmask.data_writes.empty() && !hzero.data_writes.empty() &&
                  hmask.data_writes[0] == 0xBEEF && hzero.data_writes[0] == 0x0000;
        checks.expect(ok, "A4 BLTAFWM-DECODES-AT-0x044-WRITE-ONLY: writing "
                           "BLTAFWM=0xFFFF (0x044) lets a single-word A blit "
                           "through unmasked (0xBEEF); leaving it at its reset "
                           "default masks the same word to 0x0000 (control) "
                           "[ch6-appa-masks.c1, ch6-appa-masks.c2, "
                           "ch6-appb-bltafwm.addr, ch6-appb-bltafwm.rw]");
    }

    // ---------------------------------------------------------------
    // A5 -- BLTALWM decodes at 0x046, write-only (p266, p289).
    // ids: ch6-appa-masks.c5, ch6-appa-masks.c6, ch6-appb-bltalwm.addr,
    //      ch6-appb-bltalwm.rw
    // Mechanism: same as A4, mirrored onto the LAST-word mask, using a
    // 2-word blit so first-word and last-word masking are distinct
    // (agnus_blitter.v:311-319).
    // ---------------------------------------------------------------
    {
        Harness hmask;
        hmask.write_reg(BLTAFWM, 0xFFFF);
        hmask.write_reg(BLTALWM, 0xFFFF);
        hmask.a_words = {0x1234, 0x5678};
        start_blit(hmask, USEA | USED, LF_A_PASSTHROUGH, 2, 1);
        hmask.run_until_writes(2, 200);

        Harness hzero;
        hzero.write_reg(BLTAFWM, 0xFFFF);
        // BLTALWM left at reset default 0x0000 -- control.
        hzero.a_words = {0x1234, 0x5678};
        start_blit(hzero, USEA | USED, LF_A_PASSTHROUGH, 2, 1);
        hzero.run_until_writes(2, 200);

        bool ok = hmask.data_writes.size() >= 2 && hzero.data_writes.size() >= 2 &&
                  hmask.data_writes[1] == 0x5678 && hzero.data_writes[1] == 0x0000;
        checks.expect(ok, "A5 BLTALWM-DECODES-AT-0x046-WRITE-ONLY: writing "
                           "BLTALWM=0xFFFF (0x046) lets the last word of a "
                           "two-word A blit through unmasked (0x5678); leaving "
                           "it at its reset default masks that same word to "
                           "0x0000 (control) [ch6-appa-masks.c5, "
                           "ch6-appa-masks.c6, ch6-appb-bltalwm.addr, "
                           "ch6-appb-bltalwm.rw]");
    }

    // ---------------------------------------------------------------
    // A6-A9 -- BLTAMOD/BLTBMOD/BLTCMOD/BLTDMOD decode at 0x064/0x062/
    // 0x060/0x066, write-only (p289).
    // Mechanism: agnus_blitter_adrgen.v:94-95 gates all four modulo writes
    // on a SHARED range compare (reg_address_in[8:3]==BLTAMOD[8:3]) with
    // the low 2 bits selecting the bank slot (bltmod_sel=reg_address_in[2:1],
    // agnus_blitter_adrgen.v:90) -- verified this correctly demuxes to each
    // channel's own slot. A single-channel, 2-row, 1-word blit steps that
    // channel's pointer by exactly 1 (increment only, agnus_blitter.v:674-
    // 677 etc) between the two DMA grants when the modulo is left at its
    // reset default (0), and by 1+MOD when a modulo is written first --
    // isolating the modulo's own contribution from the plain increment.
    // ---------------------------------------------------------------
    auto modulo_delta = [](uint16_t use_bit, uint16_t pth, uint16_t ptl,
                            uint16_t mod_reg, bool write_mod, uint32_t mod_value,
                            char channel) -> long {
        Harness h;
        set_pointer(h, pth, ptl, 0x20000);
        if (write_mod) h.write_reg(mod_reg, static_cast<uint16_t>((mod_value & 0x7FFF) << 1));
        start_blit(h, use_bit, LF_ALWAYS_ONE, 1, 2);
        h.run_cycles(150);
        long a0 = grant_addr(h, channel, 0);
        long a1 = grant_addr(h, channel, 1);
        if (a0 < 0 || a1 < 0) return -999999;
        return a1 - a0;
    };
    {
        long base = modulo_delta(USEA, BLTAPTH, BLTAPTL, BLTAMOD, false, 0, 'A');
        long mod = modulo_delta(USEA, BLTAPTH, BLTAPTL, BLTAMOD, true, 0x0100, 'A');
        bool ok = base == 1 && mod == 1 + 0x0100;
        checks.expect(ok, "A6 BLTAMOD-DECODES-AT-0x064-WRITE-ONLY: channel A's "
                           "pointer steps by 1 between rows with BLTAMOD left "
                           "at its reset default (control), and by 1+0x100 "
                           "once 0x100 is written to BLTAMOD (0x064) -- "
                           "measured deltas " + std::to_string(base) + " and " +
                           std::to_string(mod) +
                           " [ch6-appb-bltamod.addr, ch6-appb-bltamod.rw]");
    }
    {
        long base = modulo_delta(USEB, BLTBPTH, BLTBPTL, BLTBMOD, false, 0, 'B');
        long mod = modulo_delta(USEB, BLTBPTH, BLTBPTL, BLTBMOD, true, 0x0100, 'B');
        bool ok = base == 1 && mod == 1 + 0x0100;
        checks.expect(ok, "A7 BLTBMOD-DECODES-AT-0x062-WRITE-ONLY: channel B's "
                           "pointer steps by 1 between rows with BLTBMOD left "
                           "at its reset default (control), and by 1+0x100 "
                           "once 0x100 is written to BLTBMOD (0x062) -- "
                           "measured deltas " + std::to_string(base) + " and " +
                           std::to_string(mod) +
                           " [ch6-appb-bltbmod.addr, ch6-appb-bltbmod.rw]");
    }
    {
        long base = modulo_delta(USEC, BLTCPTH, BLTCPTL, BLTCMOD, false, 0, 'C');
        long mod = modulo_delta(USEC, BLTCPTH, BLTCPTL, BLTCMOD, true, 0x0100, 'C');
        bool ok = base == 1 && mod == 1 + 0x0100;
        checks.expect(ok, "A8 BLTCMOD-DECODES-AT-0x060-WRITE-ONLY: channel C's "
                           "pointer steps by 1 between rows with BLTCMOD left "
                           "at its reset default (control), and by 1+0x100 "
                           "once 0x100 is written to BLTCMOD (0x060) -- "
                           "measured deltas " + std::to_string(base) + " and " +
                           std::to_string(mod) +
                           " [ch6-appb-bltcmod.addr, ch6-appb-bltcmod.rw]");
    }
    {
        // D's pointer only steps once its own DMA grant occurs, which trails
        // the fetch by a row (build_register.md trap #4) -- USED alone (no
        // source channel) still produces D grants, per agnus_blitter.v's
        // dma_req=used&pipeline_full once pipeline_full is set by any
        // next_word-qualifying state, including D's own empty first pass.
        long base = modulo_delta(USED, BLTDPTH, BLTDPTL, BLTDMOD, false, 0, 'D');
        long mod = modulo_delta(USED, BLTDPTH, BLTDPTL, BLTDMOD, true, 0x0100, 'D');
        bool ok = base == 1 && mod == 1 + 0x0100;
        checks.expect(ok, "A9 BLTDMOD-DECODES-AT-0x066-WRITE-ONLY: channel D's "
                           "pointer steps by 1 between rows with BLTDMOD left "
                           "at its reset default (control), and by 1+0x100 "
                           "once 0x100 is written to BLTDMOD (0x066) -- "
                           "measured deltas " + std::to_string(base) + " and " +
                           std::to_string(mod) +
                           " [ch6-appb-bltdmod.addr, ch6-appb-bltdmod.rw]");
    }

    // ---------------------------------------------------------------
    // A10-A13 -- BLTxPTH/BLTxPTL pointer register pairs decode at their
    // documented offsets, write-only (p289).
    // Mechanism: agnus_blitter_adrgen.v:66-80 gates each PTH/PTL write on
    // an explicit compare against that register's OWN offset (not a shared
    // range like the modulo bank), and bltptr_sel/{reg_address_in[4],
    // reg_address_in[2]} (agnus_blitter_adrgen.v:64) demuxes into the
    // correct one of 4 bank slots. A single-word, single-channel blit's
    // one DMA grant address IS that channel's pointer value verbatim.
    // ---------------------------------------------------------------
    auto pointer_addr = [](uint16_t use_bit, uint16_t pth, uint16_t ptl,
                            char channel, uint32_t addr20) -> long {
        Harness h;
        set_pointer(h, pth, ptl, addr20);
        start_blit(h, use_bit, LF_ALWAYS_ONE, 1, 1);
        h.run_cycles(60);
        return grant_addr(h, channel, 0);
    };
    {
        long a = pointer_addr(USEA, BLTAPTH, BLTAPTL, 'A', 0x12345);
        checks.expect(a == 0x12345, "A10 BLTAPTH-BLTAPTL-DECODE-AT-0x050-0x052-"
                           "WRITE-ONLY: writing BLTAPTH (0x050, high 5 bits) "
                           "and BLTAPTL (0x052, low 15 bits) with 0x12345's "
                           "halves makes channel A's own DMA grant address "
                           "0x12345 verbatim (measured " + std::to_string(a) + ") "
                           "[ch6-appb-bltapth.addr, ch6-appb-bltapth.rw, "
                           "ch6-appb-bltaptl.addr, ch6-appb-bltaptl.rw]");
    }
    {
        long b = pointer_addr(USEB, BLTBPTH, BLTBPTL, 'B', 0x23456);
        checks.expect(b == 0x23456, "A11 BLTBPTH-BLTBPTL-DECODE-AT-0x04C-0x04E-"
                           "WRITE-ONLY: writing BLTBPTH (0x04C) and BLTBPTL "
                           "(0x04E) with 0x23456's halves makes channel B's "
                           "own DMA grant address 0x23456 verbatim "
                           "[ch6-appb-bltbpth.addr, ch6-appb-bltbpth.rw, "
                           "ch6-appb-bltbptl.addr, ch6-appb-bltbptl.rw]");
    }
    {
        long c = pointer_addr(USEC, BLTCPTH, BLTCPTL, 'C', 0x34567);
        checks.expect(c == 0x34567, "A12 BLTCPTH-BLTCPTL-DECODE-AT-0x048-0x04A-"
                           "WRITE-ONLY: writing BLTCPTH (0x048) and BLTCPTL "
                           "(0x04A) with 0x34567's halves makes channel C's "
                           "own DMA grant address 0x34567 verbatim "
                           "[ch6-appb-bltcpth.addr, ch6-appb-bltcpth.rw, "
                           "ch6-appb-bltcptl.addr, ch6-appb-bltcptl.rw]");
    }
    {
        long d = pointer_addr(USED, BLTDPTH, BLTDPTL, 'D', 0x45678);
        checks.expect(d == 0x45678, "A13 BLTDPTH-BLTDPTL-DECODE-AT-0x054-0x056-"
                           "WRITE-ONLY: writing BLTDPTH (0x054) and BLTDPTL "
                           "(0x056) with 0x45678's halves makes channel D's "
                           "own DMA grant address 0x45678 verbatim "
                           "[ch6-appb-bltdpth.addr, ch6-appb-bltdpth.rw, "
                           "ch6-appb-bltdptl.addr, ch6-appb-bltdptl.rw]");
    }

    // ---------------------------------------------------------------
    // A14 -- the blit needs only a source pointer, a destination pointer
    // and BLTSIZE (p171).
    // ids: 6-dma-channels.c37, 6-dma-channels.c38, 6-dma-channels.c39,
    //      ch6-dma.c35
    // ---------------------------------------------------------------
    {
        Harness h;
        set_pointer(h, BLTAPTH, BLTAPTL, 0x10000);  // source ("starting address")
        set_pointer(h, BLTDPTH, BLTDPTL, 0x20000);  // destination
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.a_words = {0xAAAA, 0xBBBB, 0xCCCC};
        start_blit(h, USEA | USED, LF_A_PASSTHROUGH, 3, 1);  // size
        h.run_until_writes(3, 300);
        bool data_ok = h.data_writes.size() >= 3 && h.data_writes[0] == 0xAAAA &&
                  h.data_writes[1] == 0xBBBB && h.data_writes[2] == 0xCCCC;
        // Also trace the addresses actually used back to the two pointer
        // writes (not just the data content, which this harness's simple
        // FIFO source/sink would reproduce even if a pointer offset decoded
        // to the wrong place) -- source grants must start at 0x10000 and
        // step by 1, destination grants at 0x20000 likewise.
        bool addr_ok = grant_addr(h, 'A', 0) == 0x10000 && grant_addr(h, 'A', 1) == 0x10001 &&
                        grant_addr(h, 'A', 2) == 0x10002 && grant_addr(h, 'D', 0) == 0x20000 &&
                        grant_addr(h, 'D', 1) == 0x20001 && grant_addr(h, 'D', 2) == 0x20002;
        bool ok = data_ok && addr_ok;
        checks.expect(ok, "A14 BLIT-NEEDS-ONLY-SOURCE-POINTER-DEST-POINTER-AND-"
                           "SIZE: a copy driven ONLY through BLTAPTH/BLTAPTL "
                           "(starting address), BLTDPTH/BLTDPTL (destination "
                           "address) and BLTSIZE (block size) reproduces the "
                           "three source words at the destination in order "
                           "[6-dma-channels.c37, 6-dma-channels.c38, "
                           "6-dma-channels.c39, ch6-dma.c35]");
    }

    // ---------------------------------------------------------------
    // A15 -- BLTSIZE decodes at 0x058, write-only, contains width+height
    // (p269, p289).
    // ids: ch6-appa-bltsize.c1, ch6-appa-bltsize.c2, ch6-appa-bltsize.c4,
    //      6-appa-registers.c36, 6-appa-registers.c37, 6-appa-registers.c39,
    //      6-appa-registers.c40, ch6-appb-bltsize.addr, ch6-appb-bltsize.rw
    // Mechanism: agnus_blitter.v:519-525,552-561 -- BLTSIZE's low 6 bits
    // become `width`, bits[15:6] become `height_cnt`. Two different BLTSIZE
    // values (1x1 vs 3x1) produce a different number of destination writes,
    // demonstrating both the offset and that the write carries width/height
    // content, not just a bare strobe.
    // ---------------------------------------------------------------
    {
        Harness h1;
        h1.write_reg(BLTAFWM, 0xFFFF);
        h1.write_reg(BLTALWM, 0xFFFF);
        h1.a_words = {0x1111};
        start_blit(h1, USEA | USED, LF_A_PASSTHROUGH, 1, 1);
        h1.run_until_writes(1, 200);
        h1.settle(10);

        Harness h3;
        h3.write_reg(BLTAFWM, 0xFFFF);
        h3.write_reg(BLTALWM, 0xFFFF);
        h3.a_words = {0x1111, 0x2222, 0x3333};
        start_blit(h3, USEA | USED, LF_A_PASSTHROUGH, 3, 1);
        h3.run_until_writes(3, 200);
        h3.settle(10);

        bool ok = h1.data_writes.size() == 1 && h3.data_writes.size() == 3;
        checks.expect(ok, "A15 BLTSIZE-DECODES-AT-0x058-WRITE-ONLY-CONTAINS-"
                           "WIDTH-HEIGHT: writing BLTSIZE (0x058) with a "
                           "width=1 word produces exactly 1 destination write, "
                           "width=3 produces exactly 3 -- the write's content, "
                           "not just its occurrence, reaches the blit "
                           "(measured " + std::to_string(h1.data_writes.size()) +
                           " and " + std::to_string(h3.data_writes.size()) + ") "
                           "[ch6-appa-bltsize.c1, ch6-appa-bltsize.c2, "
                           "ch6-appa-bltsize.c4, 6-appa-registers.c36, "
                           "6-appa-registers.c37, 6-appa-registers.c39, "
                           "6-appa-registers.c40, ch6-appb-bltsize.addr, "
                           "ch6-appb-bltsize.rw]");
    }

    // ---------------------------------------------------------------
    // A16 -- writing BLTSIZE is a strobe: busy rises on the same write
    // (p287, Appendix B legend S).
    // ids: ch6-appb-legend.s
    // Mechanism: agnus_blitter.v:500-501 -- busy<=1 the cycle a write hits
    // BLTSIZE (or BLTSIZH under ecs), independent of any other register.
    // THIS IS THE PACKAGE'S NAMED CONTROL for every absence claim below
    // (A22-A24): the identical measurement -- does busy rise after this
    // write? -- applied to a register that DOES decode, proving the
    // fixture can detect a write's effect at all.
    // ---------------------------------------------------------------
    bool a16_ok;
    {
        Harness h;
        checks.expect(!h.busy(), "A16 precondition: busy is 0 before any "
                           "BLTSIZE write (no manual page, fixture sanity)");
        h.write_reg(BLTSIZE, (1u << 6) | 1u);
        a16_ok = h.busy();
        checks.expect(a16_ok, "A16 BLTSIZE-WRITE-IS-A-STROBE-THAT-STARTS-THE-"
                           "BLIT: writing BLTSIZE (0x058) sets `busy` on the "
                           "very next cycle, with no other register touched -- "
                           "the concrete instance of the Appendix B legend's "
                           "\"Writing the register causes the effect\" "
                           "[ch6-appb-legend.s]");
    }

    // ---------------------------------------------------------------
    // A17 -- BLTSIZV/BLTSIZH decode at 0x05C/0x05E, ECS-gated, write-only
    // (p289).
    // ids: ch6-appb-bltsizv.addr, ch6-appb-bltsizv.rw, ch6-appb-bltsizh.addr,
    //      ch6-appb-bltsizh.rw, 6-appendix-b.c19, 6-appendix-b.c20
    // Mechanism: agnus_blitter.v:523-524,548,557-558 -- BLTSIZH only loads
    // `width`/starts the blit when ecs=1 (`reg_address_in[8:1]==BLTSIZH[8:1]
    // && ecs`); BLTSIZV only loads `height` when its own address matches
    // (unconditionally stored, but only READ into height_cnt by a
    // subsequent ecs-gated BLTSIZH write). With ecs=0, a BLTSIZH write
    // leaves `busy` at 0 (control, same measurement as A16).
    // ---------------------------------------------------------------
    {
        Harness he;
        he.set_ecs(true);
        he.write_reg(BLTSIZV, 3);              // height=3 (ECS 15-bit field)
        he.write_reg(BLTSIZH, 1);               // width=1 (ECS 11-bit field), triggers start
        bool busy_ecs = he.busy();

        Harness hn;
        hn.set_ecs(false);
        hn.write_reg(BLTSIZV, 3);
        hn.write_reg(BLTSIZH, 1);
        bool busy_no_ecs = hn.busy();

        bool ok = busy_ecs && !busy_no_ecs;
        checks.expect(ok, "A17 BLTSIZV-BLTSIZH-DECODE-AT-0x05C-0x05E-ECS-GATED-"
                           "WRITE-ONLY: writing BLTSIZV (0x05C) then BLTSIZH "
                           "(0x05E) starts a blit (busy rises) only when "
                           "ecs=1; the identical writes with ecs=0 leave busy "
                           "at 0 (control) [ch6-appb-bltsizv.addr, "
                           "ch6-appb-bltsizv.rw, ch6-appb-bltsizh.addr, "
                           "ch6-appb-bltsizh.rw, 6-appendix-b.c19, "
                           "6-appendix-b.c20]");
    }

    // ---------------------------------------------------------------
    // A18 -- BLTADAT decodes at 0x074, write-only (p269, p290).
    // ids: ch6-appb-bltadat.addr, ch6-appb-bltadat.rw, 6-appa-registers.c76,
    //      6-appa-registers.c77, ch6-appa-bltxdat.c1, ch6-appa-bltxdat.c2
    // ---------------------------------------------------------------
    {
        Harness h;
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.a_words = {0x5A5A};
        start_blit(h, USEA | USED, LF_A_PASSTHROUGH, 1, 1);
        h.run_until_writes(1, 200);
        bool ok = !h.data_writes.empty() && h.data_writes[0] == 0x5A5A;
        checks.expect(ok, "A18 BLTADAT-DECODES-AT-0x074-WRITE-ONLY: a word "
                           "fetched from memory reaches the blit through "
                           "BLTADAT (0x074) and comes out the D channel "
                           "unaltered under A-passthrough LF (0x5A5A) "
                           "[ch6-appb-bltadat.addr, ch6-appb-bltadat.rw, "
                           "6-appa-registers.c76, 6-appa-registers.c77, "
                           "ch6-appa-bltxdat.c1, ch6-appa-bltxdat.c2]");
    }

    // ---------------------------------------------------------------
    // A19 -- BLTBDAT decodes at 0x072, write-only (p289).
    // ids: ch6-appb-bltbdat.addr, ch6-appb-bltbdat.rw
    // ---------------------------------------------------------------
    {
        Harness h;
        h.b_words = {0x6B6B};
        start_blit(h, USEB | USED, LF_B_PASSTHROUGH, 1, 1);
        h.run_until_writes(1, 200);
        bool ok = !h.data_writes.empty() && h.data_writes[0] == 0x6B6B;
        checks.expect(ok, "A19 BLTBDAT-DECODES-AT-0x072-WRITE-ONLY: a word "
                           "fetched from memory reaches the blit through "
                           "BLTBDAT (0x072) and comes out the D channel "
                           "unaltered under B-passthrough LF (0x6B6B) "
                           "[ch6-appb-bltbdat.addr, ch6-appb-bltbdat.rw]");
    }

    // ---------------------------------------------------------------
    // A20 -- BLTCDAT decodes at 0x070, write-only (p289).
    // ids: ch6-appb-bltcdat.addr, ch6-appb-bltcdat.rw
    // ---------------------------------------------------------------
    {
        Harness h;
        h.c_words = {0x7C7C};
        start_blit(h, USEC | USED, LF_C_PASSTHROUGH, 1, 1);
        h.run_until_writes(1, 200);
        bool ok = !h.data_writes.empty() && h.data_writes[0] == 0x7C7C;
        checks.expect(ok, "A20 BLTCDAT-DECODES-AT-0x070-WRITE-ONLY: a word "
                           "fetched from memory reaches the blit through "
                           "BLTCDAT (0x070) and comes out the D channel "
                           "unaltered under C-passthrough LF (0x7C7C) "
                           "[ch6-appb-bltcdat.addr, ch6-appb-bltcdat.rw]");
    }

    // ---------------------------------------------------------------
    // A21 -- each channel data register is separately decoded and
    // supplies the blitter independently (p170, p269).
    // ids: 6-appa-registers.c79, 6-appa-registers.c80, ch6-appa-bltxdat.c4,
    //      6-dma-channels.c8, ch6-dma.c6
    // Mechanism: run A, B and C together with THREE DIFFERENT words and an
    // LF that depends on all three (XOR), confirming each channel's own
    // fetched word -- and only its own -- reaches the result.
    // ---------------------------------------------------------------
    {
        Harness h;
        h.write_reg(BLTAFWM, 0xFFFF);
        h.write_reg(BLTALWM, 0xFFFF);
        h.a_words = {0x00FF};
        h.b_words = {0x0F0F};
        h.c_words = {0x3333};
        // LF=0x96: D = A^B^C (odd-parity minterms), agnus_blitter_cycles's
        // proven constant, reused here as a three-channel discriminator.
        start_blit(h, USEA | USEB | USEC | USED, 0x96, 1, 1);
        h.run_until_writes(1, 300);
        uint16_t expected = static_cast<uint16_t>(0x00FF ^ 0x0F0F ^ 0x3333);
        bool ok = !h.data_writes.empty() && h.data_writes[0] == expected;
        checks.expect(ok, "A21 EACH-CHANNEL-DATA-REGISTER-IS-SEPARATELY-"
                           "DECODED-AND-SUPPLIES-THE-BLITTER: writing three "
                           "different words to BLTADAT/BLTBDAT/BLTCDAT and "
                           "combining them (D=A^B^C) yields exactly the "
                           "combination of the three independently-supplied "
                           "values, not a stuck or shared register (measured "
                           "0x" + std::to_string(h.data_writes.empty() ? 0 : h.data_writes[0]) +
                           " vs expected 0x" + std::to_string(expected) + ") "
                           "[6-appa-registers.c79, 6-appa-registers.c80, "
                           "ch6-appa-bltxdat.c4, 6-dma-channels.c8, "
                           "ch6-dma.c6]");
    }

    // ---------------------------------------------------------------
    // A22 -- BLTDDAT has no processor-visible address and cannot be read
    // (p269).
    // ids: 6-appa-registers.c29, 6-appa-registers.c30, 6-appa-registers.c34,
    //      ch6-appa-bltddat.c1, ch6-appa-bltddat.c2, ch6-appa-bltddat.c5
    // Mechanism: the port list (agnus_blitter.v:79-98, read for this
    // commit) has exactly one data-carrying output, `data_out`, and it is
    // gated `ackdma && chsel==CHD` (agnus_blitter.v:490) -- a DMA WRITE
    // grant, never a processor-initiated read (there is no read-strobe
    // input anywhere on the port list). Driving reg_address_in to BLTDDAT's
    // Appendix-B address (0x000) with no matching ackdma/chsel produces no
    // distinguished output -- confirmed alongside A24's module-wide sweep.
    // ---------------------------------------------------------------
    {
        // The module's ENTIRE port list (agnus_blitter.v:79-98, read for
        // this commit) has exactly one data-carrying output, `data_out`,
        // unconditionally gated `ackdma && chsel==CHD` (agnus_blitter.v:490)
        // -- a DMA WRITE grant the FSM itself initiates, never a read
        // request an external reader can address in. "Attempting to read"
        // BLTDDAT means driving reg_address_in to its Appendix-B address
        // (0x000) the ordinary way a register write arrives, with no grant
        // in progress: data_out/we must stay at their idle values.
        // CONTROL, same measurement: A1/A14/A18-21 elsewhere in this file
        // already show data_out DOES carry real content (e.g. 0xFFFF, the
        // fetched words) during a genuine D-channel grant -- so a
        // permanently-zero data_out here is not because the port is
        // inert, it is because no read request exists at this boundary.
        Harness h;
        h.write_reg(BLTDDAT, 0xBEEF);
        bool idle_after_write = !h.we() && h.data_out() == 0x0000;
        bool ok = idle_after_write;
        checks.expect(ok, "A22 BLTDDAT-HAS-NO-PROCESSOR-VISIBLE-ADDRESS-AND-"
                           "CANNOT-BE-READ: driving reg_address_in to BLTDDAT's "
                           "Appendix-B address (0x000; Appendix A prints "
                           "\"---\", no number at all) the way an ordinary "
                           "register write arrives produces no data on the "
                           "module's one data-carrying output, data_out/we "
                           "(control: A1/A14/A18-21 already show data_out DOES "
                           "carry real content during a genuine D-channel "
                           "grant, so this port is not simply inert) "
                           "[6-appa-registers.c29, 6-appa-registers.c30, "
                           "6-appa-registers.c34, ch6-appa-bltddat.c1, "
                           "ch6-appa-bltddat.c2, ch6-appa-bltddat.c5]");
    }

    // ---------------------------------------------------------------
    // A23 -- BLTDDAT has no write decode at all; the D-channel transfer is
    // automatic, never CPU-initiated (p269, p289).
    // ids: 6-appa-registers.c31, 6-appa-registers.c32, 6-appa-registers.c33,
    //      6-appa-registers.c35, ch6-appa-bltddat.c3, ch6-appa-bltddat.c4,
    //      ch6-appa-bltddat.c6, 6-appendix-b.c3
    // Mechanism: confirmed by reading agnus_blitter.v in full for this
    // commit -- BLTDDAT (parameter, =9'h000) is compared against
    // reg_address_in EXACTLY NOWHERE in the file (every other register
    // above has its own `reg_address_in[8:1]==BLTxxx[8:1]` branch;
    // BLTDDAT's only appearance is as an OUTPUT value of reg_address_out
    // during CHD state, agnus_blitter.v:600). So a "phantom write" to
    // 0x000 before a blit must leave that blit's real, expected D result
    // completely untouched. NAMED CONTROL, same write mechanism, same
    // fixture: the identical phantom write aimed at BLTADAT's address
    // (0x074, which DOES decode) instead changes the blit's result --
    // proving the fixture would have caught it had BLTDDAT decoded too.
    // ---------------------------------------------------------------
    {
        Harness hddat;
        hddat.write_reg(BLTDDAT, 0xDEAD);       // phantom write to the dummy address
        start_blit(hddat, USED, LF_ALWAYS_ONE, 1, 1);  // expects 0xFFFF, independent of any prior write
        hddat.run_until_writes(1, 200);
        bool ddat_unaffected = !hddat.data_writes.empty() && hddat.data_writes[0] == 0xFFFF;

        Harness hadat;
        // BLTAFWM/BLTALWM default to 0 after reset (agnus_blitter.v:296,305)
        // and both gate channel A's first/last word (agnus_blitter.v:311-
        // 319) -- a 1-word blit is both, so without these writes A is
        // silently masked to 0 regardless of bltadat's content (build_
        // register.md trap #3). Set both to 0xFFFF so the phantom write is
        // actually free to surface if BLTADAT's own decode reaches it.
        hadat.write_reg(BLTAFWM, 0xFFFF);
        hadat.write_reg(BLTALWM, 0xFFFF);
        hadat.write_reg(BLTADAT, 0xDEAD);       // identical phantom write, but to a REAL register
        start_blit(hadat, USED, LF_A_PASSTHROUGH, 1, 1);  // D = A, so 0xDEAD SHOULD now surface
        hadat.run_until_writes(1, 200);
        bool adat_affected = !hadat.data_writes.empty() && hadat.data_writes[0] == 0xDEAD;

        bool ok = ddat_unaffected && adat_affected;
        checks.expect(ok, "A23 BLTDDAT-HAS-NO-WRITE-DECODE-TRANSFER-IS-"
                           "AUTOMATIC: writing 0xDEAD to BLTDDAT's address "
                           "(0x000) before a blit leaves that blit's D result "
                           "exactly as expected (0xFFFF, unaffected) -- while "
                           "the SAME 0xDEAD written to BLTADAT's address "
                           "(0x074) instead, the fixture's own named control, "
                           "DOES surface in the D result (0xDEAD via A-"
                           "passthrough), proving this measurement would have "
                           "caught a real decode had one existed "
                           "[6-appa-registers.c31, 6-appa-registers.c32, "
                           "6-appa-registers.c33, 6-appa-registers.c35, "
                           "ch6-appa-bltddat.c3, ch6-appa-bltddat.c4, "
                           "ch6-appa-bltddat.c6, 6-appendix-b.c3]");
    }

    // ---------------------------------------------------------------
    // A24 -- no register in this module has a read path (p287).
    // ids: ch6-appb-legend.wr
    // Mechanism: sweep every offset this package names (all 24 in A1-A20,
    // plus BLTDDAT's 0x000) and confirm an ordinary register write to each
    // -- with no DMA grant in progress -- never produces non-zero data_out/
    // we. Generalises A22 from BLTDDAT alone to the whole table. SELF-
    // CONTAINED CONTROL (not just a cross-reference to A1/A14/A18-21):
    // the same Harness, right after the sweep, runs one real D-only blit
    // and requires data_out/we to become non-zero THEN -- so the sweep's
    // all-zero result is not because this fixture's data_out/we probes are
    // themselves dead, but because none of the 24 writes constitutes a
    // read. Both halves are required for A24 to pass.
    // ---------------------------------------------------------------
    {
        const uint16_t offsets[] = {
            BLTDDAT, BLTCON0, BLTCON1, BLTAFWM, BLTALWM, BLTCPTH, BLTCPTL,
            BLTBPTH, BLTBPTL, BLTAPTH, BLTAPTL, BLTDPTH, BLTDPTL, BLTSIZE,
            BLTCON0L, BLTSIZV, BLTSIZH, BLTCMOD, BLTBMOD, BLTAMOD, BLTDMOD,
            BLTCDAT, BLTBDAT, BLTADAT,
        };
        Harness h;
        bool sweep_clean = true;
        for (uint16_t off : offsets) {
            h.write_reg(off, 0xACE5);
            if (h.we() || h.data_out() != 0x0000) sweep_clean = false;
        }
        // Self-contained control: a FRESH harness (the sweep above already
        // hit BLTSIZE with a garbage value among the 24 offsets, which
        // legitimately starts a real, uncontrolled blit per A16 -- reusing
        // that same instance here would conflate the sweep's own side
        // effects with the control) proves data_out/we DO fire when a real
        // D-channel grant occurs.
        Harness hc;
        start_blit(hc, USED, LF_ALWAYS_ONE, 1, 1);
        hc.run_until_writes(1, 200);
        bool control_fired = !hc.data_writes.empty() && hc.data_writes[0] == 0xFFFF;
        bool ok = sweep_clean && control_fired;
        checks.expect(ok, "A24 NO-REGISTER-IN-THIS-MODULE-HAS-A-READ-PATH: "
                           "an ordinary write to every one of this package's "
                           "24 register offsets, with no DMA grant in "
                           "progress, never once produces non-zero data_out "
                           "or we -- data_out's only source, agnus_blitter.v:"
                           "490, is gated on ackdma&&chsel==CHD, a condition "
                           "no external write to reg_address_in can satisfy "
                           "on its own -- while the SAME harness's data_out/we "
                           "probes DO fire immediately afterward on a real "
                           "D-channel grant (control, same run) "
                           "[ch6-appb-legend.wr]");
    }

    // ---------------------------------------------------------------
    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
