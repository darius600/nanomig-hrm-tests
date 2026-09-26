; prog_i_e.s -- Program I, variant I-e (SPECIFICATION.md 3.5): both lists
; omit the two SPR0PT MOVEs; the CPU still writes SPR0PT = SPR once.
; Contrasting case for P6.
I_NOSPRLIST     equ     1
                include "prog_i.s"
