// ciaapra_address_rig.v
//
// This package's own fixture wrapper -- NOT pinned RTL. It instantiates
// `ciaa` and `gary` (both $(NANOMIG)/src/minimig-aga/*.v, read-only, pinned
// submodule) and wires them exactly the way minimig.v wires them for the
// signals this package measures. The wiring was read from
// $(NANOMIG)/src/minimig-aga/minimig.v (lines 607-630 for the ciaa
// instantiation, 762-821 for gary's, both citing cpu_address_out as the
// address both modules see) only after SPECIFICATION.md was committed
// (commit bd0970f).
//
// Why ciaa alone cannot be the fixture: ciaa.v's own interface is an
// internal register-select input (`rs`, 4 bits) with an address-enable pin
// (`aen`) that something else must assert -- ciaa.v has no CPU address bus
// and no knowledge of $BFE001. The claims under test are address claims
// ("CIAAPRA is at $BFE001"), not register-index claims, so the fixture must
// include whatever decodes a CPU address into ciaa's aen/rs. That is gary:
//   gary.v:190  assign sel_cia   = cpu_address_in[23:16]==8'hBF;      // $BFxxxx
//   gary.v:191  assign sel_cia_a = sel_cia & ~cpu_address_in[12];     // CIA A half
//   gary.v:192  assign sel_cia_b = sel_cia & ~cpu_address_in[13];     // CIA B half
// gary.v takes the full CPU address bus directly as `cpu_address_in` (no
// third module pre-resolves any part of it) -- so a ciaa+gary rig reaches
// the literal value $BFE001, and no wider boundary is needed.
//
// Port list below is a subset of gary's and ciaa's own ports: gary ports
// with no path to sel_cia/sel_cia_a/sel_cia_b (dma_address_in, the RAM/
// custom data buses, a1k, bootrom, cpu_hlt, ovl, dbr, dbwe, memory_config,
// ecs, hdc_ena, and every sel_* output except the three named above) are
// omitted from the instantiation below and left at Verilator's default
// (this package's Makefile carries -Wno-pinmissing, the same flag
// hrm/ch8-ciaa-port-direction/Makefile uses). Confirmed by reading gary.v
// in full: sel_cia, sel_cia_a and sel_cia_b are plain `assign` statements
// depending only on cpu_address_in -- none of the omitted ports reaches
// them, so this omission does not touch anything this package measures.
// Every ciaa.v port is connected; ciaa is the module under test.

module ciaapra_address_rig
(
	input             clk,
	input             clk7_en,
	input             clk7n_en,
	input             reset,

	// CPU-bus side, wired the way minimig.v wires cpu_address_out into
	// both gary (as cpu_address_in) and ciaa (as rs = address[11:8]).
	input      [23:1] cpu_address,
	input             cpu_rd,
	input             cpu_hwr,
	input             cpu_lwr,
	input      [15:0] cpu_data_out,
	output     [7:0]  cia_data_out,

	// ciaa's own remaining ports, unrelated to address decode, exposed so
	// the fixture can prove its data path is live (porta_in) without
	// touching anything outside ciaa+gary.
	input      [7:2]  porta_in,
	output     [3:0]  porta_out,
	input      [7:0]  portb_in,
	input             tick,
	input             eclk,
	output            irq,
	input             kms_level,
	input      [1:0]  kbd_mouse_type,
	input      [7:0]  kbd_mouse_data,
	output            freeze,
	input             hrtmon_en,

	// gary's decode outputs, exposed so the precondition (chip actually
	// selected) can be asserted instead of assumed.
	output            sel_cia,
	output            sel_cia_a,
	output            sel_cia_b
);

gary GARY1
(
	.cpu_address_in(cpu_address),
	.cpu_rd(cpu_rd),
	.cpu_hwr(cpu_hwr),
	.cpu_lwr(cpu_lwr),
	.clk(clk),
	.reset(reset),
	.sel_cia(sel_cia),
	.sel_cia_a(sel_cia_a),
	.sel_cia_b(sel_cia_b)
);

ciaa CIAA1
(
	.clk(clk),
	.clk7_en(clk7_en),
	.clk7n_en(clk7n_en),
	.aen(sel_cia_a),
	.rd(cpu_rd),
	.wr(cpu_lwr | cpu_hwr),
	.reset(reset),
	.rs(cpu_address[11:8]),
	.data_in(cpu_data_out[7:0]),
	.data_out(cia_data_out),
	.tick(tick),
	.eclk(eclk),
	.irq(irq),
	.porta_in(porta_in),
	.porta_out(porta_out),
	.portb_in(portb_in),
	.kms_level(kms_level),
	.kbd_mouse_type(kbd_mouse_type),
	.kbd_mouse_data(kbd_mouse_data),
	.freeze(freeze),
	.hrtmon_en(hrtmon_en)
);

endmodule
