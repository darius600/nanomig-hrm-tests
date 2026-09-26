; p3_mutnop.s -- p3.s with MUT_NOP set: P3.a's and P3.b's shared contrast
; (SPECIFICATION.md section 5): block 1's BRA.S is replaced by NOP, so
; execution falls through into the guard (S_X written, then FAILRUN).
MUT_NOP         equ     1
        include "p3.s"
