; prog_p7_noaud0.s -- G1's own baseline for P7 (NMH_MUTATE none; always
; run): DMACON started $8200 (no AUD0EN) instead of $8201. See prog_p7.s.

P7_NOAUD0       equ     1
        include "prog_p7.s"
