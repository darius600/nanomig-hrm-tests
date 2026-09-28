; prog_c12.s -- the fixture check's own contrast: $DEAD stored at result+2
; instead of $BEEF. NMH_MUTATE=C12.

MUT_C12 equ     1
        include "prog.s"
