module agnus_blitter_zero (
    input  wire        clk,
    input  wire        clk7_en,
    input  wire        enable,
    input  wire        init,
    input  wire        store_result,
    input  wire [15:0] fill_out,
    output reg         zero
);
// Extracted from agnus_blitter.v
always @(posedge clk)
  if (clk7_en) begin
  	if (enable && init)
  		zero <= 1;
  	else if (store_result && |fill_out[15:0])
  		zero <= 0;
  end
endmodule
