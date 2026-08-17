module agnus_blitter_line_octant (
    input [4:2] bltcon1, // SUD, SUL, AUL
    input sign_del,
    input [3:0] ash,
    output lineinc,
    output linedec,
    output lineadd,
    output linesub
);

wire   lineinc_w = (bltcon1[4] && !bltcon1[2] || !bltcon1[4] && !bltcon1[3] && !sign_del) && ash==4'b1111 ? 1'b1 : 1'b0;
wire   linedec_w = (bltcon1[4] &&  bltcon1[2] || !bltcon1[4] &&  bltcon1[3] && !sign_del) && ash==4'b0000 ? 1'b1 : 1'b0;
wire   lineadd_w = !bltcon1[4] && !bltcon1[2] ||  bltcon1[4] && !bltcon1[3] && !sign_del ? 1'b1 : 1'b0;
wire   linesub_w = !bltcon1[4] &&  bltcon1[2] ||  bltcon1[4] &&  bltcon1[3] && !sign_del ? 1'b1 : 1'b0;

assign lineinc = lineinc_w;
assign linedec = linedec_w;
assign lineadd = lineadd_w;
assign linesub = linesub_w;

endmodule
