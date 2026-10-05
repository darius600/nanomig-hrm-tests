; prog_p1_off_mut_a.s -- P1 contrasting case, precondition (a) mutation: DMACON
; = $8280 written before COP1LC/COPJMP1, then COPEN cleared at step 5.
; See prog_p1_base.s.
P1_BIT7_OFF     equ     1
P1_MUT_EARLY_ON equ     1
        include "prog_p1_base.s"
