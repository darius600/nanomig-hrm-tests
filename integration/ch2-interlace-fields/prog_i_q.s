; prog_i_q.s -- Program I, variant I-q (SPECIFICATION.md 3.5): both lists
; omit the two BPL1PT MOVEs (as I-c) and M1 waits between samples. The
; no-rewrite run with an invalid M1, contrasting case for precondition PI-10.
I_NOPTRLIST     equ     1
M1_DELAY        equ     3000
                include "prog_i.s"
