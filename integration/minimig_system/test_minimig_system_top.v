`timescale 1ns / 1ns

module test_minimig_system_top(
    input clk_28,
    input reset_n,

    // CPU interface driven by testbench
    input [23:1] cpu_address,
    output [15:0] cpu_data_out,
    input [15:0] cpu_data_in,
    output [2:0] _cpu_ipl,
    input _cpu_as,
    input _cpu_uds,
    input _cpu_lds,
    input cpu_r_w,
    output _cpu_dtack,
    output _cpu_reset,

    // SRAM interface (connected to testbench dummy RAM)
    output [15:0] ram_data_out,
    input [15:0] ram_data_in,
    output [23:1] ram_address,
    output _ram_bhe,
    output _ram_ble,
    output _ram_we,
    output _ram_oe,

    // Video out for verification
    output [7:0] video_r,
    output [7:0] video_g,
    output [7:0] video_b,
    output hsync,
    output vsync
);

    wire clk7_en;
    wire clk7n_en;
    wire c1;
    wire c3;
    wire cck;
    wire [9:0] eclk;

    amiga_clk clk_gen(
        .clk_28(clk_28),
        .reset_n(reset_n),
        .clk7_en(clk7_en),
        .clk7n_en(clk7n_en),
        .c1(c1),
        .c3(c3),
        .cck(cck),
        .eclk(eclk)
    );

    minimig sys(
        .cpu_address(cpu_address),
        .cpu_data(cpu_data_out),
        .cpudata_in(cpu_data_in),
        ._cpu_ipl(_cpu_ipl),
        ._cpu_as(_cpu_as),
        ._cpu_uds(_cpu_uds),
        ._cpu_lds(_cpu_lds),
        .cpu_r_w(cpu_r_w),
        ._cpu_dtack(_cpu_dtack),
        ._cpu_reset(_cpu_reset),
        ._cpu_reset_in(reset_n), // m68k reset in
        .nmi_addr(32'h0),
        .ovr(),
        .ovl(),

        .ram_data(ram_data_out),
        .ramdata_in(ram_data_in),
        .ram_address(ram_address),
        ._ram_bhe(_ram_bhe),
        ._ram_ble(_ram_ble),
        ._ram_we(_ram_we),
        ._ram_oe(_ram_oe),
        .chip48(48'h0),
        .refresh(),

        .rst_ext(~reset_n),
        .rst_out(),
        .clk(clk_28),
        .clk7_en(clk7_en),
        .clk7n_en(clk7n_en),
        .c1(c1),
        .c3(c3),
        .cck(cck),
        .eclk(eclk),

        .rxd(1'b1),
        .txd(),
        .cts(1'b1),
        .rts(),
        .dtr(),
        .dsr(1'b1),
        .cd(1'b1),
        .ri(1'b1),

        ._joy1(16'hFFFF),
        ._joy2(16'hFFFF),
        ._joy3(16'hFFFF),
        ._joy4(16'hFFFF),
        .joya1(16'hFFFF),
        .joya2(16'hFFFF),
        .mouse_btn(3'h7),
        .kms_level(1'b0),
        .kbd_mouse_type(2'b0),
        .kbd_mouse_data(8'b0),

        .fdd_led(),
        .memory_config(8'b0), // Will this default be OK? Usually 8'b00 means 512k chip or 2M chip. Let's see. Minimig syscontrol defaults to sensible if 0?
        .chipset_config(6'b0), // OCS / ECS?
        .floppy_config(4'b0),
        .ide_config(6'b0),
        .hdd_led(),
        
        .sdc_img_mounted(4'b0),
        .sdc_img_size(32'h0),
        .sdc_rd(),
        .sdc_wr(),
        .sdc_sector(),
        .sdc_busy(1'b0),
        .sdc_done(1'b0),
        .sdc_byte_in_strobe(1'b0),
        .sdc_byte_addr(9'h0),
        .sdc_byte_in_data(8'h0),
        .sdc_byte_out_data(),

        ._hsync(hsync),
        ._vsync(vsync),
        ._csync(),
        .field1(),
        .lace(),
        .hblank(),
        .vblank(),
        .red(video_r),
        .green(video_g),
        .blue(video_b),
        .ce_pix(),
        .res(),
        .htotal(),

        .ldata(),
        .rdata(),
        .ldata_okk(),
        .rdata_okk(),
        
        .ide_ena(),
        .ide_fast(),
        .ide_ext_irq(1'b0),
        .ide_req(),
        .ide_address(5'b0),
        .ide_write(1'b0),
        .ide_writedata(16'b0),
        .ide_read(1'b0),
        .ide_readdata(),

        // cachecfg and memcfg were connected here until the source state of
        // 07.09.2026. Upstream removed both from minimig.v's port list: the
        // module now decides its cache configuration itself (new: cpu_cache_new.v),
        // and memcfg was an output that stayed unconnected here anyway. cachecfg
        // was tied to 3'b0, so "off"; what the module now determines internally
        // is no longer set from here. This test exercises the Copper's access to
        // the register bus and does not touch the cache; it produced the same
        // result after the change.
        .cpucfg(2'b0)
    );

endmodule
