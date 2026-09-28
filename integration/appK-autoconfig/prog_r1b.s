; prog_r1b.s -- R1b: prog_r1.s with MOVE.W D0,(A0) at A+2 replaced by NOP
; (section 5.2.6 contrast for P2 and P7 part (ii)).

MUT_NOMOVE equ 1
        include "prog_r1.s"
