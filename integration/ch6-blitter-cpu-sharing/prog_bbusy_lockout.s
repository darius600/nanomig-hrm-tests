; prog_bbusy_lockout.s -- BBUSY-LOCKOUT [p186]: the blitter-busy test pair in a locked-out blit.
; prog_fastrun_lockout.s with BB_TEST set; see that file for the whole design.

BB_TEST         equ     1
        include "prog_fastrun_lockout.s"
