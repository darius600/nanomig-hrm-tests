; prog_r1_fddctl.s -- R1 with a disk DMA started between the motor on-latch
; and the first frame wait (DMACON DSKEN, DSKLEN written twice with a nonzero
; length), so the drive-LED port changes level between MK_CALBRIGHT and t_M1.

MUT_FDDCTL equ 1
        include "prog_r1.s"
