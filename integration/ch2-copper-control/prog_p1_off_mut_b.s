; prog_p1_off_mut_b.s -- P1 contrasting case, precondition (b) mutation: step 5
; writes DMACON = $0000, so DMAEN stays clear as well as COPEN.
; See prog_p1_base.s.
P1_BIT7_OFF          equ     1
P1_MUT_OFF_NO_DMAEN  equ     1
        include "prog_p1_base.s"
