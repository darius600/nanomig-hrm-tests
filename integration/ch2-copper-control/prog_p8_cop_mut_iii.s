; prog_p8_cop_mut_iii.s -- P8 COP case, check (iii) contrast: once a poll
; record reads bit 4 = 1, INTREQ=$0010 is written again. See
; prog_p8_cop_base.s.
P8_COP_MUT_III  equ     1
        include "prog_p8_cop_base.s"
