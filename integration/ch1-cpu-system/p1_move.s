; p1_move.s -- p1.s with MUT_MOVE set: P1's own contrasting case (P1-MOVE,
; SPECIFICATION.md section 3): every CLR.x replaced by MOVE.x #0.
MUT_MOVE        equ     1
        include "p1.s"
