; prog_p5_c2.s -- contrasting case C2 (NMH_MUTATE=P5-C2): one extra step
; directly after the phase-1 step to $0C8000 -- MOVE.W #$4000,$DFF082
; only, then a marker and COPJMP1. See prog_p5.s.

P5_C2           equ     1
        include "prog_p5.s"
