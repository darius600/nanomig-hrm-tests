; prog_diskrate_fastf0.s -- prog_diskrate.s with MUT_FASTF0 set: run F's
; ADKCON write becomes $0100 instead of $8100, which is B1's own
; contrasting case (ADKCONR no longer matches run F's intended FAST=1).
MUT_FASTF0      equ     1
        include "prog_diskrate.s"
