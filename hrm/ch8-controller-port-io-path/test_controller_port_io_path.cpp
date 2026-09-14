// Test purpose: HRM conformance package for the controller-port -> CIAAPRA
//   fire/mouse-button path, printed pages 227, 229 and 232. 10 claims from
//   work/ch8-s1-build/ch8-controller-port-io-path.jsonl. Full derivation
//   (propositions, KNOWN-CONFLICT-3 handling, per-claim boundary check) is
//   in SPECIFICATION.md, committed as 52418ca before this file, the
//   wrapper Verilog, or any sibling Makefile was read.
// Test class: HRM_CONFORMANCE
// Primary oracle: Amiga Hardware Reference Manual, Third Edition, printed
//   pages 227 (connector capability), 229 (CIAAPRA register-map row) and
//   232 (mouse left-button footnote).
// Fixture boundary: a rig, not a single module -- controller_port_io_path.v
//   (authored in this unit directory), instantiating `userio` (userio.v)
//   and `ciaa` (ciaa.v, which itself needs cia_int.v/cia_timera.v/
//   cia_timerb.v/cia_timerd.v to elaborate -- confirmed from
//   hrm/ch8-ciaa-port-direction/Makefile's own SRC list, read after
//   SPECIFICATION.md was committed). Wiring verified against minimig.v
//   (USERIO1 at minimig.v:551, CIAA1 at minimig.v:607, the porta_in/
//   porta_out/_fire0/_fire1/_fire0_dat/_fire1_dat/_joy1/_joy2 wiring at
//   minimig.v:560-565, 605, 622-624) after the specification commit, per
//   this task's order of work. No other package's test source or Makefile
//   was read except test_denise.cpp and its Makefile
//   (hrm/ch8-appb-chip-column-denise), for harness style only, disclosed
//   already in SPECIFICATION.md: the expect/note/record shapes, the
//   MUTATE-environment-variable mutation technique, and the generic
//   Makefile skeleton. `denise` is not part of this fixture and nothing
//   about userio/ciaa's own ports was learned from it.
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
//   - minimig.v:622 wires `.porta_in({_fire1,_fire0,_ready,_track0,
//     _wprot,_change})` onto ciaa's `porta_in[7:2]` (ciaa.v:95, 6 bits
//     wide): bit7=_fire1, bit6=_fire0. ciaa.v's own read mux
//     (ciaa.v:251-259) returns `porta_in2[7:2]` (a same-width registered
//     copy of porta_in, ciaa.v:222/227-230) for PRA bits 7:2
//     UNCONDITIONALLY -- regardless of DDRA -- so reading CIAAPRA never
//     reflects your own write for bits 6/7, only the external pin state.
//   - The claimed "output" half is real but bounded: writing PRA with
//     DDRA configured for output changes `porta_out[3:2]`
//     (ciaa.v:262 -- `porta_out[3]` from bit 7/regporta[3],
//     `porta_out[2]` from bit 6/regporta[2]), which minimig.v:605 names
//     `_fire1_dat`/`_fire0_dat`. Measured directly (ciaa.v ports,
//     minimig.v:200-201, 358-359): minimig.v's own top-level `_joy1`/
//     `_joy2` are plain primary inputs with no matching output-enable or
//     tristate port, and userio.v only reads `_fire0_dat`/`_fire1_dat`
//     into the `cd32pad` shift-register path (userio.v:181-208), which
//     `cd32pad`'s hardwired 0 makes dead code -- it never reaches
//     `_fire0`, `_fire1`, or any output port. So no RTL boundary in this
//     tree -- not this rig, not even the whole of minimig.v -- can show a
//     CIAAPRA write reaching the physical connector pin; that merge is
//     off-chip (open-drain pad behaviour). This package therefore grades
//     "output" at the CIAAPRA/CIA-register level (does the write change
//     the chip's own output-driver signal), not at the physical pin, and
//     says so on every assertion whose claim includes the word "output".
//     The register-level output check does not need `userio` at all --
//     disclosed on those assertions rather than left implicit, per
//     SPECIFICATION.md's warning against a rig used for convenience.
//
// KNOWN CONFLICT 3 (carried forward, and restated from SPECIFICATION.md
//   because it governs four assertions below): printed p.233 numbers the
//   controller ports 0 and 1 where p.240 and the rest of the chapter
//   number them 1 and 2. This package's own three pages never quote the
//   disputed numeral, but four claims (`opus:8-mbtn.c5`, `opus:8-mbtn.c6`,
//   `sol:8-button.c4`, `sol:8-button.c5`) assert a specific numeral paired
//   with a specific bit. Graded here: the two ports' buttons occupy two
//   distinct, non-overlapping CIAAPRA bits, and the mouse-shared port (the
//   RTL fact establishing "port 1" on a real Amiga) is the one on bit 6.
//   NOT graded: that the manual's printed numeral "1" is the physically
//   correct name for that connector -- no RTL measurement settles a
//   disputed page's numbering.
//   A second RTL fact not modelled: only one mouse is wired in this
//   design (`mouse_btn`, hardwired only into the `_joy2`/bit-6 path).
//   There is no second mouse to press for the "port 2" claims
//   (`opus:8-mbtn.c6`, `sol:8-button.c5`) -- they are graded using the
//   joystick-fire signal on `_joy1` instead, which is the identical
//   physical pin 6 / CIAAPRA-bit-7 path a second mouse's left button
//   would use if this RTL modelled one. Disclosed here, not silently
//   substituted.
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
//   comparison was ever edited to produce a result. Every one of the 12
//   ids below was run standalone (`MUTATE=<id> ./obj_dir/Vcontroller_port_io_path`),
//   confirmed red, then reverted and confirmed green again with MUTATE
//   unset; the baseline (MUTATE unset) and each mutation run's actual
//   result are recorded in the build report, not invented here before
//   being run.
//
// MUTATION control:userio-fire-tap-liveness stimulus red: _joy2 pin 6 is never actually driven low (stays 0xFFFF), so fire0_tap never leaves the idle level
// MUTATION control:ciaa-porta-liveness stimulus red: extra_wprot/extra_change are driven to the same idle pattern the baseline used, so the second read is indistinguishable from the first
// MUTATION opus:8-intro.c10 stimulus red: _joy2 pin 6 is never driven low, so the input half of "input or output" is never demonstrated
// MUTATION opus:8-mbtn.c3 stimulus red: mouse_btn[0] is never asserted, so the mouse-button-to-CIAAPRA path is never exercised
// MUTATION opus:8-mbtn.c5 stimulus red: the mouse-shared port's (_joy2) pin 6 is never driven low
// MUTATION sol:8-button.c4 stimulus red: the mouse-shared port's (_joy2) pin 6 is never driven low
// MUTATION opus:8-mbtn.c6 stimulus red: the joystick-only port's (_joy1) pin 6 is never driven low
// MUTATION sol:8-button.c5 stimulus red: the joystick-only port's (_joy1) pin 6 is never driven low
// MUTATION sol:8-reg.c7 stimulus red: port A's (_joy1) pin 6 is never driven low, so only one of the two ports' pin-6 claim is shown
// MUTATION opus:8-regs.c6 stimulus red: the input half's stimulus (_joy2 pin 6) is never driven low, though the output-register half is still exercised
// MUTATION sol:8-reg.c6 stimulus red: DDRA is written leaving bit 6 configured as input (0x80 instead of 0xC0), so the output-register half of the claim is never demonstrated for bit 6
// MUTATION sol:8-overview.c3 stimulus red: DDRA is written leaving bit 6 configured as input (0x80 instead of 0xC0), so only one of the two ports' output claim is shown

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
    // P227-1 -- opus:8-intro.c10 (p.227): "The nine-pin connectors can be
    // used for input or for output." Graded on the INPUT half, which
    // alone satisfies the disjunction: a distinct stimulus at pin 6 of the
    // mouse-shared port arrives, and departs, at CIAAPRA bit 6.
    // ========================================================================
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();

        uint16_t joy2_pressed = mutating("opus:8-intro.c10") ? JOY_IDLE : JOY_FIRE_PRESSED;
        set_port(JOY_IDLE, joy2_pressed, 0);
        uint8_t pressed_v = cia_read_pra();

        checks.note("P227-1: idle CIAAPRA=0x" + hex(idle_v, 2) + ", pin-6-pressed CIAAPRA=0x" + hex(pressed_v, 2));
        checks.expect(bit(idle_v, 6) == true && bit(pressed_v, 6) == false,
            "opus:8-intro.c10");
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
    // P232-2/3 -- opus:8-mbtn.c5 and sol:8-button.c4 (p.232): the
    // mouse-shared port's pin 6 appears at bit 6, and only bit 6
    // (isolation from bit 7). Same fact, independent uids, independent
    // expect() lines and independent mutation runs.
    // ========================================================================
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();
        uint16_t joy2_pressed = mutating("opus:8-mbtn.c5") ? JOY_IDLE : JOY_FIRE_PRESSED;
        set_port(JOY_IDLE, joy2_pressed, 0);
        uint8_t v = cia_read_pra();
        checks.note("P232-2: idle CIAAPRA=0x" + hex(idle_v, 2) + ", mouse-shared-port-pin6-pressed CIAAPRA=0x" + hex(v, 2));
        checks.expect(bit(idle_v, 6) == true && bit(idle_v, 7) == true && bit(v, 6) == false && bit(v, 7) == true,
            "opus:8-mbtn.c5");
    }
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();
        uint16_t joy2_pressed = mutating("sol:8-button.c4") ? JOY_IDLE : JOY_FIRE_PRESSED;
        set_port(JOY_IDLE, joy2_pressed, 0);
        uint8_t v = cia_read_pra();
        checks.note("P232-3: idle CIAAPRA=0x" + hex(idle_v, 2) + ", mouse-shared-port-pin6-pressed CIAAPRA=0x" + hex(v, 2));
        checks.expect(bit(idle_v, 6) == true && bit(idle_v, 7) == true && bit(v, 6) == false && bit(v, 7) == true,
            "sol:8-button.c4");
    }

    // ========================================================================
    // P232-4/5 -- opus:8-mbtn.c6 and sol:8-button.c5 (p.232): the
    // joystick-only port's pin 6 appears at bit 7, and only bit 7. No
    // second mouse exists in this RTL (see header) -- the joystick-fire
    // signal on the same physical pin is used instead, disclosed rather
    // than silently substituted.
    // ========================================================================
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();
        uint16_t joy1_pressed = mutating("opus:8-mbtn.c6") ? JOY_IDLE : JOY_FIRE_PRESSED;
        set_port(joy1_pressed, JOY_IDLE, 0);
        uint8_t v = cia_read_pra();
        checks.note("P232-4: idle CIAAPRA=0x" + hex(idle_v, 2) + ", joystick-only-port-pin6-pressed CIAAPRA=0x" + hex(v, 2));
        checks.expect(bit(idle_v, 6) == true && bit(idle_v, 7) == true && bit(v, 7) == false && bit(v, 6) == true,
            "opus:8-mbtn.c6");
    }
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();
        uint16_t joy1_pressed = mutating("sol:8-button.c5") ? JOY_IDLE : JOY_FIRE_PRESSED;
        set_port(joy1_pressed, JOY_IDLE, 0);
        uint8_t v = cia_read_pra();
        checks.note("P232-5: idle CIAAPRA=0x" + hex(idle_v, 2) + ", joystick-only-port-pin6-pressed CIAAPRA=0x" + hex(v, 2));
        checks.expect(bit(idle_v, 6) == true && bit(idle_v, 7) == true && bit(v, 7) == false && bit(v, 6) == true,
            "sol:8-button.c5");
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
    // P229-1 -- opus:8-regs.c6 (p.229): "CIAAPRA is input and output for
    // pin 6, the port 1 and port 2 fire buttons." Graded as a conjunction:
    // INPUT (the mouse-shared port's pin 6 arrives at bit 6, the
    // wide-boundary fact only this rig can show) AND OUTPUT (writing
    // CIAAPRA with DDRA configured for bit 6 changes the chip's own
    // output-driver signal -- a ciaa-internal fact; disclosed, not hidden,
    // per the header note on the physical-pin boundary).
    // ========================================================================
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();
        uint16_t joy2_pressed = mutating("opus:8-regs.c6") ? JOY_IDLE : JOY_FIRE_PRESSED;
        set_port(JOY_IDLE, joy2_pressed, 0);
        uint8_t pressed_v = cia_read_pra();
        bool input_ok = bit(idle_v, 6) == true && bit(pressed_v, 6) == false;

        cia_write(DDRA_RS, 0xC0);
        cia_write(PRA_RS, 0x00);
        bool out_low = bit(dut->cia_porta_out, 2) == false;
        cia_write(PRA_RS, 0xC0);
        bool out_high = bit(dut->cia_porta_out, 2) == true;
        bool output_ok = out_low && out_high;

        checks.note("P229-1: input_ok=" + std::to_string(input_ok) + " output_ok=" + std::to_string(output_ok));
        checks.expect(input_ok && output_ok, "opus:8-regs.c6");
    }

    // ========================================================================
    // P229-2 -- sol:8-reg.c6 (p.229): same fact as opus:8-regs.c6,
    // independent uid. This run's stimulus mutation targets the OUTPUT
    // half instead of the input half, so the two assertions' mutation
    // runs are not identical even though the graded fact is the same.
    // ========================================================================
    do_reset();
    {
        set_port(JOY_IDLE, JOY_IDLE, 0);
        uint8_t idle_v = cia_read_pra();
        set_port(JOY_IDLE, JOY_FIRE_PRESSED, 0);
        uint8_t pressed_v = cia_read_pra();
        bool input_ok = bit(idle_v, 6) == true && bit(pressed_v, 6) == false;

        uint8_t ddra_val = mutating("sol:8-reg.c6") ? 0x80 : 0xC0;
        cia_write(DDRA_RS, ddra_val);
        cia_write(PRA_RS, 0x00);
        bool out_low = bit(dut->cia_porta_out, 2) == false;
        cia_write(PRA_RS, 0xC0);
        bool out_high = bit(dut->cia_porta_out, 2) == true;
        bool output_ok = out_low && out_high;

        checks.note("P229-2: input_ok=" + std::to_string(input_ok) + " output_ok=" + std::to_string(output_ok) + " ddra=0x" + hex(ddra_val, 2));
        checks.expect(input_ok && output_ok, "sol:8-reg.c6");
    }

    // ========================================================================
    // P227-2 -- sol:8-overview.c3 (p.227): "Each controller connector can
    // carry output." Graded at the CIAAPRA-register level (see header):
    // BOTH ports' own output-driver bit (porta_out[2] for the mouse-shared
    // port, porta_out[3] for the joystick-only port) can be independently
    // commanded low and high. This specific sub-fact does not require
    // userio at all -- disclosed here rather than left implicit, per
    // SPECIFICATION.md's caution about this particular claim.
    // ========================================================================
    do_reset();
    {
        uint8_t ddra_val = mutating("sol:8-overview.c3") ? 0x80 : 0xC0;
        cia_write(DDRA_RS, ddra_val);
        cia_write(PRA_RS, 0x00);
        bool b6_lo = bit(dut->cia_porta_out, 2) == false;
        bool b7_lo = bit(dut->cia_porta_out, 3) == false;
        cia_write(PRA_RS, 0xC0);
        bool b6_hi = bit(dut->cia_porta_out, 2) == true;
        bool b7_hi = bit(dut->cia_porta_out, 3) == true;

        checks.note("P227-2: ddra=0x" + hex(ddra_val, 2) + " b6_lo=" + std::to_string(b6_lo) + " b7_lo=" + std::to_string(b7_lo) + " b6_hi=" + std::to_string(b6_hi) + " b7_hi=" + std::to_string(b7_hi));
        checks.expect(b6_lo && b7_lo && b6_hi && b7_hi, "sol:8-overview.c3");
    }

    // The physical-pin-arrival half of "output" (does the CIAAPRA-driven
    // value reach the DB9 connector pin, not just the chip's own output
    // signal) is not graded by any assertion above: no boundary in this
    // RTL tree can observe it. See the header for the specific lines
    // (minimig.v:200-201, 358-359; userio.v:181-208) that establish this
    // rather than assuming it.
    checks.note("OUTPUT-SCOPE: every assertion above whose claim includes the word 'output' is graded at the CIAAPRA/chip output-driver signal (cia_porta_out), never at the physical connector pin -- no RTL boundary in this tree exposes that merge (see header).");

    std::cout << "\n" << checks.passed << " passed, " << checks.failed << " failed.\n";
    return checks.failed == 0 ? 0 : 1;
}
