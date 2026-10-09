; prog_r3_n_hang.s -- R3n stops after the write at the other base and the
; scratch write, before the readback: the board has taken the write.

MUT_R3N equ 1
MUT_HANG_READ equ 1
        include "prog_r3.s"
