; prog_h_h1c_aud0.s -- A2 [H1c] mutation: H1c (N=$00B0, B=$0820,
; VARBEAMEN=0) with audio channel 0 DMA started, as prog_h_h1_aud0.s. The
; four-refresh-slot count (p.194, p.285) is fixed by the chipset's slot
; allocation, not by VARBEAMEN, so the same audio stimulus applies here
; too and must fail A2 [H1c].
N               equ     $00B0
B               equ     $0820
AUD0_ON         equ     1
        include "prog_h.s"
