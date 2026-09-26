// disk_dma_data_channel_rig.v
//
// This package's own fixture wrapper -- NOT pinned RTL. It instantiates
// `paula_floppy` (+ `paula_floppy_fifo`, which `paula_floppy.v` itself
// instantiates) and `agnus_diskdma` (both $(NANOMIG)/src/minimig-aga/*.v,
// read-only, pinned submodule) with every one of their own ports brought
// straight out to this wrapper's top level. There is NO internal Verilog
// wiring between the two module instances below -- read after
// SPECIFICATION.md was committed (commit d03c70e), `paula_floppy` and
// `agnus_diskdma` are not directly wired to each other anywhere in the real
// chip either: paula.v connects paula_floppy's dmal/dmas outputs up to
// disk_dmal/disk_dmas, and agnus.v connects those into agnus_diskdma's own
// dmal/dmas inputs (agnus.v:326-340); the shared register-address/data bus
// that reaches paula_floppy's reg_address_in/data_in is agnus.v's own
// priority-arbitrated `reg_address` (agnus.v:169-263: disk DMA is the
// TOP-priority case, `if (dma_dsk) begin reg_address = reg_address_dsk;
// dbwe = wr_dsk; ... end`), fed from six DMA engines plus the CPU.
//
// Reproducing that whole arbiter for one package would mean building a
// third module this package was never assigned. Instead, the C++ testbench
// does the arbitration in software, one tick at a time, on the two ports
// that carry it: `reg_address_in`/`data_in` (this rig presents whatever the
// testbench pokes onto them, whether that is a CPU-style register write
// such as DSKLEN/DSKPTH/DSKPTL/DSKSYNC, or -- to model a granted disk-DMA
// cycle -- `reg_address_dsk`/a stimulus RAM word, exactly mirroring what
// agnus.v's arbiter would present on those same wires when `dma_dsk` wins)
// and `agnus_dmal_in`/`agnus_dmas_in` (fed either from paula_floppy's own
// `paula_dmal`/`paula_dmas` outputs, for assertions that exercise paula's
// real DSKLEN-armed DMA request, or from an independent testbench stimulus,
// for assertions that grade `agnus_diskdma`'s own slot-timing/address-select
// logic directly and are not about paula's arming sequence). Every signal
// named in the classify evidence for this package's claims (`busrd`,
// `buswr`, `fifo_in`, `fifo_out`, `dma`, `wr`, `reg_address_out`) is either
// a top-level port here or (`busrd`/`buswr`/`fifo_in`/`fifo_out`) already
// `/* verilator public */` inside paula_floppy.v itself and reachable
// straight off this rig's PAULA1 instance handle without any extra wiring.

module disk_dma_data_channel_rig
(
	input             clk,
	input             clk7_en,
	input             clk7n_en,
	input             reset,

	// paula_floppy's own system-bus side
	input             ntsc,
	input             sof,
	input             enable,
	input             wordsync,
	input      [8:1]  reg_address_in,   // shared bus: also agnus_diskdma's reg_address_in
	input      [15:0] data_in,          // shared bus: also agnus_diskdma's data_in
	output     [15:0] paula_data_out,
	output            paula_dmal,       // paula_floppy's own DMA-request output (dmal)
	output            paula_dmas,       // paula_floppy's own DMA-request output (dmas)

	// disk control signals
	input             _step,
	input             direc,
	input      [3:0]  _sel,
	input             side,
	input             _motor,
	output            _track0,
	output            _change,
	output            _ready,
	output            _wprot,
	output            index,

	output            blckint,
	output            syncint,

	output            fdd_led,
	input      [1:0]  floppy_drives,
	input             floppy_wrprot,

	// SD-card side
	input      [3:0]  sdc_image_mounted,
	input      [31:0] sdc_image_size,
	output     [3:0]  sdc_rd,
	output     [3:0]  sdc_wr,
	output     [31:0] sdc_sector,
	input             sdc_busy,
	input             sdc_done,
	input             sdc_byte_in_strobe,
	input      [8:0]  sdc_byte_addr,
	input      [7:0]  sdc_byte_in_data,
	output     [7:0]  sdc_byte_out_data,

	// agnus_diskdma's own side -- dmal/dmas driven directly by the
	// testbench (see file header): either copied from paula_dmal/paula_dmas
	// each tick, or an independent stimulus.
	output            dma,
	input             agnus_dmal_in,
	input             agnus_dmas_in,
	input             speed,
	input      [8:0]  hpos,
	output            wr,
	output     [8:1]  reg_address_dsk,
	output     [20:1] address_out
);

paula_floppy PAULA1
(
	.clk(clk),
	.clk7_en(clk7_en),
	.clk7n_en(clk7n_en),
	.reset(reset),
	.ntsc(ntsc),
	.sof(sof),
	.enable(enable),
	.reg_address_in(reg_address_in),
	.data_in(data_in),
	.data_out(paula_data_out),
	.dmal(paula_dmal),
	.dmas(paula_dmas),
	._step(_step),
	.direc(direc),
	._sel(_sel),
	.side(side),
	._motor(_motor),
	._track0(_track0),
	._change(_change),
	._ready(_ready),
	._wprot(_wprot),
	.index(index),
	.blckint(blckint),
	.syncint(syncint),
	.wordsync(wordsync),
	.fdd_led(fdd_led),
	.floppy_drives(floppy_drives),
	.floppy_wrprot(floppy_wrprot),
	.sdc_image_mounted(sdc_image_mounted),
	.sdc_image_size(sdc_image_size),
	.sdc_rd(sdc_rd),
	.sdc_wr(sdc_wr),
	.sdc_sector(sdc_sector),
	.sdc_busy(sdc_busy),
	.sdc_done(sdc_done),
	.sdc_byte_in_strobe(sdc_byte_in_strobe),
	.sdc_byte_addr(sdc_byte_addr),
	.sdc_byte_in_data(sdc_byte_in_data),
	.sdc_byte_out_data(sdc_byte_out_data)
);

agnus_diskdma AGNUS1
(
	.clk(clk),
	.clk7_en(clk7_en),
	.dma(dma),
	.dmal(agnus_dmal_in),
	.dmas(agnus_dmas_in),
	.speed(speed),
	.hpos(hpos),
	.wr(wr),
	.reg_address_in(reg_address_in),
	.reg_address_out(reg_address_dsk),
	.data_in(data_in),
	.address_out(address_out)
);

endmodule
