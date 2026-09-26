; p1_mutc.s -- p1.s with MUT_C set: P1.c's contrast (SPECIFICATION.md
; section 3): case 11 is assembled as CLR.L while the case table still
; names CLR.W.
MUT_C           equ     1
        include "p1.s"
