; prog_h_h2_burst.s -- X-BURST mutation for H2 (N=$00C0, B=$08A0): same
; burst-mode sampling as prog_h_h1_burst.s, for P1-COVER [H2].
N               equ     $00C0
B               equ     $08A0
BURST_MODE      equ     1
        include "prog_h.s"
