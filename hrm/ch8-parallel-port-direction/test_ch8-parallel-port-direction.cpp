// Test purpose: assert printed p.259's claim that the general-purpose
//   parallel interface is bi-directional -- at this fixture's boundary,
//   that means a data register this interface exposes must have a write
//   side a read side can observe, under a direction setting that says it
//   should (output) -- at the `ciaa` module boundary, compiled alone.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   page 259; committed specification 5eb4afb (SPECIFICATION.md, this
//   package).
// Requirement IDs: PROP-1, per SPECIFICATION.md's claim inventory.
//   Claims file: hrm-testpipeline/work/ch8-s4-build/ch8-parallel-port-direction.jsonl
//   (2 claims; both carried on PROP-1's single graded line -- same clause of
//   the same sentence, see SPECIFICATION.md's claim inventory).
//
// Order-of-work disclosure (carried forward from SPECIFICATION.md, restated
//   here because it bears on how this file's assertions were designed): the
//   task instructions that assigned this package, and the classify.evidence
//   field co-resident with extract.quote in both claim records, pre-exposed
//   ciaa.v RTL detail (line numbers 267, 270-279, 268, 282-288, 296, 114)
//   before SPECIFICATION.md was written. That document's PROP-1 wording and
//   testability argument nonetheless rest only on extract.quote/
//   classify.restatement and the p.259 text. The RTL facts below were
//   independently re-verified by this builder (Read, not re-typed from what
//   was pre-exposed) after SPECIFICATION.md's commit, as step 2.
//
// RTL read for step 2: ciaa.v (388 lines, pinned submodule commit
//   d6f7df957fad655bc07f6a381e5094a9edff0be0) in full, plus its four
//   instantiated submodules (cia_int.v, cia_timera.v, cia_timerb.v,
//   cia_timerd.v -- needed only for Verilator to elaborate the top module).
//   Also read, per the task instructions naming it as this package's
//   control pattern: hrm/ch8-ciaa-port-direction/Makefile and its full test
//   .cpp (Checks class, Harness/write_reg/read_reg/tick bus-timing protocol
//   for ciaa.v specifically, and the MUTATION-comment convention -- reused
//   below as verified RTL-interface plumbing and suite coding convention,
//   not as a source of this package's own assertions or expected values,
//   which come only from SPECIFICATION.md). Also read minimig.v's CIAA1 and
//   CIAB1 instantiation sites (:603-651) and its 2011-04-02 change-log
//   comment (:136), and, per the build brief and the task instructions,
//   _emulators/minimig-aga-mister/rtl/ciaa.v (the MiSTer-devel/
//   Minimig-AGA_MiSTer mirror this repo's `src/minimig-aga` tracks) for the
//   lineage check.
//
// RTL facts established by that reading (none of this shaped
//   SPECIFICATION.md, which was committed first):
//   - ciaa.v:130-150 (address decoder): rs==4'h1 -> prb (CIAAPRB select),
//     rs==4'h3 -> ddrb (CIAADDRB select), never by name/address -- same
//     rs[3:0]-driven scheme as port A's pra (rs==4'h0) / ddra (rs==4'h2).
//   - ciaa.v:267, `//reg [7:0] regportb;`, and :270-279 (the entire write
//     block for it) are commented out. There is no register in this design
//     that a write to CIAAPRB (prb) ever reaches -- confirmed by grep: the
//     only occurrences of `regportb` in the file are inside that comment
//     block (:267, :275, :277).
//   - ciaa.v:268 `reg [7:0] ddrportb;` is real; written at :282-288
//     (posedge clk, clk7_en-gated, reset-cleared -- same bus-timing shape as
//     ddrporta) and read back at :296 on a CIAADDRB read (`!wr && ddrb`).
//   - ciaa.v:291-299 (pb_out, PRB/DDRB's shared read mux): a CIAAPRB read
//     (`!wr && prb`, :293-294) returns `portb_in[7:0]` -- the raw,
//     unsynchronised external input pins -- UNCONDITIONALLY. `ddrportb` does
//     not appear anywhere in that branch. A CIAADDRB read (:295-296)
//     returns `ddrportb[7:0]` correctly. So: `ddrportb` is written and can
//     be read back faithfully at its own address, but it governs nothing
//     about what a CIAAPRB read returns -- direction is real and inert.
//   - ciaa.v:114 `wire [7:0] portb_out;` is declared and appears nowhere
//     else in the file (confirmed by grep: one occurrence, the declaration
//     itself) -- never assigned, never read, never connected to a module
//     output port (ciaa's own port list, :80-104, has `output [3:0]
//     porta_out` but no `portb_out` output port at all).
//   - Consequence, confirmed by direct measurement below: this interface
//     has no write side a read can ever observe. A write to CIAAPRB is
//     dropped (no register catches it, at any bit position); a read of
//     CIAAPRB always echoes the external pins regardless of what was
//     written and regardless of CIAADDRB. p.259's bi-directional claim
//     fails at every bit position, not some.
//   - minimig.v:607-630 (CIAA1, this interface's own instance): no
//     `.portb_out(...)` connection appears in its port map at all -- there
//     is nothing to connect, since `ciaa` has no such output port.
//     `.portb_in(...)` at :624 wires `{_joy4[0..3], _joy3[0..3]}` -- port B
//     is this tree's four-player joystick adapter, an input-only use that
//     never touches the write side this claim is about.
//   - minimig.v:632-651 (CIAB1, a *different* module, `ciab`, for CIA-B):
//     `.portb_out({_motor,_sel3,_sel2,_sel1,_sel0,side,direc,_step})` at
//     :650 is THIS instance's port, driving floppy control signals. It is
//     not `ciaa`'s dead `portb_out` wire and is not evidence about this
//     claim; not cited as such.
//   - minimig.v:136's change-log comment, "2011-04-02 - added functional
//     ciaa port b (parallel) register to let Unreal game work and some
//     trainer store data", is stale and contradicted by the tree measured
//     above: the register it describes does not exist in this commit.
//
// Lineage check (per the build brief: this package's top, `ciaa`, lives
//   under `src/minimig-aga`, the shared-lineage layer also present in
//   MiSTer-devel/Minimig-AGA_MiSTer -- check the upstream file and say what
//   was found). `_emulators/minimig-aga-mister/rtl/ciaa.v`, that project's
//   own mirror, has the identical defect at its own Port B section
//   (:311-334): `regportb` does not exist there EITHER -- not even as a
//   commented-out declaration, it is simply absent -- `ddrportb` is
//   declared/written/read-back the same way nanomig's tree does, and the
//   PRB read mux (:326-334) unconditionally returns `portb_in[7:0]` with
//   the comment "Read port pins", never consulting `ddrportb`. Upstream's
//   own comments name this outcome directly: ":312 // Port B - Parallel
//   port (simplified, mostly unused in Amiga)" and ":141 wire [7:0]
//   portb_out; // Port B output (unused)". This is not a defect NanoMig
//   introduced -- it is inherited, comment-acknowledged behaviour in the
//   Minimig-AGA_MiSTer line this tree derives from. A red on PROP-1 below
//   is therefore a statement about the Minimig line, not only about this
//   board.
//
// Every mutation below was actually built as a `-D` variant (WSL Verilator
// 5.052, `make MUT=<NAME> test`, each a full `rm -rf obj_dir` rebuild) and
// the PASS/FAIL lines quoted are what that run printed, not predictions.
//
// Baseline (no MUT): 2 passed, 1 failed -- FIXTURE-LIVE PASS,
//   opus:8-par.c1/sol:8-par.c1 PROP-1 FAIL, CONTROL-PA0-OUTPUT PASS.
//   Reproduced identically on a second from-scratch build after all three
//   mutation runs below, to rule out ordering/state leakage between builds.
//
// MUTATION FIXTURE-LIVE stimulus red: MUT_FIXLIVE repeats the first DDRB
//   word (0xA5) instead of writing a distinct second word (0x5A) -> FAIL
//   ("1 passed, 2 failed": FIXTURE-LIVE flips to FAIL, PROP-1 stays FAIL
//   unaffected, CONTROL-PA0-OUTPUT stays PASS unaffected). The two readings
//   can no longer be told apart. Reverted (this is the baseline default,
//   MUT_FIXLIVE=0).
// MUTATION opus:8-par.c1 stimulus green: PROP-1 is graded at baseline
//   against RS_PRB/RS_DDRB and is red there (regportb's write path is
//   entirely commented out, so a CIAAPRB read always echoes portb_in
//   regardless of what was written). MUT_PROP1_TARGET_A retargets the
//   IDENTICAL bit_output_follows_direction() comparator -- register-select
//   arguments only, same code, same bit 0 -- at RS_PRA/RS_DDRA instead ->
//   PASS ("3 passed, 0 failed": all three flip to/stay PASS). Substitution
//   proof: the comparator is not vacuously false, it returns true for the
//   port where the mechanism actually works, on the same fixture, same
//   protocol, same run shape as CONTROL-PA0-OUTPUT. Reverted
//   (MUT_PROP1_TARGET_A=0 is baseline).
// MUTATION CONTROL-PA0-OUTPUT stimulus red: MUT_CONTROL_NODIR leaves PA0's
//   direction bit at its reset default (0, input) instead of writing it to
//   output, while the same PRA bit-0 write/read sequence runs unchanged ->
//   FAIL ("1 passed, 2 failed": CONTROL-PA0-OUTPUT flips to FAIL, PROP-1
//   stays FAIL unaffected, FIXTURE-LIVE stays PASS unaffected), proving the
//   control is not vacuously true either. Reverted (MUT_CONTROL_NODIR=0 is
//   baseline).

