; prog_p1_k3.s -- prog_p1.s with K3 set: every Fc's R step waits for line
; 12 instead of line 10. See SPECIFICATION.md section 6, K3 (PC1 mutation).

K3      equ     1
        include "prog_p1.s"
