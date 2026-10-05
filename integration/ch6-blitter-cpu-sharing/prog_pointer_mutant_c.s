; prog_pointer_mutant_c.s -- A1-C mutation: the single MOVE.L of channel C points at
; the channel's decoy word; every other channel is unchanged.

PTR_MUT_C     equ     1
        include "prog_pointer.s"