#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include <verilated.h>
#include "Vciaa.h"

#ifndef MUT_FIXLIVE
#define MUT_FIXLIVE 0
#endif
#ifndef MUT_PROP1_TARGET_A
#define MUT_PROP1_TARGET_A 0
#endif
#ifndef MUT_CONTROL_NODIR
#define MUT_CONTROL_NODIR 0
#endif

namespace {

constexpr uint8_t RS_PRA  = 0x0;
constexpr uint8_t RS_DDRA = 0x2;
constexpr uint8_t RS_PRB  = 0x1;
constexpr uint8_t RS_DDRB = 0x3;

class Checks {
public:
    void expect(bool condition, const std::string& label) {
        std::cout << (condition ? "PASS: " : "FAIL: ") << label << "\n";
        condition ? ++passed : ++failed;
    }
    void note(const std::string& msg) { std::cout << "MEASURED: " << msg << "\n"; }
    void record(const std::string& msg) { std::cout << "RECORDED: " << msg << "\n"; }
    int passed = 0;
    int failed = 0;
};

// Bus-timing protocol for ciaa.v, reused verbatim from
// hrm/ch8-ciaa-port-direction's own Harness (named by the task instructions
// as this package's control-pattern sibling, building this exact fixture):
// regporta/ddrporta/ddrportb are posedge-clk, clk7_en-gated, so a write
// needs one tick to latch; pa_out/pb_out/porta_out are combinational and
// need only eval().
class Harness {
public:
    Harness() : dut(new Vciaa) {
        dut->clk = 0;
        dut->clk7_en = 1;
        dut->clk7n_en = 1;
        dut->reset = 1;
        dut->aen = 0;
        dut->rd = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        dut->tick = 0;
        dut->eclk = 0;
        dut->porta_in = 0;
        dut->portb_in = 0;
        dut->kms_level = 0;
        dut->kbd_mouse_type = 0;
        dut->kbd_mouse_data = 0;
        dut->hrtmon_en = 0;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void write_reg(uint8_t rs, uint8_t data) {
        dut->aen = 1;
        dut->wr = 1;
        dut->rd = 0;
        dut->rs = rs;
        dut->data_in = data;
        tick();
        dut->aen = 0;
        dut->wr = 0;
        dut->rs = 0;
        dut->data_in = 0;
        tick();
    }

    uint8_t read_reg(uint8_t rs) {
        dut->aen = 1;
        dut->rd = 1;
        dut->wr = 0;
        dut->rs = rs;
        dut->eval();
        const uint8_t v = dut->data_out;
        dut->aen = 0;
        dut->rd = 0;
        dut->eval();
        return v;
    }

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    Vciaa* dut;
};

// ---------------------------------------------------------------------
// FIXTURE-LIVE: this exact build of `ciaa` must be shown able to deliver a
// known value before anything PROP-1 depends on is trusted. Uses CIAADDRB
// (the port-B side's own, genuinely working register) rather than CIAAPRB
// itself, so this precondition does not depend on the very mechanism
// PROP-1 grades.
// ---------------------------------------------------------------------
bool fixture_live(Harness& h, bool repeat_first_word_bug = false) {
    h.write_reg(RS_DDRB, 0xA5);
    const uint8_t r1 = h.read_reg(RS_DDRB);
    h.write_reg(RS_DDRB, repeat_first_word_bug ? 0xA5 : 0x5A);
    const uint8_t r2 = h.read_reg(RS_DDRB);
    return r1 == 0xA5 && r2 == 0x5A && r1 != r2;
}

// ---------------------------------------------------------------------
// Shared comparator. PROP-1 and CONTROL-PA0-OUTPUT call the SAME function,
// parameterised only by which data/direction register-select pair it
// targets -- the substitution proof for PROP-1's red result needs the
// identical comparator, not a reimplementation of it, pointed at a
// mechanism already proven live.
//
// With its own direction bit set to output, a data register's read-back
// must follow what was just written to it at that position, both 0->0 and
// 1->1. A read driven by anything held constant through both halves of this
// check (an external pin, a hard-wired value) is structurally unable to
// satisfy both -- it can equal 0 or equal the mask, never both in sequence
// -- so this comparator cannot pass by coincidence for a register that
// never captures a write, whatever fixed value the read happens to echo.
// ---------------------------------------------------------------------
bool bit_output_follows_direction(Harness& h, uint8_t rs_data, uint8_t rs_ddr,
                                   int bit, bool omit_direction_bug = false) {
    const uint8_t mask = static_cast<uint8_t>(1u << bit);
    h.write_reg(rs_ddr, omit_direction_bug ? uint8_t{0x00} : mask);
    h.write_reg(rs_data, 0x00);
    const bool tracks_zero = (h.read_reg(rs_data) & mask) == 0x00;
    h.write_reg(rs_data, mask);
    const bool tracks_one = (h.read_reg(rs_data) & mask) == mask;
    return tracks_zero && tracks_one;
}

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    {
        Harness h;
        checks.expect(fixture_live(h, MUT_FIXLIVE),
            "FIXTURE-LIVE: two distinct, known CIAADDRB bytes (0xA5, 0x5A) "
            "both arrive and read back correctly and distinctly before any "
            "data-dependent assertion is graded [no page -- infrastructure]");
    }

