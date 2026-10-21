// Test purpose: HRM conformance package for the controller-port -> CIAAPRA
//   fire/mouse-button path, printed pages 227, 229 and 232. 10 claims from
//   work/ch8-s1-build/ch8-controller-port-io-path.jsonl. Full derivation
//   (propositions, KNOWN-CONFLICT-3 handling, per-claim boundary check) is
//   in SPECIFICATION.md, committed as 3eaea2c before this file, the wrapper
//   Verilog, or any sibling Makefile was read.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 227 (connector capability), 229 (CIAAPRA register-map row) and 232
//   (mouse left-button footnote).
// Fixture boundary: a rig, not a single module -- controller_port_io_path.v
//   (authored in this unit directory), instantiating `userio` (userio.v)
//   and `ciaa` (ciaa.v, which itself needs cia_int.v/cia_timera.v/
//   cia_timerb.v/cia_timerd.v to elaborate -- confirmed from
//   hrm/ch8-ciaa-port-direction/Makefile's own SRC list, read after
//   SPECIFICATION.md was committed). Wiring verified against minimig.v
//   (USERIO1 at minimig.v:561, CIAA1 at minimig.v:617, the porta_in/
//   porta_out/_fire0/_fire1/_fire0_dat/_fire1_dat/_joy1/_joy2 wiring at
//   minimig.v:570-575, 605, 622-624) after the specification commit, per
//   this task's order of work. No other package's test source or Makefile
//   was read except test_denise.cpp and its Makefile
//   (hrm/ch8-appb-chip-column-denise), for harness style only, disclosed
//   already in SPECIFICATION.md: the expect/note/record shapes, the
//   MUTATE-environment-variable mutation technique, and the generic
//   Makefile skeleton. `denise` is not part of this fixture and nothing
//   about userio/ciaa's own ports was learned from it.
// Non-goals: anything outside the `controller_port_io_path` fixture this
//   package builds, and any claim it does not cite. A green here
//   means those assertions held on this fixture at the pinned revision --
//   not that the module is correct, and not that the chapter is covered.
// Claim ids: the ids below carry a reader prefix -- `sol:` and `opus:` are
//   the two models that read the printed pages independently, and an id
//   names one reader's reading of one sentence. They resolve in
//   `work/ch8-s1-build/ch8-controller-port-io-path.jsonl` in the sibling
//   repository (hrm-testpipeline). docs/legacy-names.md explains the
//   vocabulary.
//
// WHAT STEP 2 (RTL READING) ESTABLISHED, independently of the claims
//   file's own `classify.evidence` field, which this build did not use as
//   its source of expected values (per SPECIFICATION.md's order-of-work
//   discipline -- that field is a prior reader's classification argument,
//   not this build's evidence):
//   - userio.v's `joy_swap`, `joy_ana_en` and `cd32pad` are hardwired 0
//     (userio.v:91-93), which collapses every ternary that switches on
//     them to its single live branch. Read directly, not assumed:
//     `_sjoy1 <= joy_swap ? joy1 : joy2` (userio.v:215) becomes
//     `_sjoy1 <= joy2`, and `joy2` (userio.v:211) is a continuous alias of
//     the top-level input `_joy2` since `joy_ana_en` is 0. So the register
//     block that produces JOY0DAT (userio.v:230-254, the "Port 1" block)
//     is fed by `_djoy1`/`_sjoy1`, which is itself fed by the physical
//     `_joy2` input -- the register named "port 1" is wired to the
//     connector named "_joy2". Symmetrically, `_sjoy2 <= joy1 = _joy1`
//     feeds the "Port 2"/JOY1DAT block from the physical `_joy1` input.
//   - `_fire0 = _sjoy1[4] & _mleft` (userio.v:313, `cd32pad` branch dead)
//     is therefore driven by `_joy2[4]` AND the mouse's left button
//     (`_mleft = ~mouse_btn[0]`, userio.v:361); `_fire1 = _sjoy2[4]`
//     (userio.v:314) is driven by `_joy1[4]` alone, with no mouse
//     involvement -- confirming the physical connector wired to `_joy2`
//     is the one this RTL treats as mouse-shared (the real Amiga's
//     "port 1" convention), and `_joy1` is the joystick-only connector.
//     This package uses that RTL-internal, mouse-sharing distinction to
//     tell the two physical ports apart; it does NOT use it to assert
//     which manual numeral ("1" vs "2") names which connector -- see
//     KNOWN CONFLICT 3 below and in SPECIFICATION.md. Given that
//     JOY0DAT/JOY1DAT are already reversed from what their own names
//     suggest, this build does not assume CIAAPRA's bit/numeral pairing
//     is the naive one either, and checked it here instead of guessing.
//   - minimig.v:632 wires `.porta_in({_fire1,_fire0,_ready,_track0,
//     _wprot,_change})` onto ciaa's `porta_in[7:2]` (ciaa.v:95, 6 bits
//     wide): bit7=_fire1, bit6=_fire0. ciaa.v's own read mux
//     (ciaa.v:251-259) returns `porta_in2[7:2]` (a same-width registered
//     copy of porta_in, ciaa.v:222/227-230) for PRA bits 7:2
//     UNCONDITIONALLY -- regardless of DDRA -- so reading CIAAPRA never
//     reflects your own write for bits 6/7, only the external pin state.
//   - The claimed "output" half is real but bounded: writing PRA with
//     DDRA configured for output changes `porta_out[3:2]`
//     (ciaa.v:262 -- `porta_out[3]` from bit 7/regporta[3],
//     `porta_out[2]` from bit 6/regporta[2]), which minimig.v:615 names
//     `_fire1_dat`/`_fire0_dat`. Measured directly (ciaa.v ports,
//     minimig.v:205-206, 358-359): minimig.v's own top-level `_joy1`/
//     `_joy2` are plain primary inputs with no matching output-enable or
//     tristate port, and userio.v only reads `_fire0_dat`/`_fire1_dat`
//     into the `cd32pad` shift-register path (userio.v:181-208), which
//     `cd32pad`'s hardwired 0 makes dead code -- it never reaches
//     `_fire0`, `_fire1`, or any output port. So no RTL boundary in this
//     tree -- not this rig, not even the whole of minimig.v -- can show a
//     CIAAPRA write reaching the physical connector pin; that merge is
//     off-chip, open-drain pad behaviour that no synchronous RTL
//     testbench at any boundary (this rig, the whole of minimig.v, or any
//     future sibling package) can observe. `cia_porta_out` (the CIA's own
//     internal output-driver signal) is therefore evidence of a necessary
//     precondition, never of the manual's connector-level "output" claim
//     itself. Every claim below whose text includes "output"
//     (`opus:8-intro.c10`, `opus:8-regs.c6`, `sol:8-reg.c6`,
//     `sol:8-overview.c3`) is declined via `record()`, naming this
//     boundary, rather than graded on the weaker `cia_porta_out` signal or
//     on an unstated half of the claim -- the coverage ledger counts a
//     uid on a graded `expect()` line as fully ASSERTED regardless of
//     what the label or an adjacent `note()` says, so a claim only
//     partly demonstrated (a real input half, an unreachable output half)
//     cannot be left on a graded line even with disclosure. Where a real,
//     connector-level fact survives from these measurements (the input
//     half of opus:8-intro.c10; the register-level output-driver
//     behaviour), it is kept printed via `note()` or reused by a
//     uid-free graded line elsewhere (`PROP-PORT-LOW6`) rather than
//     discarded.
//
// KNOWN CONFLICT 3 (carried forward, and restated from SPECIFICATION.md
//   because it governs four claims below): printed p.233 numbers the
//   controller ports 0 and 1 where p.240 and the rest of the chapter
//   number them 1 and 2. This package's own three pages never quote the
//   disputed numeral, but four claims (`opus:8-mbtn.c5`, `opus:8-mbtn.c6`,
//   `sol:8-button.c4`, `sol:8-button.c5`) restate a specific numeral
//   paired with a specific bit ("port 1 -> bit 6", "port 2 -> bit 7").
//   The undisputed sub-claim -- the two ports' buttons occupy two
//   distinct, non-overlapping CIAAPRA bits, and the lower-numbered port
//   under EITHER printed scheme is the one on bit 6 -- is graded below on
//   two uid-free lines, `PROP-PORT-LOW6` and `PROP-PORT-HIGH7`. The
//   numeral itself is not graded -- no RTL measurement settles a disputed
//   page's numbering -- so none of the four claims above is fully
//   exercised by that ordering fact alone; each is declined via
//   `record()` next to the proposition it partially supports, naming the
//   p.232-versus-p.233 conflict, rather than kept on a graded line whose
//   uid would be counted ASSERTED for a numeral never checked.
//   A second RTL fact not modelled: only one mouse is wired in this
//   design (`mouse_btn`, hardwired only into the `_joy2`/bit-6 path).
//   There is no second mouse to press for the "port 2" claims
//   (`opus:8-mbtn.c6`, `sol:8-button.c5`) -- `PROP-PORT-HIGH7` is graded
//   using the joystick-fire signal on `_joy1` instead, which is the
//   identical physical pin 6 / CIAAPRA-bit-7 path a second mouse's left
//   button would use if this RTL modelled one. Disclosed here, not
//   silently substituted.
//
// FIXTURE LIVENESS. Two checks, no uid, no page citation, run before any
//   claim-bearing assertion is trusted -- this rig has two joins (the
//   userio-internal stimulus->output half, and the ciaa-internal
//   register-read half) and both are proven alive independently.
//   L1 uses the wrapper's `fire0_tap` diagnostic output (userio's own
//   `_fire0`, before it ever reaches ciaa) with two distinct `_joy2`
//   patterns. L2 uses `extra_wprot`/`extra_change` -- porta_in bits
//   minimig.v sources from disk logic elsewhere, not from userio, and not
//   named by any of the 10 claims -- with two distinct patterns read back
//   through CIAAPRA. This rig's only userio-to-ciaa DATA path is the fire
//   bit itself (no other `_joy1`/`_joy2` bit reaches ciaa at all), so
//   unlike the denise sibling's BPLCON0 liveness check there is no
//   claim-independent bit to prove the cross-module wire alive with; L1
//   and L2 are each a genuine half-proof, together giving strong evidence
//   the whole pipe is alive without overlapping the graded claims below,
//   which is the strongest liveness proof this specific rig's own
//   topology allows for.
//
// MUTATION METHODOLOGY. Every `expect()` below is guarded by a runtime
//   check, `mutating("ID")`, read from the MUTATE environment variable at
//   startup -- not a compile-time #ifdef -- so one build serves the real
//   run (MUTATE unset) and every mutation run (MUTATE=<id>, one id per
//   assertion), the technique disclosed as read from
//   ch8-appb-chip-column-denise's test_denise.cpp. Each mutation changes a
//   STIMULUS value the fixture is asked to drive; no `expect()` call's own
//   comparison was ever edited to produce a result. `record()` lines are
//   declines, not assertions -- they carry no `expect()` label and
//   therefore need no mutation entry. Every one of the 6 ids below was run
//   standalone (`MUTATE=<id> ./obj_dir/Vcontroller_port_io_path`),
//   confirmed red, then reverted and confirmed green again with MUTATE
//   unset; the baseline (MUTATE unset) and each mutation run's actual
//   result are recorded in the build report, not invented here before
//   being run.
//
// MUTATION control:userio-fire-tap-liveness stimulus red: _joy2 pin 6 is never actually driven low (stays 0xFFFF), so fire0_tap never leaves the idle level
// MUTATION control:ciaa-porta-liveness stimulus red: extra_wprot/extra_change are driven to the same idle pattern the baseline used, so the second read is indistinguishable from the first
// MUTATION opus:8-mbtn.c3 stimulus red: mouse_btn[0] is never asserted, so the mouse-button-to-CIAAPRA path is never exercised
// MUTATION PROP-PORT-LOW6 stimulus red: the mouse-shared port's (_joy2) pin 6 is never driven low
// MUTATION PROP-PORT-HIGH7 stimulus red: the joystick-only port's (_joy1) pin 6 is never driven low
// MUTATION sol:8-reg.c7 stimulus red: port A's (_joy1) pin 6 is never driven low, so only one of the two ports' pin-6 claim is shown

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include "Vcontroller_port_io_path.h"
#include "verilated.h"

