; prog_p8_cop_mut_a.s -- P8 COP case, precondition (a) mutation: INTENA =
; $C020 with a level-3 handler that clears bit 5. See prog_p8_cop_base.s.
P8_COP_MUT_A    equ     1
        include "prog_p8_cop_base.s"
