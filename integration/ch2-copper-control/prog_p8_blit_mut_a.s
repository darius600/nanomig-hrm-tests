; prog_p8_blit_mut_a.s -- P8 BLIT case, precondition (a) mutation: INTENA =
; $8040 (bit 6 set, master enable clear). See prog_p8_blit_base.s.
P8_BLIT_MUT_A   equ     1
        include "prog_p8_blit_base.s"
