// controller_port_io_path -- rig wrapper for ch8-controller-port-io-path.
//
// Authored in this unit directory (not part of the NanoMig submodule).
// Instantiates `userio` and `ciaa` and wires them exactly the way
// minimig.v does, so the fixture can see a controller-port pin at one end
// and a CIAAPRA bit at the other -- the wire neither module alone can
// observe. Wiring verified against minimig.v (lines ~551-630) after
// SPECIFICATION.md was committed; ports not needed by any of the 10
// claims under test are tied off to the same idle/pulled-up values
// minimig.v or the 8520's own reset default would give them, and are
// documented inline below rather than left as silent defaults.
//
// Ports not wired from minimig.v's own instantiation, and why:
//  - userio.reg_address_in / data_in / pot_cnt_en: none of the 10 claims
//    read or write a userio register (JOY0DAT, POTGO, ...); only the
//    always-combinational _fire0/_fire1 outputs are used. Tied to 0.
//  - userio.joy_ana1/joy_ana2: analog-joystick path, gated off entirely
//    by userio.v's own `joy_ana_en = 1'b0` (userio.v:93). Tied to 0.
//  - userio.kms_level/kbd_mouse_type/kbd_mouse_data and
//    ciaa.kms_level/kbd_mouse_type/kbd_mouse_data: keyboard-mouse
//    emulation path, unrelated to the controller-port claims. Tied
//    inactive (kbd_mouse_type = 2'b11, a value userio.v never compares
//    against, so the keystrobe/mouse-counter logic they gate never
//    fires; kms_level held constant so no edge is ever seen).
//  - ciaa.tick/eclk: timer-chain inputs; ciaa.v's porta read/write logic
//    (the only logic this package grades) depends on clk/clk7_en/reset/
//    wr/pra/ddra/data_in/porta_in only, never on tick or eclk (verified
//    by reading ciaa.v's porta block, lines 219-263). Tied to 0.
//  - ciaa.portb_in: CIA-A port B, not part of any claim here. Tied to
//    8'hFF, the pulled-up idle level the 8520 gives an unused port.
//  - ciaa.hrtmon_en: Action-Replay freeze feature, unrelated. Tied to 0.
//  - ciaa.irq / ciaa.freeze / userio.data_out: not read by this package.
//    Left unconnected at the top level (verilator -Wno-pinmissing already
//    used by every sibling package for the reverse case; here it is
//    unused-output, not missing-pin, so no warning suppression is even
//    needed).

module controller_port_io_path
(
    input         clk,
    input         clk7_en,
    input         reset,

    // ---- controller-port-facing stimulus (userio side; page 227/232) ----
    // Named exactly as minimig.v names them (minimig.v:205-206) so the
    // wiring below is traceable line-for-line against minimig.v:574-575.
    input  [15:0] _joy1,
    input  [15:0] _joy2,
    input   [2:0] mouse_btn,

    // ---- CIA-A port-A register access (ciaa side; page 229) ----
    input         cia_aen,
    input         cia_rd,
    input         cia_wr,
    input   [3:0] cia_rs,
    input   [7:0] cia_data_in,
    output  [7:0] cia_data_out,
    output  [3:0] cia_porta_out,

    // Non-claim porta_in bits (minimig.v:632 supplies these from disk
    // logic elsewhere in minimig.v, not from userio). Exposed here only
    // so the fixture can prove its ciaa-side register-read path is alive
    // on bits that are not one of the 10 claims under test -- see
    // SPECIFICATION.md's Control/liveness section.
    input         extra_ready,
    input         extra_track0,
    input         extra_wprot,
    input         extra_change,

    // Diagnostic taps on the userio/ciaa join itself. NOT the graded
    // observation channel for any claim -- every graded expect() in the
    // test file reads the join through cia_data_out (the CIAAPRA a
    // programmer would actually read), matching what the manual
    // describes. These two taps exist only for the userio-side half of
    // the liveness proof (see test file), so a failure there can be
    // localised to "userio never produced a distinct value" instead of
    // "either module, somewhere."
    output        fire0_tap,
    output        fire1_tap
);

    wire [3:0] porta_out;
    wire       _fire0_dat = porta_out[2];
    wire       _fire1_dat = porta_out[3];

    assign cia_porta_out = porta_out;
    assign fire0_tap = fire0;
    assign fire1_tap = fire1;

    wire fire0, fire1;

    userio USERIO1
    (
        .clk(clk),
        .clk7_en(clk7_en),
        .reset(reset),
        .reg_address_in(9'h000),
        .data_in(16'h0000),
        .data_out(),
        .pot_cnt_en(1'b0),
        ._fire0(fire0),
        ._fire1(fire1),
        ._fire0_dat(_fire0_dat),
        ._fire1_dat(_fire1_dat),
        ._joy1(_joy1),
        ._joy2(_joy2),
        .joy_ana1(16'h0000),
        .joy_ana2(16'h0000),
        .mouse_btn(mouse_btn),
        .kms_level(1'b0),
        .kbd_mouse_type(2'b11),
        .kbd_mouse_data(8'h00)
    );

    ciaa CIAA1
    (
        .clk(clk),
        .clk7_en(clk7_en),
        .clk7n_en(1'b0),
        .aen(cia_aen),
        .rd(cia_rd),
        .wr(cia_wr),
        .reset(reset),
        .rs(cia_rs),
        .data_in(cia_data_in),
        .data_out(cia_data_out),
        .tick(1'b0),
        .eclk(1'b0),
        .irq(),
        .porta_in({fire1, fire0, extra_ready, extra_track0, extra_wprot, extra_change}),
        .porta_out(porta_out),
        .portb_in(8'hFF),
        .kms_level(1'b0),
        .kbd_mouse_type(2'b11),
        .kbd_mouse_data(8'h00),
        .freeze(),
        .hrtmon_en(1'b0)
    );

endmodule
