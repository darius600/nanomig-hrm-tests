; prog_i_w.s -- Program I, variant I-w (SPECIFICATION.md 3.5): the level-3
; handler adds 2 to HCNT, not 1. It still toggles COP1LC and clears COPER, so
; interrupts, the completion word and one HCNT write per field stay live.
; Contrasting case for precondition PI-5 (the value each write carries).
I_HCNTSTEP2     equ     1
                include "prog_i.s"
