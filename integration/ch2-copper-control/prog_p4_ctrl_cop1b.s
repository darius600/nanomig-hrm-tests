; prog_p4_ctrl_cop1b.s -- P4 comparator contrast: COP1LC = B, COPJMP1
; (p.25 predicts the first fetch is B's IR1, not A's). See prog_p4_base.s.
P4_CTRL_COP1B   equ     1
        include "prog_p4_base.s"
