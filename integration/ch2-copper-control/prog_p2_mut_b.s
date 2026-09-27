; prog_p2_mut_b.s -- P2 precondition (b) mutation: INTENA = $C010 with a
; level-3 handler installed that clears bit 4. See prog_p2_base.s.
P2_MUT_B        equ     1
        include "prog_p2_base.s"
