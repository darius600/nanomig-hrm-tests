// Test purpose: HRM conformance for ch1-cpu-options -- the two printed
// page-1 "Components of the Amiga" propositions whose classify.evidence
// names `sysctrl` -- at the `sysctrl` module boundary (src/misc/sysctrl.v),
// compiled alone, per SPECIFICATION.md.
//
// 4 claims total: 2 fully graded (sol:1-components.c7, opus:1-components.c7,
// both carrying CPU-OPTIONS-FOUR-DISTINCT-STATES's uid slot directly); 2
// partially graded (sol:1-components.c12, opus:1-components.c12 -- the
// expansion-RAM/8MB half is graded at RAM-EXPANSION-CEILING-8MB, the
// base-RAM-floor/256K half at RAM-BASE-FLOOR-BELOW-256K-ABSENT, and the
// base-RAM-ceiling/2MB half and the Amiga-3000/"over a gigabyte" clause are
// declined -- see the record() call and SPECIFICATION.md's "Duplicate-
// grading check"/PROP-2 sections). Per the build task's own rule ("a uid on
// a graded line counts as ASSERTED, whatever the words around it say"),
// neither graded RAM label below carries the c12 uids in its own text; the
// uids live on the record() line alone, which tells the whole c12 story.
//
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, 3rd edition, printed page
//   1; committed specification (SPECIFICATION.md, this package, commit
//   c481d19).
// Requirement IDs: CPU-OPTIONS-FOUR-DISTINCT-STATES (PROP-1),
//   RAM-EXPANSION-CEILING-8MB and RAM-BASE-FLOOR-BELOW-256K-ABSENT
//   (PROP-2, partial), per SPECIFICATION.md's claim inventory.
// Non-goals: anything outside the `sysctrl` fixture this package builds, and
//   any claim outside the 4 it cites. A green here means those assertions
//   held on this fixture at the pinned revision -- not that the module is
//   correct, and not that the chapter is covered.
// Claim ids: 4 ids below carry a reader prefix -- `sol:` and `opus:` are the
//   two models that read the printed pages independently, and an id names
//   one reader's reading of one sentence. They resolve in the chapter's
//   claim file in the sibling repository (hrm-testpipeline).
//   docs/legacy-names.md explains the vocabulary.
//
// Deviation from SPECIFICATION.md, disclosed rather than papered over:
//   the spec scoped PROP-2's gradable half as "a base-RAM ceiling of 2 MB
//   and an expansion-RAM ceiling of 8 MB". Opening sysctrl.v (step 2) found
//   the chipmem comment (sysctrl.v:240) itself internally inconsistent --
//   it tags both "1.5M" and "2M" with the same "(2)", and names no label
//   at all for the field's fourth reachable raw value -- so which raw code
//   the design intends as "2 MB" cannot be read off without guessing.
//   Rather than invent that requirement, the 2 MB chipmem ceiling is
//   declined below (record()), and grading instead falls on two facts that
//   turned out to be unambiguous once the RTL was open: the fastmem/8MB
//   ceiling (its own comment, sysctrl.v:244, has four distinct tags, no
//   duplication) and the chipmem/256K floor (sysctrl.v:240 names raw value
//   0 -- this field's numeric minimum, direct passthrough, no remap --
//   "512k" without ambiguity; the duplicate tag only affects the top two
//   entries, not the bottom one).
//
// Non-goal, per the build task's own chapter-wide cautions (neither claim
//   here makes a frequency claim, so this does not constrain either
//   assertion, and is noted only so a later reader does not look for a
//   clock-rate check in a CPU-options package): no absolute clock
//   frequency is gradeable in this chapter -- amiga_clk.v divides a clock
//   handed to it; the megahertz comes from a board oscillator outside both
//   UUT roots.
//
// Scope note: `sysctrl` lives in src/misc -- NanoMig's own substitute
//   configuration mechanism, not the shared chipset lineage (agnus/paula/
//   denise/gary). Every finding below is a finding about NanoMig's own
//   reimplementation of "which CPU/RAM options exist," not about a custom
//   chip another chapter grades; it does not transfer to that lineage.
//
// Duplicate-grading check (per the build task, before writing any
//   assertion): `grep -rli "68010\|68020\|68030"`, `grep -rli "sysctrl"`,
//   `grep -rli "system_chipmem\|system_slowmem\|system_fastmem"` across
//   every .cpp/.md/Makefile under nanomig-hrm-tests/hrm/ returned nothing
//   on this proposition (three incidental numeric hits inside generated
//   obj_dir/*.cpp Verilator output are build artefacts). No existing
//   package builds `sysctrl`. Nothing here re-grades an accepted verdict.
//
// RTL read for step 2, after SPECIFICATION.md was committed (c481d19):
//   sysctrl.v in full (320 lines). Key facts:
//   - sysctrl.v:16-18 (data_in_strobe/data_in_start/data_in[7:0]) and
//     :171-261 are the whole serial config protocol: a command is a
//     3-byte sequence for CMD 4 ("config values") -- start byte carrying
//     the command number, an id byte selecting which system_* value to
//     set, then the value byte -- latched one byte per posedge clk where
//     data_in_strobe is high (sysctrl.v:171-178, :209-213).
//   - sysctrl.v:42 `system_cpu` is `output reg [1:0]`. sysctrl.v:230-231:
//     comment "CPU 68000(0), 68010(1) or 68020(2)"; code
//     `system_cpu <= data_in[1:0]==2'b10 ? 2'b11 : data_in[1:0]`. This
//     remaps raw input 2 (2'b10) to output 3 (2'b11) -- the SAME output
//     raw input 3 already produces unmapped. Raw inputs 2 and 3 collide;
//     only 3 distinct system_cpu states (0, 1, 3) are reachable through
//     this path, not 4. The comment never mentions "68030" at all, at any
//     tag. (A crossreader's restatement for opus:1-components.c7 also
//     flags a scan artefact -- the source PDF's "68010,68020," lost a
//     space after the first comma; a scanning defect, not a content
//     difference, and not relevant to this RTL finding.)
//   - sysctrl.v:49-51 declare `system_chipmem`, `system_slowmem`,
//     `system_fastmem`, all `output reg [1:0]`. sysctrl.v:241,243,245:
//     `system_chipmem <= data_in[1:0]`, `system_slowmem <= data_in[1:0]`,
//     `system_fastmem <= data_in[1:0]` -- all three are direct, unremapped
//     passthroughs (no collision, unlike system_cpu).
//   - sysctrl.v:240 comment: `Chipmem 512k(0), 1M(1), 1.5M(2) or 2M(2)`
//     -- an internal defect: tag "(2)" is used for both "1.5M" and "2M",
//     and raw value 3 (2'b11), though reachable and distinct (direct
//     passthrough), has no label at all.
//   - sysctrl.v:244 comment: `Fastmem none(0), 2M(1), 4M(2) or 8M(3)` --
//     four distinct tags, one per label, no defect.
//   - sysctrl.v:259: `if(id == "B") system_lcd_v_pos <= data_in;` -- a
//     full 8-bit passthrough, used below only as a generic, port-agnostic
//     liveness proof for the 3-byte config protocol itself, decoupled from
//     any encoding quirk of system_cpu/system_chipmem/system_fastmem.
//   - sysctrl.v:89 `initial $readmemh("amiga_xml.hex", amiga_xml);` --
//     unrelated to any claim graded here; the Makefile copies the hex file
//     next to the built binary so this does not raise a runtime warning.
//
// Every mutation below is a compile-time variant, replayable with
// `make clean && make MUT=MUT_n test` (n = 0..4, matching the MUT_n token
// at the end of each MUTATION record at the bottom of this file) -- see
// the `#if MUT_n` guard at each assertion's stimulus/oracle site. Every
// mutation was actually applied (`make MUT=MUT_n test`, a full separate
// build+run) and the PASS/FAIL lines quoted in the MUTATION records are
// what that run printed, not predictions.

