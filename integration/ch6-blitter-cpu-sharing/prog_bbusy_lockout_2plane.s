; prog_bbusy_lockout_2plane.s -- BBUSY-LOCKOUT-PRE mutation: two high-resolution bitplanes.
; prog_fastrun_lockout.s with BB_TEST and FR_2PLANE set; see that file for the whole design.

BB_TEST         equ     1
FR_2PLANE       equ     1
        include "prog_fastrun_lockout.s"
