; prog_i_r.s -- Program I, variant I-r (SPECIFICATION.md 3.5): both lists
; omit the two SPR0PT MOVEs (as I-e) and M1 waits between samples. The
; no-rewrite run with an invalid M1, contrasting case for precondition PI-11.
I_NOSPRLIST     equ     1
M1_DELAY        equ     3000
                include "prog_i.s"
