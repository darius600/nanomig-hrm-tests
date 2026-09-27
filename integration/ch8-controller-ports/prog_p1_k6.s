; prog_p1_k6.s -- prog_p1.s with K6 set: every Fc's R step waits for line
; 5 instead of line 10. See SPECIFICATION.md section 6, K6 (PC4 mutation).

K6      equ     1
        include "prog_p1.s"
