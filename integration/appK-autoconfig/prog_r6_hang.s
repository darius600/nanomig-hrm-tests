; prog_r6_hang.s -- R6 whose instruction stream after RESET never reaches
; the tail: it stops at the first word after the C2 write. No M2 write and no
; completion marker follow.

MUT_R6_HANG equ 1
        include "prog_r6.s"
