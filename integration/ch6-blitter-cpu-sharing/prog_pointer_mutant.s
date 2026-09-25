; prog_pointer_mutant.s -- A1 mutation: the same single 32-bit MOVE.L now
; points BLTAPTH/BLTAPTL at a different, equally known source word.

PTR_MUTANT      equ     1
        include "prog_pointer.s"
