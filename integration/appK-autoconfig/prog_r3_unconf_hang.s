; prog_r3_unconf_hang.s -- apparatus contrast of the no-answer branch: the
; unconfigured control stops after the write and the scratch write, before the
; readback.

MUT_R3U equ 1
MUT_R3N equ 1
MUT_HANG_READ equ 1
        include "prog_r3.s"
