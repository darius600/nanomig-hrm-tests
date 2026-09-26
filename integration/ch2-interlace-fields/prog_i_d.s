; prog_i_d.s -- Program I, variant I-d (SPECIFICATION.md 3.5): the handler
; omits the COP1LC rewrite (it still counts and clears). Contrasting case
; for P4 (d)/(e).
I_NOTOGGLE      equ     1
                include "prog_i.s"
