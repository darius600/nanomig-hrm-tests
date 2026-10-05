; prog_i_o.s -- Program I, variant I-o (SPECIFICATION.md 3.5): LACE = 0 and
; M1 waits between samples. The LACE-clear control run with an invalid M1,
; contrasting case for precondition PI-9.
I_NOLACE        equ     1
M1_DELAY        equ     3000
                include "prog_i.s"
