; prog_i_t.s -- Program I, variant I-t (SPECIFICATION.md 3.5): in both lists
; the two BPL1PT MOVEs are placed after WAIT $2001,$FF00 (line 32), in front
; of the first bitplane fetch (line 44) but after the end of vertical blanking.
; Contrasting case for precondition PI-12(bpl).
I_MIDPTR        equ     1
                include "prog_i.s"