    {
        Harness h;
        const uint8_t rs_data = MUT_PROP1_TARGET_A ? RS_PRA : RS_PRB;
        const uint8_t rs_ddr  = MUT_PROP1_TARGET_A ? RS_DDRA : RS_DDRB;
        const bool result = bit_output_follows_direction(h, rs_data, rs_ddr, /*bit=*/0);
        checks.expect(result,
            "opus:8-par.c1 sol:8-par.c1 PROP-1: the general-purpose "
            "parallel interface is bi-directional -- with its direction "
            "bit set to output, a value written to its data register must "
            "be what a subsequent read of that register returns, both 0 "
            "and 1, at bit position 0. False: CIAAPRB's write path "
            "(regportb) is entirely commented out in this tree, so a "
            "CIAAPRB read always echoes the raw external portb_in pins, "
            "never what was written, at any bit position "
            "[p.259 \"The general-purpose bi-directional parallel "
            "interface is a 25-pin connector on the back panel of the "
            "computer\"]");
    }

    {
        Harness h;
        checks.expect(
            bit_output_follows_direction(h, RS_PRA, RS_DDRA, /*bit=*/0, MUT_CONTROL_NODIR),
            "CONTROL-PA0-OUTPUT: the identical bit_output_follows_direction() "
            "comparator PROP-1 evaluates above, pointed instead at CIA-A "
            "port A's own data/direction register pair (CIAAPRA/CIAADDRA) "
            "at bit 0 -- the same shape hrm/ch8-ciaa-port-direction already "
            "shows working there, on the same fixture and bus protocol. Not "
            "itself a page claim: it is PROP-1's measurement-basis control "
            "[no page -- infrastructure control]");
    }

