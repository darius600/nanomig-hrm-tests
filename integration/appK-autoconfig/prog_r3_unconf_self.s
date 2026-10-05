; prog_r3_unconf_self.s -- P3-CONTROL-UNCONF's mutation: the board configured
; at B2 = $00400000 (bases exchanged, steps b-d run) and the write/readback of
; R3n aimed at B2 itself, where the unconfigured control reads it.

MUT_SWAP equ 1
MUT_R3N  equ 1
MUT_SELF equ 1
        include "prog_r3.s"