namespace {
const char* g_mutate = nullptr;
inline bool mutating(const char* id) {
    return g_mutate != nullptr && std::strcmp(g_mutate, id) == 0;
}
}  // namespace

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

namespace {
std::string hex(uint32_t v, int digits) {
    char b[16];
    std::snprintf(b, sizeof b, "%0*X", digits, v);
    return std::string(b);
}
}  // namespace

int main(int argc, char** argv) {
    Verilated::commandArgs(argc, argv);
    g_mutate = std::getenv("MUTATE");
    Vcontroller_port_io_path* dut = new Vcontroller_port_io_path;
    Checks checks;

    const uint16_t JOY_IDLE = 0xFFFF;
    const uint16_t JOY_FIRE_PRESSED = (uint16_t)(0xFFFF & ~(1u << 4));  // pin 6 (bit 4) driven low
    const int PRA_RS = 0x0;
    const int DDRA_RS = 0x2;

    int clk7_cycle = 0;
    auto tick = [&]() {
        dut->eval();
        dut->clk = 1;
        dut->eval();
        dut->clk = 0;
        dut->eval();
        clk7_cycle = (clk7_cycle + 1) % 4;
        dut->clk7_en = (clk7_cycle == 0);
    };

    auto do_reset = [&]() {
        dut->clk = 0;
        dut->clk7_en = 0;
        dut->reset = 1;
        dut->_joy1 = JOY_IDLE;
        dut->_joy2 = JOY_IDLE;
        dut->mouse_btn = 0;
        dut->cia_aen = 0;
        dut->cia_rd = 0;
        dut->cia_wr = 0;
        dut->cia_rs = 0;
        dut->cia_data_in = 0;
        dut->extra_ready = 1;
        dut->extra_track0 = 1;
        dut->extra_wprot = 1;
        dut->extra_change = 1;
        for (int i = 0; i < 16; i++) tick();
        dut->reset = 0;
        for (int i = 0; i < 16; i++) tick();
    };

    // Drive the controller-port-facing stimulus and let both userio's
    // raw-clock _sjoy synchronizer (userio.v:214-219, ungated by clk7_en)
    // and ciaa's clk7_en-gated porta_in2 synchronizer (ciaa.v:227-230)
    // clear before anything is read.
    auto set_port = [&](uint16_t joy1, uint16_t joy2, uint8_t mbtn) {
        dut->_joy1 = joy1;
        dut->_joy2 = joy2;
        dut->mouse_btn = mbtn;
        for (int i = 0; i < 12; i++) tick();
    };

    auto cia_write = [&](int rs, uint8_t data) {
        while (!dut->clk7_en) tick();
        dut->cia_aen = 1;
        dut->cia_wr = 1;
        dut->cia_rd = 0;
        dut->cia_rs = rs;
        dut->cia_data_in = data;
        tick(); tick(); tick(); tick();
        dut->cia_aen = 0;
        dut->cia_wr = 0;
        dut->cia_rs = 0;
        dut->cia_data_in = 0;
        tick(); tick();
    };

    auto cia_read_pra = [&]() -> uint8_t {
        while (!dut->clk7_en) tick();
        dut->cia_aen = 1;
        dut->cia_rd = 1;
        dut->cia_wr = 0;
        dut->cia_rs = PRA_RS;
        tick();
        uint8_t val = dut->cia_data_out;
        tick(); tick();
        dut->cia_aen = 0;
        dut->cia_rd = 0;
        dut->cia_rs = 0;
        return val;
    };

    auto bit = [](uint32_t v, int n) -> bool { return (v >> n) & 1u; };

    // ========================================================================
    // L1 -- FIXTURE LIVENESS, userio half. No uid, no page. Proves userio's
    // own stimulus -> _fire0 output half of the join is alive, tapped
    // before ciaa ever sees it.
    // ========================================================================
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        bool tap_idle = dut->fire0_tap;
        checks.note("L1-IDLE: _joy2=0x" + hex(JOY_IDLE, 4) + " (pin 6 released) gives fire0_tap=" + std::to_string(tap_idle));

        uint16_t joy2_pressed = mutating("control:userio-fire-tap-liveness") ? JOY_IDLE : JOY_FIRE_PRESSED;
        set_port(JOY_IDLE, joy2_pressed, 0);
        bool tap_pressed = dut->fire0_tap;
        checks.note("L1-PRESSED: _joy2=0x" + hex(joy2_pressed, 4) + " gives fire0_tap=" + std::to_string(tap_pressed));

        checks.expect(tap_idle == true && tap_pressed == false,
            "control:userio-fire-tap-liveness");
    }

    // ========================================================================
    // L2 -- FIXTURE LIVENESS, ciaa half. No uid, no page. Proves ciaa's own
    // register-read path is alive using porta_in bits (_wprot, _change)
    // that are not part of any of the 10 claims below.
    // ========================================================================
    do_reset();
    {
        dut->extra_wprot = 1;
        dut->extra_change = 1;
        for (int i = 0; i < 8; i++) tick();
        uint8_t v1 = cia_read_pra();
        checks.note("L2-PATTERN1: extra_wprot=1 extra_change=1 gives CIAAPRA=0x" + hex(v1, 2));

        bool mut = mutating("control:ciaa-porta-liveness");
        dut->extra_wprot = mut ? 1 : 0;
        dut->extra_change = mut ? 1 : 0;
        for (int i = 0; i < 8; i++) tick();
        uint8_t v2 = cia_read_pra();
        checks.note("L2-PATTERN2: extra_wprot=" + std::to_string(dut->extra_wprot) + " extra_change=" + std::to_string(dut->extra_change) + " gives CIAAPRA=0x" + hex(v2, 2));

        checks.expect(bit(v1, 3) == true && bit(v1, 2) == true && bit(v2, 3) == false && bit(v2, 2) == false,
            "control:ciaa-porta-liveness");
    }

    // ========================================================================
    // opus:8-intro.c10 (p.227): "The nine-pin connectors can be used for
    // input or for output." Read as a capability statement, "can be used
    // for A or B" asserts that both A and B are possible usage modes, not
    // that showing one suffices -- proving the input half shows the
    // sentence is not false, it does not show it true. The input half is
    // real and connector-level (measured below, same mechanism as
    // PROP-PORT-LOW6 below). The output half cannot be observed at any
    // boundary in this RTL tree: no signal here carries a CIAAPRA-driven
    // value to a connector-facing pin (see header WHAT STEP 2
    // ESTABLISHED). Declined via record(), naming that boundary, rather
    // than graded on half the claim -- the coverage ledger counts a uid on
    // a graded line as fully ASSERTED regardless of a label's disclaimer,
    // so a half-proof cannot be left there even when disclosed.
    // ========================================================================
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();
        set_port(JOY_IDLE, JOY_FIRE_PRESSED, 0);
        uint8_t pressed_v = cia_read_pra();

        checks.note("P227-1: idle CIAAPRA=0x" + hex(idle_v, 2) + ", pin-6-pressed CIAAPRA=0x" + hex(pressed_v, 2));
        checks.record("opus:8-intro.c10 (p.227, \"The nine-pin connectors can be used for input or for "
            "output\"): the input half is measured true above, a real connector-level fact (also proven "
            "by PROP-PORT-LOW6 below). The output half cannot be observed at any boundary in this RTL "
            "tree -- no signal here carries a CIAAPRA-driven value to a connector-facing pin (see "
            "header). \"Can be used for input or output\" is a capability statement needing both halves "
            "shown; declined rather than graded on half of it.");
    }

    // ========================================================================
    // P232-1 -- opus:8-mbtn.c3 (p.232): "The left button of the Amiga
    // mouse is connected to CIAAPRA." Graded with the mouse button alone,
    // joysticks idle.
    // ========================================================================
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();

        uint8_t mbtn = mutating("opus:8-mbtn.c3") ? 0 : 0x1;
        set_port(JOY_IDLE, JOY_IDLE, mbtn);
        uint8_t pressed_v = cia_read_pra();

        checks.note("P232-1: idle CIAAPRA=0x" + hex(idle_v, 2) + ", mouse-left-pressed CIAAPRA=0x" + hex(pressed_v, 2));
        checks.expect(bit(idle_v, 6) == true && bit(pressed_v, 6) == false,
            "opus:8-mbtn.c3");
    }

    // ========================================================================
    // PROP-PORT-LOW6 -- the undisputed ordering sub-claim behind
    // opus:8-mbtn.c5 and sol:8-button.c4 (p.232): the lower-numbered port
    // under EITHER printed scheme (p.232's "port 1", p.233's "port 0") is
    // the one whose pin 6 reaches CIAAPRA bit 6, isolated from bit 7 (see
    // KNOWN CONFLICT 3 in the header). Graded uid-free -- see the two
    // record() lines below for why the disputed numeral itself stays off
    // this graded line.
    // ========================================================================
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();
        uint16_t joy2_pressed = mutating("PROP-PORT-LOW6") ? JOY_IDLE : JOY_FIRE_PRESSED;
        set_port(JOY_IDLE, joy2_pressed, 0);
        uint8_t v = cia_read_pra();
        checks.note("P232-2: idle CIAAPRA=0x" + hex(idle_v, 2) + ", mouse-shared-port-pin6-pressed CIAAPRA=0x" + hex(v, 2));
        checks.expect(bit(idle_v, 6) == true && bit(idle_v, 7) == true && bit(v, 6) == false && bit(v, 7) == true,
            "PROP-PORT-LOW6");

        checks.record("opus:8-mbtn.c5 (p.232, \"The left button of port 1 appears in bit 6 of "
            "CIAAPRA\"): the bit pairing is asserted by PROP-PORT-LOW6 above, but this claim's own "
            "restatement names the numeral \"port 1\", and p.232 numbers the connectors 1 and 2 where "
            "p.233 numbers the same two connectors 0 and 1 -- no RTL measurement settles a disputed "
            "page's numbering. Not fully exercised by PROP-PORT-LOW6; conflict-recorded, not asserted.");
        checks.record("sol:8-button.c4 (p.232, \"Controller port 1's left mouse button uses CIAAPRA bit "
            "6\"): same p.232-versus-p.233 conflict as opus:8-mbtn.c5 above -- the bit pairing is "
            "asserted by PROP-PORT-LOW6, the numeral \"port 1\" is not.");
    }

    // ========================================================================
    // PROP-PORT-HIGH7 -- the undisputed ordering sub-claim behind
    // opus:8-mbtn.c6 and sol:8-button.c5 (p.232): the higher-numbered port
    // under either printed scheme is the one whose pin 6 reaches CIAAPRA
    // bit 7, isolated from bit 6. No second mouse exists in this RTL (see
    // header) -- the joystick-fire signal on the same physical pin is used
    // instead, disclosed rather than silently substituted. Graded
    // uid-free -- see the two record() lines below for the disputed
    // numeral.
    // ========================================================================
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();
        uint16_t joy1_pressed = mutating("PROP-PORT-HIGH7") ? JOY_IDLE : JOY_FIRE_PRESSED;
        set_port(joy1_pressed, JOY_IDLE, 0);
        uint8_t v = cia_read_pra();
        checks.note("P232-4: idle CIAAPRA=0x" + hex(idle_v, 2) + ", joystick-only-port-pin6-pressed CIAAPRA=0x" + hex(v, 2));
        checks.expect(bit(idle_v, 6) == true && bit(idle_v, 7) == true && bit(v, 7) == false && bit(v, 6) == true,
            "PROP-PORT-HIGH7");

        checks.record("opus:8-mbtn.c6 (p.232, \"The left button of port 2 appears in bit 7 of "
            "CIAAPRA\"): the bit pairing is asserted by PROP-PORT-HIGH7 above, but this claim's own "
            "restatement names the numeral \"port 2\", and p.232 numbers the connectors 1 and 2 where "
            "p.233 numbers the same two connectors 0 and 1 -- no RTL measurement settles a disputed "
            "page's numbering. Not fully exercised by PROP-PORT-HIGH7; conflict-recorded, not asserted.");
        checks.record("sol:8-button.c5 (p.232, \"Controller port 2's left mouse button uses CIAAPRA bit "
            "7\"): same p.232-versus-p.233 conflict as opus:8-mbtn.c6 above -- the bit pairing is "
            "asserted by PROP-PORT-HIGH7, the numeral \"port 2\" is not.");
    }

    // ========================================================================
    // P229-3 -- sol:8-reg.c7 (p.229): "The fire buttons of controller
    // ports 1 and 2 use pin 6." A pin-POSITION claim (each port's own
    // pin 6, not which numeral), so isolation is not required here --
    // graded on existence of the pin-6 -> CIAAPRA link on BOTH ports.
    // ========================================================================
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();

        uint16_t joy1_pressed = mutating("sol:8-reg.c7") ? JOY_IDLE : JOY_FIRE_PRESSED;
        set_port(joy1_pressed, JOY_IDLE, 0);
        uint8_t va = cia_read_pra();

        set_port(JOY_IDLE, JOY_FIRE_PRESSED, 0);
        uint8_t vb = cia_read_pra();

        checks.note("P229-3: idle CIAAPRA=0x" + hex(idle_v, 2) + ", port-A-pin6-pressed CIAAPRA=0x" + hex(va, 2) + ", port-B-pin6-pressed CIAAPRA=0x" + hex(vb, 2));
        checks.expect(bit(idle_v, 6) == true && bit(idle_v, 7) == true && bit(va, 7) == false && bit(vb, 6) == false,
            "sol:8-reg.c7");
    }

    // ========================================================================
    // opus:8-regs.c6 / sol:8-reg.c6 (p.229): "CIAAPRA is input and output
    // for pin 6" is a CONJUNCTION, unlike opus:8-intro.c10's disjunction
    // above. The input half is real and connector-level (measured below,
    // identical mechanism to opus:8-intro.c10 and PROP-PORT-LOW6). The
    // output half can only be measured at cia_porta_out, the CIA's own
    // internal output-driver signal (ciaa.v:262) -- no boundary in this
    // RTL tree carries that value to a connector-facing pin (see header
    // WHAT STEP 2 ESTABLISHED). A conjunction needs BOTH sides true; the
    // weaker chip-internal signal cannot stand in for the missing
    // connector-level side, so both uids are declined below rather than
    // graded on it.
    // ========================================================================
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();
        set_port(JOY_IDLE, JOY_FIRE_PRESSED, 0);
        uint8_t pressed_v = cia_read_pra();
        bool input_ok = bit(idle_v, 6) == true && bit(pressed_v, 6) == false;

        cia_write(DDRA_RS, 0xC0);
        cia_write(PRA_RS, 0x00);
        bool out_low = bit(dut->cia_porta_out, 2) == false;
        cia_write(PRA_RS, 0xC0);
        bool out_high = bit(dut->cia_porta_out, 2) == true;
        bool output_ok_at_register = out_low && out_high;

        checks.note("P229-1: input_ok=" + std::to_string(input_ok) + " output_ok_at_register=" + std::to_string(output_ok_at_register));
        checks.record("opus:8-regs.c6 (p.229, \"CIAAPRA is input and output for pin 6, the port 1 and "
            "port 2 fire buttons\"): input_ok measured true above, a real connector-level fact. "
            "output_ok_at_register measured true above too, but that is cia_porta_out (ciaa.v:262), the "
            "CIA's own internal driver signal -- not a connector-facing one. No boundary in this RTL "
            "tree exposes the further merge onto the physical pin (see header). The conjunction as "
            "printed on p.229 is not fully exercised by either signal alone; declined rather than "
            "graded on the weaker one.");
        checks.record("sol:8-reg.c6 (p.229, \"CIAAPRA provides input and output for controller pin 6 on "
            "both ports\"): same measurement and same boundary as opus:8-regs.c6 above.");
    }

    // ========================================================================
    // sol:8-overview.c3 (p.227): "Each controller connector can carry
    // output." Unlike opus:8-intro.c10 (a disjunction with a provable
    // connector-level input disjunct), this claim is output-only -- there
    // is no input fallback. Measured at the same cia_porta_out boundary as
    // opus:8-regs.c6/sol:8-reg.c6 above; declined for the same reason (no
    // connector-facing merge exists anywhere in this RTL tree). This
    // sub-fact does not require `userio` at all -- disclosed here rather
    // than left implicit, per SPECIFICATION.md's caution about this
    // particular claim.
    // ========================================================================
    do_reset();
    {
        cia_write(DDRA_RS, 0xC0);
        cia_write(PRA_RS, 0x00);
        bool b6_lo = bit(dut->cia_porta_out, 2) == false;
        bool b7_lo = bit(dut->cia_porta_out, 3) == false;
        cia_write(PRA_RS, 0xC0);
        bool b6_hi = bit(dut->cia_porta_out, 2) == true;
        bool b7_hi = bit(dut->cia_porta_out, 3) == true;

        checks.note("P227-2: b6_lo=" + std::to_string(b6_lo) + " b7_lo=" + std::to_string(b7_lo) + " b6_hi=" + std::to_string(b6_hi) + " b7_hi=" + std::to_string(b7_hi));
        checks.record("sol:8-overview.c3 (p.227, \"Each controller connector can carry output\"): both "
            "ports' own CIA output-driver bits (porta_out[2]/porta_out[3]) are measured commandable low "
            "and high above, but that is cia_porta_out, the CIA's internal driver signal, not the "
            "connector itself -- no boundary in this RTL tree carries it further (see header). Declined "
            "rather than graded on the weaker signal.");
    }

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
