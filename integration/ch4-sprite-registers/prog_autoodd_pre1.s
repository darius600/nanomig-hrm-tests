; prog_autoodd_pre1.s -- prog_autoodd.s (the SPR0PTL MOVE carries $5001) with MUT_P6PRE1
; set: DMACON without SPREN. See prog_auto.s.
MUT_ODD         equ     1
MUT_P6PRE1      equ     1
        include "prog_auto.s"
