; prog_nodone.s -- the DONE line's contrast: the program stops in an endless loop
; right before its DONE store. NMH_MUTATE=NODONE.

MUT_NODONE equ  1
        include "prog.s"
