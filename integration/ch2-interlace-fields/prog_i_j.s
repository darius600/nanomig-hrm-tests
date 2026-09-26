; prog_i_j.s -- Program I, variant I-j (SPECIFICATION.md 3.5): M1 waits
; between samples (m1.i). Contrasting case for precondition PI-1. See
; m1.i's own header comment for why the exact count is not a page value.
M1_DELAY        equ     3000
                include "prog_i.s"
