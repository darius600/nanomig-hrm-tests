; prog_p1_k7.s -- prog_p1.s with K7 set: every W step repeats its POTGO write,
; with the same value, after waiting for line 9, and stores its M1 marker after
; that second write. The first POTGO write at line 4, its V reads and every R
; step are unchanged. See SPECIFICATION.md section 6, K7 (PC4 mutation).

K7      equ     1
        include "prog_p1.s"
