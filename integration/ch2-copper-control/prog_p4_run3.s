; prog_p4_run3.s -- P4 Run 3 (strobe, then rewrite): COP1LC=A, COP2LC=B,
; COPJMP2, COP2LC=C (no strobe). See prog_p4_base.s.
P4_RUN2         equ     1
P4_RUN3         equ     1
        include "prog_p4_base.s"
