; prog_p2_mut_p2p1.s -- P2-p1's own mutation (NMH_MUTATE=P2-P1): the write
; of $AAAA to $0E000E happens in step 3, before the Copper starts, instead
; of after the first field wrap. See prog_p2.s.

P2_MUT_EARLY    equ     1
        include "prog_p2.s"
