; prog_r3_swap_no48.s -- P3-BASE-ASSIGNED's mutation: R3' with step c (the
; byte write to register 48) left out and step d kept, so the board is never
; mapped in at B2.

MUT_SWAP    equ 1
MUT_R3_NO48 equ 1
        include "prog_r3.s"
