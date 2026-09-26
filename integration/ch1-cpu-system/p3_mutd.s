; p3_mutd.s -- p3.s with MUT_D set: P3.d's contrast (SPECIFICATION.md
; section 5): block 1 is assembled with BRA.W instead of BRA.S, while the
; block table still names BRA.S, so the opcode read at X_1 differs.
MUT_D           equ     1
        include "p3.s"
