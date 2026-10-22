// cia_disk_port_windows_rig.v
//
// This package's own fixture wrapper -- NOT pinned RTL. It instantiates
// `gary`, `ciaa` and `ciab` (all $(NANOMIG)/src/minimig-aga/*.v, read-only,
// pinned submodule) and wires them exactly the way minimig.v wires them,
// for the signals this package measures. Read in full, after
// SPECIFICATION.md was committed (9d3366c):
//   gary.v, ciaa.v, ciab.v, cia_int.v (cia_timera.v/cia_timerb.v/cia_timerd.v
//   were not read -- ciaa/ciab instantiate them for the timers, which none
//   of this package's assertions touch, the same scoping call
//   ch8-ciaapra-address-window made for the identical reason).
//   minimig.v lines 610-661 (ciaa CIAA1 / ciab CIAB1 instantiations) and
//   772-829 (gary GARY1 instantiation) -- the wiring this rig reproduces.
//
// THE JOINS THIS RIG MAKES, and the minimig.v line each one reproduces:
//   - gary.cpu_address_in <- this rig's cpu_address         (minimig.v:775)
//   - ciaa.aen <- gary.sel_cia_a                             (minimig.v:622)
//   - ciab.aen <- gary.sel_cia_b                             (minimig.v:647)
//   - ciaa.rs  <- cpu_address[11:8]                          (minimig.v:626)
//   - ciab.rs  <- cpu_address[11:8]                          (minimig.v:651)
//   - ciaa.data_in <- cpu_data_out[7:0]  (LOW byte lane)     (minimig.v:627)
//   - ciab.data_in <- cpu_data_out[15:8] (HIGH byte lane)    (minimig.v:652)
//   - ciaa.wr <- cpu_lwr | cpu_hwr                           (minimig.v:624)
//   - ciab.wr <- cpu_hwr | cpu_lwr                           (minimig.v:649)
//   - ciaa.rd <- cpu_rd, ciab.rd <- cpu_rd (same signal)     (minimig.v:623,648)
//   - ciaa.porta_in <- {_fire1,_fire0,_ready,_track0,_wprot,_change}
//         -- the six disk/joystick sense bits PROP-TWO-PORTS names
//                                                             (minimig.v:632)
//   - ciab.portb_out -> {_motor,_sel3,_sel2,_sel1,_sel0,side,direc,_step}
//         -- the eight disk control bits PROP-TWO-PORTS and
//            PROP-CIABPRB-WINDOW name                        (minimig.v:660)
//   - ciab.flag <- (this rig's ciab_flag input; minimig.v wires the real
//         disk index pulse `index` here) -- the mechanism
//         PROP-DSKINDEX-WINDOW's ICR bit rides on             (minimig.v:657)
// This rig does NOT reproduce the disk-signal *names* as internal wires
// (no `_ready`, `_motor`, etc. exist inside this file) -- it exposes the
// generic ciaa/ciab port positions those minimig.v lines connect to,
// exactly as ciaa_porta_in[7:2] and ciab_portb_out[7:0], the same
// restriction `ch8-ciabprb-disk-control-bits` documented for the identical
// reason: which bit carries which drive-control name is a minimig-level
// binding, not something either CIA module's own boundary can confirm.
//
// Why gary is needed (not ciaa/ciab alone): neither ciaa nor ciab has a CPU
// address bus -- each exposes only an internal register-select input (`rs`,
// 4 bits) and a chip-enable pin (`aen`) that something else must assert
// (ciab.v:8,12 -- ciab has no address bus, only `aen` and `rs[3:0]`, cited
// by opus:8-cia.c5's own evidence). Every claim in this package names a CPU
// bus address ($BFD100, $BFDD00), so the fixture must include whatever
// decodes a CPU address into aen/rs for both chips. That is gary:
//   gary.v:190  assign sel_cia   = cpu_address_in[23:16]==8'hBF;  // $BFxxxx
//   gary.v:191  assign sel_cia_a = sel_cia & ~cpu_address_in[12]; // one bit
//   gary.v:192  assign sel_cia_b = sel_cia & ~cpu_address_in[13]; // one bit
// Confirmed by reading gary.v in full: these are the whole decode, each a
// single combinational `assign` depending only on `cpu_address_in`. Neither
// line reads the other's tested bit, and bits 15/14 (inside the fully-
// compared $BFxxxx byte) appear in neither line -- which is the mechanism
// PROP-CIABPRB-WINDOW / PROP-DSKINDEX-WINDOW / OVERLAP-BOTH-SELECTS-ACTIVE
// measure, exactly as the build task and finding-candidate C12 said before
// this file was opened.
//
// Port list below is a subset of gary's own ports: gary ports with no path
// to sel_cia/sel_cia_a/sel_cia_b (dma_address_in, the RAM/custom data
// buses, a1k, bootrom, cpu_hlt, ovl, dbr, dbwe, memory_config, ecs,
// hdc_ena, and every sel_* output except the three named above) are left
// unconnected (this package's Makefile carries -Wno-pinmissing, the same
// flag hrm/ch8-ciaapra-address-window/Makefile uses, for the same reason).
// Confirmed by reading gary.v in full: sel_cia, sel_cia_a and sel_cia_b
// are plain `assign` statements depending only on cpu_address_in -- none of
// the omitted ports reaches them. Every port of ciaa and ciab is connected;
// both are under test.

