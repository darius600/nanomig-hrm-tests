; prog_diskrate_fixturemut.s -- prog_diskrate.s with MUT_FIXTURE set: the
; FIXTURE's own contrasting case (KFIX stores $DEAD instead of $BEEF).
MUT_FIXTURE     equ     1
        include "prog_diskrate.s"
