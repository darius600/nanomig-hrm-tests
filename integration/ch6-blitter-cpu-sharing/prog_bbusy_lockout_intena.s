; prog_bbusy_lockout_intena.s -- BBUSY-LOCKOUT mutation: both tests read INTENAR instead of DMACONR.
; prog_fastrun_lockout.s with BB_TEST and BB_INTENA set; see that file for the whole design.

BB_TEST         equ     1
BB_INTENA       equ     1
        include "prog_fastrun_lockout.s"
