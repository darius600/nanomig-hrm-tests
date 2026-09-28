; prog_r3_c48first.s -- R3c (section 5.2.6 contrast for IMG1=IMG0): step b
; writes register 48 in place of the 4A write, configuring the board
; immediately; IMG1 is then read as the last action instead of IMG0's own
; step-b snapshot.

MUT_R3C equ 1
        include "prog_r3.s"
