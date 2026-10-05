; prog_r6a.s -- R6a: prog_r6.s with RESET at A replaced by NOP (section
; 5.2.6 contrast for P6-WITNESS). Nothing resets, so the tail runs and the
; run completes.

MUT_R6_NORESET equ 1
        include "prog_r6.s"