#include <verilated.h>
#include "Vsysctrl.h"
#include <cstdint>
#include <iostream>
#include <string>

#ifndef MUT_0
#define MUT_0 0
#endif
#ifndef MUT_1
#define MUT_1 0
#endif
#ifndef MUT_2
#define MUT_2 0
#endif
#ifndef MUT_3
#define MUT_3 0
#endif
#ifndef MUT_4
#define MUT_4 0
#endif

namespace {

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

int count_distinct(const uint8_t* vals, int n) {
    int distinct = 0;
    for (int i = 0; i < n; ++i) {
        bool seen_before = false;
        for (int j = 0; j < i; ++j) {
            if (vals[j] == vals[i]) { seen_before = true; break; }
        }
        if (!seen_before) ++distinct;
    }
    return distinct;
}

// Fixture wrapper for `sysctrl`, compiled alone. Single clock domain
// (sysctrl.v:13, `always @(posedge clk)`), no separate enables.
class Harness {
public:
    Harness() : dut(new Vsysctrl) {
        dut->clk = 0;
        dut->reset = 1;
        dut->data_in_strobe = 0;
        dut->data_in_start = 0;
        dut->data_in = 0;
        dut->int_in = 0;
        dut->buttons = 0;
        for (int i = 0; i < 8; ++i) tick();
        dut->reset = 0;
        for (int i = 0; i < 4; ++i) tick();
    }
    ~Harness() { dut->final(); delete dut; }
    Harness(const Harness&) = delete;
    Harness& operator=(const Harness&) = delete;

