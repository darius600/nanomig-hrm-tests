; prog_i_u.s -- Program I, variant I-u (SPECIFICATION.md 3.5): in both lists
; the two SPR0PT MOVEs are placed after WAIT $1901,$FF00 (line 25): outside
; the 25 lines of vertical blanking p219 guarantees, and still in front of the
; sprite's first fetch. Contrasting case for precondition PI-12(spr).
I_MIDSPR        equ     1
                include "prog_i.s"
