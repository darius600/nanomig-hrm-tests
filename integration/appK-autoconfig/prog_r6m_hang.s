; prog_r6m_hang.s -- R6m whose instruction stream after RESET never reaches
; the tail (see prog_r6_hang.s).

MUT_R6M equ 1
MUT_R6_HANG equ 1
        include "prog_r6.s"
