; prog_pointer_mutant_a.s -- A1-A mutation: the single MOVE.L of channel A points at
; the channel's decoy word; every other channel is unchanged.

PTR_MUT_A     equ     1
        include "prog_pointer.s"
