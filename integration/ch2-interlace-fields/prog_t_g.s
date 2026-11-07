; prog_t_g.s -- Program T, variant T-g (SPECIFICATION.md 4.4): the WAIT of
; line 10 uses H=$40, so its mark lies early in line 10 and the lengths of
; lines 9 and 10 change. A gating observation of PT-5's marks (PT-5(mark)),
; not a contrasting case: it moves a measuring mark and no physical line.
T_LINE10_40     equ     1
                include "prog_t.s"
