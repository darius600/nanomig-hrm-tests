; prog_i_m.s -- Program I, variant I-m (SPECIFICATION.md 3.5): in both
; lists the two SPR0PT MOVEs are placed after WAIT $2D01,$FF00.
; Contrasting case for precondition PI-7(spr).
I_LATESPR       equ     1
                include "prog_i.s"
