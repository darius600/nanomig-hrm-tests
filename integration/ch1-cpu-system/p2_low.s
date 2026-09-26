; p2_low.s -- p2.s with MUT_P2LOW set: P2's own contrasting case (P2-LOW,
; SPECIFICATION.md section 4): every register loaded with its pattern's
; lower half, zero-extended.
MUT_P2LOW       equ     1
        include "p2.s"
