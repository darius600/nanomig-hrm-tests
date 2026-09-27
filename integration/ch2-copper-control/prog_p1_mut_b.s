; prog_p1_mut_b.s -- P1 precondition (b) mutation: DMACON = $8080 (COPEN
; without DMAEN) instead of $8280. See prog_p1_base.s.
P1_MUT_NO_DMAEN equ     1
        include "prog_p1_base.s"
