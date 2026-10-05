; prog_t_g.s -- Program T, variant T-g (SPECIFICATION.md 4.4): the WAIT of
; line 10 uses H=$40, so its mark lies early in line 10 and the lengths of
; lines 9 and 10 change. Contrasting case for precondition PT-5.
T_LINE10_40     equ     1
                include "prog_t.s"
