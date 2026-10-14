; prog_p2_mut_fetch.s -- P2 clause contrast (fetching): phase 2 writes
; DMACON = $0000 and L2's MOVE data word is rewritten to $0010, so the Copper
; keeps fetching L2 while INTREQR bit 4 stays 0. See prog_p2_base.s.
P2_MUT_FETCH    equ     1
        include "prog_p2_base.s"
