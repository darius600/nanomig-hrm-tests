; prog_r6am.s -- R6am: prog_r6.s's memory-board ending with RESET at A
; replaced by NOP (section 5.2.6 contrast for P6/P7 part (iii)).

MUT_R6M equ 1
MUT_R6_NORESET equ 1
        include "prog_r6.s"
