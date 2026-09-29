; prog_h_h2c_aud0.s -- A2 [H2c] mutation: H2c (N=$00C0, B=$0820,
; VARBEAMEN=0) with audio channel 0 DMA started, as prog_h_h1_aud0.s. Must
; fail A2 [H2c], as prog_h_h1c_aud0.s.
N               equ     $00C0
B               equ     $0820
AUD0_ON         equ     1
        include "prog_h.s"
