`timescale 1ns / 1ns

module ide_dpram (
    input clka,
    input reseta,
    input cea,
    input [11:0] ada,
    input wrea,
    input [15:0] dina,
    input ocea,
    output reg [15:0] douta,

    input clkb,
    input resetb,
    input ceb,
    input [11:0] adb,
    input wreb,
    input [15:0] dinb,
    input oceb,
    output reg [15:0] doutb
);

    reg [15:0] mem [0:4095];

    always @(posedge clka) begin
        if (cea) begin
            if (wrea) mem[ada] <= dina;
            douta <= mem[ada];
        end
    end

    always @(posedge clkb) begin
        if (ceb) begin
            if (wreb) mem[adb] <= dinb;
            doutb <= mem[adb];
        end
    end

endmodule
