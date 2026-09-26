; prog_i_c.s -- Program I, variant I-c (SPECIFICATION.md 3.5): both lists
; omit the two BPL1PT MOVEs; the CPU still writes BPL1PT = PIC once.
; Contrasting case for P5.
I_NOPTRLIST     equ     1
                include "prog_i.s"