module cia_disk_port_windows_rig
(
	input             clk,
	input             clk7_en,
	input             clk7n_en,
	input             reset,

	// CPU-bus side, wired the way minimig.v wires cpu_address_out into
	// gary (as cpu_address_in) and into ciaa/ciab (as rs = address[11:8]).
	input      [23:1] cpu_address,
	input             cpu_rd,
	input             cpu_hwr,
	input             cpu_lwr,
	// [7:0] -> ciaa.data_in (minimig.v:627), [15:8] -> ciab.data_in (minimig.v:652)
	input      [15:0] cpu_data_out,
	output     [7:0]  cia_a_data_out,  // ciaa's own data_out (minimig.v:628 lands this on cia_data_out[7:0])
	output     [7:0]  cia_b_data_out,  // ciab's own data_out (minimig.v:653 lands this on cia_data_out[15:8])

	// gary's decode outputs, exposed so the precondition and the overlap
	// fact are asserted directly rather than inferred from data content.
	output            sel_cia,
	output            sel_cia_a,
	output            sel_cia_b,

	// ciaa's own remaining ports (minimig.v:617-640)
	input      [7:2]  ciaa_porta_in,   // minimig.v:632 -- disk sense + joystick fire bits
	output     [3:0]  ciaa_porta_out,
	input      [7:0]  ciaa_portb_in,
	input             ciaa_tick,
	input             ciaa_eclk,
	output            ciaa_irq,
	input             kms_level,
	input      [1:0]  kbd_mouse_type,
	input      [7:0]  kbd_mouse_data,
	output            freeze,
	input             hrtmon_en,

	// ciab's own remaining ports (minimig.v:643-661)
	input      [5:0]  ciab_porta_in,
	output     [7:6]  ciab_porta_out,
	output     [7:0]  ciab_portb_out,  // minimig.v:660 -- disk control bits
	input             ciab_tick,
	input             ciab_eclk,
	input             ciab_flag,       // minimig.v:657 -- DSKINDEX*'s mechanism
	output            ciab_irq
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
	.data_out(cia_a_data_out),
	.tick(ciaa_tick),
	.eclk(ciaa_eclk),
	.irq(ciaa_irq),
	.porta_in(ciaa_porta_in),
	.porta_out(ciaa_porta_out),
	.portb_in(ciaa_portb_in),
	.kms_level(kms_level),
	.kbd_mouse_type(kbd_mouse_type),
	.kbd_mouse_data(kbd_mouse_data),
	.freeze(freeze),
	.hrtmon_en(hrtmon_en)
);

ciab CIAB1
(
	.clk(clk),
	.clk7_en(clk7_en),
	.aen(sel_cia_b),
	.rd(cpu_rd),
	.wr(cpu_hwr | cpu_lwr),
	.reset(reset),
	.rs(cpu_address[11:8]),
	.data_in(cpu_data_out[15:8]),
	.data_out(cia_b_data_out),
	.tick(ciab_tick),
	.eclk(ciab_eclk),
	.flag(ciab_flag),
	.irq(ciab_irq),
	.porta_in(ciab_porta_in),
	.porta_out(ciab_porta_out),
	.portb_out(ciab_portb_out)
);

endmodule
