; prog_i_x.s -- Program I, variant I-x (SPECIFICATION.md 3.5): the program
; writes $0007 to HCNT before it installs the handler, not 0. Every later
; write of the handler still adds 1 to the previous value. Contrasting case
; for precondition PI-13 (HCNT starts from a known value).
I_HCNTSEED      equ     1
                include "prog_i.s"
