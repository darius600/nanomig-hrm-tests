; prog_t_d.s -- Program T, variant T-d (SPECIFICATION.md 4.4): the mark of
; line 100 uses WAIT (100,$C0), not (100,$E0). Contrasting case for
; precondition PT-3.
T_LINE100_C0    equ     1
                include "prog_t.s"
