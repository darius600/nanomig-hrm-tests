; prog_pointer_mutant.s -- A1 mutation: the single MOVE.L of every channel
; (A, B, C and D) points at the channel's decoy word.

PTR_MUT_A       equ     1
PTR_MUT_B       equ     1
PTR_MUT_C       equ     1
PTR_MUT_D       equ     1
        include "prog_pointer.s"
