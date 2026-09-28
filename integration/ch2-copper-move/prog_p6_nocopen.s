; prog_p6_nocopen.s -- G1's own baseline for P6 (NMH_MUTATE none; always
; run): DMACON started $8200 (no COPEN) instead of $8280. See prog_p6.s.

P6_NOCOPEN      equ     1
        include "prog_p6.s"
