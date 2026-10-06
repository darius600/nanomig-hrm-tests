; prog_diskrate_fasts1.s -- prog_diskrate.s with MUT_FASTS1 set: run S's
; ADKCON write becomes $8100 instead of $0100, which is B1's own
; contrasting case for run S (ADKCONR no longer matches run S's intended
; FAST=0).
MUT_FASTS1      equ     1
        include "prog_diskrate.s"
