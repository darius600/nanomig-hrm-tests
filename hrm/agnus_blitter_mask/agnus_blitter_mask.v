module agnus_blitter_mask (
    input  wire        first_word,
    input  wire        last_word,
    input  wire [15:0] bltafwm,
    input  wire [15:0] bltalwm,
    input  wire [15:0] bltadat,
    output wire [15:0] mask_out,
    output wire [15:0] data_out
);

reg [15:0] bltamask;

// Extracted from agnus_blitter.v lines 310-319
always @(*)
	if (first_word && last_word)
		bltamask[15:0] = bltafwm[15:0] & bltalwm[15:0];
	else if (last_word)
		bltamask[15:0] = bltalwm[15:0];
	else if (first_word)
		bltamask[15:0] = bltafwm[15:0];	
	else
		bltamask[15:0] = 16'hFF_FF;

assign mask_out = bltamask;
assign data_out = bltadat & bltamask;

endmodule
