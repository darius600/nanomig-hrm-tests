; prog_p7_b.s -- P7 contrasting case (b): the CPU write of $0000 goes to $4F100, a word
; no other access of the program touches, instead of to $48000.

P7_NOCLEAR      equ     1
        include "prog_p7.s"
