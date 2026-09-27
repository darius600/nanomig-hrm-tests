; prog_p1_k4.s -- prog_p1.s with K4 set: Phase P's F1 write is $AA00
; instead of $FF00. See SPECIFICATION.md section 6, K4 (PC2 mutation).

K4      equ     1
        include "prog_p1.s"
