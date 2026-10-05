; prog_pointer_mutant_b.s -- A1-B mutation: the single MOVE.L of channel B points at
; the channel's decoy word; every other channel is unchanged.

PTR_MUT_B     equ     1
        include "prog_pointer.s"
