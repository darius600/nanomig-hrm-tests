// audio_dma_first_pass_rig.v
//
// This package's own fixture wrapper -- NOT pinned RTL. It instantiates
// `paula_audio` (which itself instantiates `paula_audio_channel` for each
// of the four channels, plus `paula_audio_mixer` and `paula_audio_volume`)
// and `agnus_audiodma` (both $(NANOMIG)/src/minimig-aga/*.v, read-only,
// pinned submodule), and wires them together with a small register-bus mux
// and a small combinational RAM, both entirely this package's own fixture
// -- neither is pinned RTL, and neither carries an expected value of its
// own; SPECIFICATION.md's own "Fixture" section states the wiring this
// file implements.
//
// Register bus: on a cycle where AGNUS1's own `dma` output is 1, the shared
// bus PAULA1 sees (`reg_address_in`/`data_in`) carries AGNUS1's own
// `reg_address_out` together with this file's RAM content at AGNUS1's own
// `address_out`; on every other cycle it carries this package's harness
// inputs (`cpu_reg_address`/`cpu_data`, idle address 8'hFF when the harness
// is not writing). AGNUS1's OWN `reg_address_in` always carries the
// harness's inputs directly, never the DMA-muxed value -- during a granted
// DMA cycle AGNUS1 is itself the one producing `reg_address_out`, so its
// own address input would otherwise read back its own output.
//
// RAM: addressed by AGNUS1's own `address_out` (a word address), holding
// the priming set P at word address $4000 and the graded data set D (with
// its sentinel words) at word address $8000, sized by this run's own
// `sim_len` input -- the exact content SPECIFICATION.md's "RAM (words)"
// section gives. Any other address reads a fixed fill value; the content
// there is not part of any graded data set.

module audio_dma_first_pass_rig
(
    input             clk,
    input             clk7_en,
    input             cck,
    input             strhor,
    input      [8:0]  hpos,
    input             rst,

    input             en,    // audio channel 0 DMA enable (DMACON bit, this fixture's own input)
    input             pend,  // audio channel 0 interrupt pending (this fixture's own input)

    input      [8:1]  cpu_reg_address,  // harness register-write address; 8'hFF when idle
    input      [15:0] cpu_data,         // harness register-write data

    input      [15:0] sim_len,          // this run's own AUDxLEN, for the RAM's data-set/sentinel split

    output     [20:1] address_out,      // AGNUS1's own fetch address -- the graded observable
    output            dma,              // AGNUS1's own "fetch cycle now" flag -- the graded observable
    output            dmal0,            // PAULA1's own dmal[0] -- fixture-liveness only
    output            dmas0             // PAULA1's own dmas[0] -- fixture-liveness only
);

// Word addresses (address_out is already a word address): D sits at byte
// $10000 = word $8000; P sits at byte $08000 = word $4000. Both per
// SPECIFICATION.md's own "RAM (words)" section.
localparam [20:1] D_BASE = 20'h08000;
localparam [20:1] P_BASE = 20'h04000;

wire [20:1] sim_len_ext = {4'b0, sim_len};

reg [15:0] ram_data;
always @(*) begin
    if (address_out == P_BASE)
        ram_data = 16'hAAAA;
    else if (address_out == P_BASE + 20'd1)
        ram_data = 16'hBBBB;
    else if (address_out >= D_BASE && (address_out - D_BASE) < sim_len_ext)
        ram_data = 16'h1050 + 16'h0101 * (address_out - D_BASE);
    else if (address_out >= D_BASE && (address_out - D_BASE) < (sim_len_ext + 20'd16))
        ram_data = 16'hEE00 + (address_out - D_BASE - sim_len_ext);
    else
        ram_data = 16'hDEAD;
end

wire [8:1]  paula_reg_address_in = dma ? agnus_reg_address_out : cpu_reg_address;
wire [15:0] paula_data_in        = dma ? ram_data              : cpu_data;

wire [8:1] agnus_reg_address_out;
wire [3:0] paula_dmal;
wire [3:0] paula_dmas;

paula_audio PAULA1
(
    .clk            (clk),
    .clk7_en        (clk7_en),
    .cck            (cck),
    .rst            (rst),
    .strhor         (strhor),
    .reg_address_in (paula_reg_address_in),
    .data_in        (paula_data_in),
    .dmaena         ({3'b0, en}),
    .audint         (),
    .audpen         ({3'b0, pend}),
    .dmal           (paula_dmal),
    .dmas           (paula_dmas),
    .ldata          (),
    .rdata          (),
    .ldata_okk      (),
    .rdata_okk      ()
);

assign dmal0 = paula_dmal[0];
assign dmas0 = paula_dmas[0];

agnus_audiodma AGNUS1
(
    .clk             (clk),
    .clk7_en         (clk7_en),
    .dma             (dma),
    .audio_dmal      (paula_dmal),
    .audio_dmas      (paula_dmas),
    .hpos            (hpos),
    .reg_address_in  (cpu_reg_address),
    .reg_address_out (agnus_reg_address_out),
    .data_in         (cpu_data),
    .address_out     (address_out)
);

endmodule