    void tick() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
    }

    // One byte of the sysctrl serial config protocol (sysctrl.v:171-178):
    // latched on the posedge where data_in_strobe is high. `start`=true
    // marks the first byte of a command (resets `state`, latches
    // `command`); false for every byte after it.
    void pulse_byte(bool start, uint8_t byte) {
        dut->data_in_start = start ? 1 : 0;
        dut->data_in = byte;
        dut->data_in_strobe = 1;
        tick();
        dut->data_in_strobe = 0;
        dut->data_in_start = 0;
        dut->data_in = 0;
        tick();
    }

    // CMD 4 (sysctrl.v:209-260): start byte carrying command number 4,
    // then an id byte selecting which system_* value to set, then the
    // value byte itself.
    void write_config(char id, uint8_t value) {
        pulse_byte(/*start=*/true, 0x04);
        pulse_byte(/*start=*/false, static_cast<uint8_t>(id));
        pulse_byte(/*start=*/false, value);
    }

    Vsysctrl* dut;
};

}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    Checks checks;

    // ========================================================================
    // FIXTURE-LIVE. `sysctrl` has never been measured by this package
    // before. Two distinct, known bytes must both arrive and read back
    // correctly and distinctly through the 3-byte config protocol before
    // anything below is trusted -- using id='B' (system_lcd_v_pos, an
    // 8-bit direct passthrough, sysctrl.v:259), deliberately NOT one of
    // the ports graded below, so this proves the protocol itself works,
    // independent of any single port's own encoding.
    // ========================================================================
    {
        Harness h;
        h.write_config('B', 0xA5);
        const uint8_t r1 = h.dut->system_lcd_v_pos;
#if MUT_0
        h.write_config('B', 0xA5);  // mutation: repeat the first sentinel instead of a second, distinct one
#else
        h.write_config('B', 0x5A);
#endif
        const uint8_t r2 = h.dut->system_lcd_v_pos;
        checks.note("FIXTURE-LIVE: system_lcd_v_pos after two config writes: r1=" +
            std::to_string(r1) + " r2=" + std::to_string(r2));
        checks.expect(r1 == 0xA5 && r2 != r1,
            "FIXTURE-LIVE [no claim id -- infrastructure]: two distinct, known bytes (0xA5, then "
            "0x5A) sent through sysctrl's 3-byte config protocol both arrive and read back "
            "correctly and distinctly at system_lcd_v_pos, before any data-dependent assertion "
            "below is graded.");
    }

    // ========================================================================
    // CPU-OPTIONS-FOUR-DISTINCT-STATES. sol:1-components.c7,
    // opus:1-components.c7 (p.1 "The Amiga also supports the 68010, 68020,
    // and 68030 processors as an option."). Page 1 names four distinct
    // processor types (the base 68000 plus three options); representing
    // four distinct types on a port requires -- at minimum, regardless of
    // which state is meant to mean which processor -- four pairwise
    // distinct, reachable states. system_cpu is 2 bits (0..3): every raw
    // config value 0-3 is driven via id='T' and read back.
    // ========================================================================
    {
        Harness h;
        uint8_t cpu_vals[4];
        for (int i = 0; i < 4; ++i) {
            h.write_config('T', static_cast<uint8_t>(i));
            cpu_vals[i] = h.dut->system_cpu;
        }
        const int distinct = count_distinct(cpu_vals, 4);
        checks.note("CPU-OPTIONS-SWEEP: raw id='T' values 0,1,2,3 -> system_cpu = " +
            std::to_string(cpu_vals[0]) + "," + std::to_string(cpu_vals[1]) + "," +
            std::to_string(cpu_vals[2]) + "," + std::to_string(cpu_vals[3]) +
            " (" + std::to_string(distinct) + " distinct); sysctrl.v:230 comment names only "
            "68000/68010/68020, never 68030, at any tag.");
#if MUT_1
        // Oracle mutation: relax the required distinct-state count from 4
        // (page 1's four processor types) to 3 (the RTL's own measured
        // count), flipping the identical measurement green -- proving the
        // baseline FAIL below is a genuine collision (raw 2 and raw 3 both
        // produce system_cpu=3, sysctrl.v:231), not a broken read of
        // system_cpu.
        checks.expect(distinct == 3,
#else
        checks.expect(distinct == 4,
#endif
            "CPU-OPTIONS-FOUR-DISTINCT-STATES (sol:1-components.c7 opus:1-components.c7 p.1 "
            "\"The Amiga also supports the 68010, 68020, and 68030 processors as an option.\"): "
            "system_cpu reaches only " + std::to_string(distinct) + " pairwise-distinct states "
            "across the four raw config values id='T' accepts, not the four page 1's four named "
            "processor types (68000 plus three options) would require at minimum -- sysctrl.v:231 "
            "remaps raw input 2 onto the same output (3) raw input 3 already produces, so no "
            "labelling of this port's reachable states could represent all four types "
            "simultaneously. This is a NanoMig substitute (src/misc), not the shared chipset "
            "lineage; the finding does not transfer to agnus/paula/denise/gary.");
    }

    // ========================================================================
    // CONTROL-CPU-BASE-AND-68010 [no claim id, same basis as
    // CPU-OPTIONS-FOUR-DISTINCT-STATES]. The two lowest option slots (raw
    // 0 = 68000, raw 1 = 68010, sysctrl.v:230's own comment) read back
    // distinctly -- the write/read path itself works; the four-way
    // collision found above is a genuine RTL fact, not a broken harness.
    // ========================================================================
    {
        Harness h;
        h.write_config('T', 0);
        const uint8_t a = h.dut->system_cpu;
#if MUT_2
        h.write_config('T', 0);  // mutation: repeat the same raw value instead of a second, different one
#else
        h.write_config('T', 1);
#endif
        const uint8_t b = h.dut->system_cpu;
        checks.note("CONTROL-CPU-BASE-AND-68010: raw 0 -> system_cpu=" + std::to_string(a) +
            ", raw " + std::string(MUT_2 ? "0 (repeat)" : "1") + " -> system_cpu=" + std::to_string(b));
        checks.expect(a == 0 && b != a,
            "CONTROL-CPU-BASE-AND-68010 [no claim id, same basis as "
            "CPU-OPTIONS-FOUR-DISTINCT-STATES]: raw config values 0 and 1 (id='T') read back as "
            "distinct system_cpu states -- the two lowest option slots work; the collision "
            "measured above is not an artefact of a broken read/write path.");
    }

    // ========================================================================
    // RAM-EXPANSION-CEILING-8MB [no claim id -- grades one clause of a
    // partially-graded claim; see the record() below for the whole claim
    // and its uids, which do not appear on this graded line].
    // p.1 "...expandable to a total of 8 MB". sysctrl.v:244's fastmem
    // comment ("Fastmem none(0), 2M(1), 4M(2) or 8M(3)") has four distinct
    // tags, no defect: raw value 3 is unambiguously "8M".
    // ========================================================================
    {
        Harness h;
#if MUT_3
        h.write_config('Q', 2);  // mutation: probe the "4M" tag instead of the "8M" ceiling
#else
        h.write_config('Q', 3);
#endif
        const uint8_t fastmem = h.dut->system_fastmem;
        checks.note("RAM-EXPANSION-CEILING: raw id='Q' value " + std::string(MUT_3 ? "2" : "3") +
            " -> system_fastmem=" + std::to_string(fastmem));
        checks.expect(fastmem == 3,
            "RAM-EXPANSION-CEILING-8MB [no claim id -- grades one clause of a partially-graded "
            "claim; see the record() at the end of main() for the whole claim and its uids] "
            "(p.1 \"...expandable to a total of 8 MB\"): the expansion-RAM (Fastmem) "
            "configuration reaches a distinctly "
            "selectable state that sysctrl.v:244's own documentation names \"8M\" without "
            "ambiguity, matching the page's expansion-ceiling figure -- system_fastmem=" +
            std::to_string(fastmem) + ". This is a NanoMig substitute (src/misc), not the shared "
            "chipset lineage.");
    }

    // ========================================================================
    // RAM-BASE-FLOOR-BELOW-256K-ABSENT [no claim id -- grades one clause of
    // a partially-graded claim; see the record() below for the whole claim
    // and its uids, which do not appear on this graded line]. p.1
    // "From 256K to 2 MB of RAM". sysctrl.v:240
    // documents raw value 0 (system_chipmem's numeric floor -- direct
    // data_in[1:0] passthrough, no remap, unlike system_cpu) as "512k",
    // unambiguously (the comment's "(2)"-tag duplication affects only the
    // top two entries, 1.5M and 2M, not this one).
    // ========================================================================
    {
        Harness h;
        h.write_config('Y', 0);
        const uint8_t chipmem_floor = h.dut->system_chipmem;
        checks.note("RAM-BASE-FLOOR: raw id='Y' value 0 -> system_chipmem=" +
            std::to_string(chipmem_floor) + "; sysctrl.v:240 documents this state \"512k\"");
#if MUT_4
        // Oracle mutation: relax the required floor from the page's 256K
        // to sysctrl's own documented floor (512K) -- flips the identical
        // measured floor state to green, proving the baseline FAIL below
        // is a genuine 256K-vs-512K unit gap, not a broken read of raw
        // value 0.
        checks.expect(chipmem_floor == 0,
#else
        checks.expect(false,
#endif
            "RAM-BASE-FLOOR-BELOW-256K-ABSENT [no claim id -- grades one clause of a "
            "partially-graded claim; see the record() at the end of main() for the whole claim "
            "and its uids] (p.1 \"From 256K to 2 MB of RAM\"): no reachable system_chipmem "
            "configuration corresponds to a capacity "
            "as low as 256K. Raw config value 0 -- confirmed live by FIXTURE-LIVE's identical "
            "protocol -- is this field's numeric floor (direct passthrough, no remap) and reads "
            "back as " + std::to_string(chipmem_floor) + "; sysctrl.v:240 documents that state "
            "\"512k\", not 256K, and no state smaller than a 2-bit field's own numeric minimum "
            "exists. 512K > 256K, so the page's stated floor is not represented on this port. "
            "This is a NanoMig substitute (src/misc), not the shared chipset lineage.");
    }

    // ------------------------------------------------------------------
    // Declined claim content. Per the build task's own rule, a uid on a
    // graded (expect()) line counts as ASSERTED whatever the label's prose
    // says -- so where only PART of a claim is graded, the uid goes on a
    // record() line alone, telling the whole story, and no graded label
    // above carries it.
    // ------------------------------------------------------------------
    checks.record(
        "sol:1-components.c12 opus:1-components.c12 (p.1 \"From 256K to 2 MB of RAM expandable "
        "to a total of 8 MB (over a gigabyte on the Amiga 3000).\"): PARTIALLY graded. The "
        "expansion-RAM (Fastmem) 8 MB ceiling is graded above at RAM-EXPANSION-CEILING-8MB "
        "(PASS -- system_fastmem reaches a distinctly selectable, unambiguously-labelled \"8M\" "
        "state). The base-RAM (Chipmem) 256K floor is graded above at "
        "RAM-BASE-FLOOR-BELOW-256K-ABSENT (FAIL -- the lowest reachable state is documented "
        "\"512k\", not 256K). Two clauses of this same sentence are NOT graded: (1) the base-RAM "
        "2 MB CEILING -- sysctrl.v:240's own comment is internally inconsistent (it tags both "
        "\"1.5M\" and \"2M\" with the same \"(2)\", and names no label at all for the field's "
        "fourth reachable raw value), so which raw code the design intends as \"2 MB\" cannot be "
        "read off without guessing designer intent, which would be inventing a requirement the "
        "source does not state; (2) \"over a gigabyte on the Amiga 3000\" -- an A3000-specific "
        "memory-controller figure with no sysctrl counterpart (sysctrl is a single, model-generic "
        "fixture; per SPECIFICATION.md, this clause is out of scope on that basis alone, "
        "independent of the comment-ambiguity reason above).");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed\n";
    return checks.failed == 0 ? 0 : 1;
}

// MUTATION FIXTURE-LIVE stimulus red MUT_0: the second config byte changed
//   from a distinct sentinel (0x5A) to a repeat of the first (0xA5) -- FAIL,
//   the two readbacks no longer differ.
// MUTATION CPU-OPTIONS-FOUR-DISTINCT-STATES oracle green MUT_1: the required
//   distinct-state count relaxed from 4 (page 1's four processor types) to
//   3 (the RTL's own measured count) -- PASS on the identical measurement,
//   showing the baseline FAIL is the genuine raw-2/raw-3 collision, not a
//   broken read of system_cpu.
// MUTATION CONTROL-CPU-BASE-AND-68010 stimulus red MUT_2: the second probe
//   changed from raw 1 (68010) to a repeat of raw 0 (68000) -- FAIL, the two
//   readbacks no longer differ.
// MUTATION RAM-EXPANSION-CEILING-8MB stimulus red MUT_3: the probed raw
//   value changed from 3 (the "8M" ceiling) to 2 (the "4M" tag) -- FAIL,
//   system_fastmem reads back 2, not the required 3.
// MUTATION RAM-BASE-FLOOR-BELOW-256K-ABSENT oracle green MUT_4: the required
//   floor relaxed from the page's 256K to sysctrl's own documented floor
//   (512K, i.e. "does raw value 0 read back as itself") -- PASS on the
//   identical measurement, showing the baseline FAIL is the genuine
//   256K-vs-512K unit gap, not a broken read of raw value 0.