    {
        // Supporting measurement, not itself graded: demonstrates the
        // second half of ciaa.v's finding -- that CIAADDRB, though real and
        // faithfully read back at its own address, governs nothing about
        // what a CIAAPRB read returns.
        Harness h;
        h.dut->portb_in = 0x3C;
        h.write_reg(RS_DDRB, 0x00);  // direction: all input
        h.write_reg(RS_PRB, 0x00);
        const uint8_t read_as_input = h.read_reg(RS_PRB);
        h.write_reg(RS_DDRB, 0xFF);  // direction: all output; PRB not rewritten
        const uint8_t read_as_output = h.read_reg(RS_PRB);
        char hexbuf[128];
        std::snprintf(hexbuf, sizeof(hexbuf), "0x%02X with CIAADDRB=0x00 (all "
            "input) and 0x%02X with CIAADDRB=0xFF (all output)",
            read_as_input, read_as_output);
        checks.note(
            std::string("Direction-independence: with portb_in fixed at ") +
            "0x3C, CIAAPRB reads " + hexbuf + ", same prior CIAAPRB write -- "
            "identical either way. CIAADDRB is real and reads back "
            "faithfully at its own address (ciaa.v:296) but governs nothing "
            "about a CIAAPRB read (ciaa.v:293-294).");
    }

    checks.record(
        "minimig.v:136's change-log comment (\"2011-04-02 - added "
        "functional ciaa port b (parallel) register to let Unreal game "
        "work and some trainer store data\") describes a register that "
        "does not exist in this commit -- regportb is entirely commented "
        "out (ciaa.v:267, :270-279). A stale comment in the tree, not "
        "evidence for or against PROP-1, and not itself a page claim; "
        "carries no uid.");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}
