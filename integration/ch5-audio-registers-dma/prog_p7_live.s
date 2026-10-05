; prog_p7_live.s -- the control for P7's comparators: the CPU executes STOP
; straight after the BLTSIZE write, so it makes no memory access while the
; blit runs.

LIVE            equ     1
        include "prog_p7.s"
