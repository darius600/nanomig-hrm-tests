; prog_p8_cop_mut_b.s -- P8 COP case, precondition (b) mutation: WAIT V=$20 and
; no clear, so bit 4 reads 1 at the first record after MC. See
; prog_p8_cop_base.s.
P8_COP_MUT_B    equ     1
        include "prog_p8_cop_base.s"
