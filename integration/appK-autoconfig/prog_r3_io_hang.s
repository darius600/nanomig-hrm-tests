; prog_r3_io_hang.s -- apparatus contrast of the no-answer branch: the I/O-space
; control stops after the write and the scratch write, before the readback.

MUT_R3N    equ 1
MUT_IOBASE equ 1
MUT_HANG_READ equ 1
        include "prog_r3.s"
