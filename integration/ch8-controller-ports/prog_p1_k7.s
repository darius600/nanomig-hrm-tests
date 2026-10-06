; prog_p1_k7.s -- prog_p1.s with K7 set: every W step stores its M1 marker
; after waiting for line 9 instead of right after the POTGO write at line 4.
; The POTGO write, the V reads and every R step are unchanged. See
; SPECIFICATION.md section 6, K7 (PC4 mutation).

K7      equ     1
        include "prog_p1.s"
