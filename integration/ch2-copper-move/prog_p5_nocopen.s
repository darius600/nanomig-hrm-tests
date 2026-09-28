; prog_p5_nocopen.s -- G1's own baseline for P5 (NMH_MUTATE none; always
; run): DMACON started $8200 (no COPEN) instead of $8280. See prog_p5.s.

P5_NOCOPEN      equ     1
        include "prog_p5.s"
