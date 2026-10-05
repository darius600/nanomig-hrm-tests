; prog_r3_io_self.s -- P3-CONTROL-IO's mutation: the board configured
; normally at B1, and the write/readback of R3n aimed at B1 itself instead of
; at $00A00000.

MUT_R3N  equ 1
MUT_SELF equ 1
        include "prog_r3.s"
