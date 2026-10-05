; prog_r1a.s -- R1a: prog_r1.s with RESET at A replaced by NOP (section
; 5.1.6 contrast for P1b and P2-WITNESS). Nothing resets, so the tail runs
; and the run completes.

MUT_NORESET equ 1
        include "prog_r1.s"
