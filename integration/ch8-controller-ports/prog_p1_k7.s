; prog_p1_k7.s -- prog_p1.s with K7 set: after waiting for line 9, every W step
; drives every pot line to the other level, writes its own POTGO value again
; and stores its M1 marker after that. The first POTGO write at line 4, its V reads and every R
; step are unchanged. See SPECIFICATION.md section 6, K7 (PC4 mutation).

K7      equ     1
        include "prog_p1.s"
