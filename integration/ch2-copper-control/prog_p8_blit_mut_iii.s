; prog_p8_blit_mut_iii.s -- P8 BLIT case, check (iii) contrast: once a poll
; record reads bit 6 = 1, INTREQ = $0040 is written again. See
; prog_p8_blit_base.s.
P8_BLIT_MUT_III equ     1
        include "prog_p8_blit_base.s"
