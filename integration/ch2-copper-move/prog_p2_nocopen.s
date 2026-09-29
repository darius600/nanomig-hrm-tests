; prog_p2_nocopen.s -- G1's own baseline for P2 (NMH_MUTATE none; always
; run): DMACON started $8200 (no COPEN) instead of $8280, "a baseline run
; of the same program" (SPECIFICATION.md 4.5). See prog_p2.s.

P2_NOCOPEN      equ     1
        include "prog_p2.s"
