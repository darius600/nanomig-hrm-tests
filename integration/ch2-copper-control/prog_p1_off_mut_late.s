; prog_p1_off_mut_late.s -- P1 contrasting case, clause contrast: DMACON =
; $8280 at M1, M2 written at once, and the clear of bit 7 (DMACON = $0080)
; written only after the next frame wrap. The Copper fetches L1's IR1 after
; t(M2), and DMACONR bit 7 reads 0. See prog_p1_base.s.
P1_BIT7_OFF     equ     1
P1_MUT_LATE_CLEAR equ   1
        include "prog_p1_base.s"
