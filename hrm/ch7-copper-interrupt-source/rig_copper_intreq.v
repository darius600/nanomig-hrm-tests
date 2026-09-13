// Test-only fixture glue for ch7-copper-interrupt-source. NOT part of nanomig/,
// NOT RTL under test on its own: it only instantiates the two unmodified RTL
// modules (agnus_copper, paula_intcontroller) as one Verilator top so the
// testbench can drive a real Copper MOVE across the real register-address
// path and observe the interrupt controller's response.
//
// Wiring choices, and why:
//
// - `copper_reg_address_in` mirrors the real boundary at agnus.v:231-239
//   ("reg_address = reg_address_cop" only while the Copper's DMA request is
//   granted): it reflects the Copper's own reg_address_out only while
//   `ackdma` is asserted, otherwise it presents 8'hFF, the same "no register
//   selected" idle value agnus_copper.v itself uses for its dummy cycle
//   (see agnus_copper.v:218). This is exactly the self-loop the Copper's own
//   COPINS fetch mechanism depends on (it "fetches" by writing to its own
//   instruction register through the same shared bus).
//
// - `paula_reg_address_in` is left as a free top-level input, decoupled from
//   the Copper, on purpose: paula_intcontroller's single reg_address_in port
//   serves both the write decode AND the combinational readback mux in the
//   real chip, and the fixture has no third module to arbitrate between a
//   write cycle and a readback query the way the real chip bus would. The
//   testbench supplies the Copper's own reg_address_out on this port only at
//   the exact cycle the Copper is granted the bus (mirroring
//   minimig.v's shared custom_data_in fan-out at the same moment); at every
//   other cycle it supplies whatever address it wants to read back. The
//   Copper never reads a register itself, so this never competes with
//   anything the Copper needs to see.
//
// - `chip_data` is the one shared "chip data bus" word (matching
//   minimig.v's custom_data_in, which fans out identically to every custom
//   register module) driving both modules' data_in every cycle.
//
// This rig performs no arbitration and injects no expected value: every
// signal the testbench does not drive is a plain wire between two unmodified
// RTL modules.

module rig_copper_intreq
(
    input             clk,
    input             clk7_en,
    input             reset,
    input             ecs,

    // Copper DMA handshake, driven by the testbench acting as Agnus's slot
    // arbiter for this reduced fixture.
    input             ackdma,
    input             enadma,
    input             sof,
    input             blit_busy,
    input      [7:0]  vpos,
    input      [8:0]  hpos,
    output            reqdma,
    output     [8:1]  copper_reg_address_out,
    output     [20:1] copper_address_out,

    // Shared chip data bus (custom_data_in in the real design).
    input      [15:0] chip_data,

    // Interrupt controller register-bus side, testbench-driven (see header).
    input      [8:1]  paula_reg_address_in,
    output     [15:0] paula_data_out,
    output     [2:0]  _ipl
);

// Real-boundary self-loop: the Copper only sees its own issued address back
// while it actually holds the bus (agnus.v:231-239).
wire [8:1] copper_reg_address_in = ackdma ? copper_reg_address_out : 8'hFF;

agnus_copper cop
(
    .clk            (clk),
    .clk7_en        (clk7_en),
    .reset          (reset),
    .ecs            (ecs),
    .reqdma         (reqdma),
    .ackdma         (ackdma),
    .enadma         (enadma),
    .sof            (sof),
    .blit_busy      (blit_busy),
    .vpos           (vpos),
    .hpos           (hpos),
    .data_in        (chip_data),
    .reg_address_in (copper_reg_address_in),
    .reg_address_out(copper_reg_address_out),
    .address_out    (copper_address_out)
);

paula_intcontroller pic
(
    .clk            (clk),
    .clk7_en        (clk7_en),
    .reset          (reset),
    .reg_address_in (paula_reg_address_in),
    .data_in        (chip_data),
    .data_out       (paula_data_out),
    .rxint          (1'b0),
    .txint          (1'b0),
    .vblint         (1'b0),
    .int2           (1'b0),
    .int3           (1'b0),
    .int6           (1'b0),
    .blckint        (1'b0),
    .syncint        (1'b0),
    .audint         (4'b0000),
    .audpen         (),
    .rbfmirror      (),
    ._ipl           (_ipl)
);

endmodule
