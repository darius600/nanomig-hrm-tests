; prog_r3_no48.s -- P3-CONTENT's mutation (SPECIFICATION.md section 8.4):
; step c (the byte write to register 48) is left out; step d (the
; memory-board content check) still runs, so the population is not
; empty.

MUT_R3_NO48 equ 1
        include "prog_r3.s"
