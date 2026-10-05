; prog_r3_n_no48.s -- mutation of P3-NOTOTHER-ASSIGNED
; (SPECIFICATION.md section 5.2, P3, "6. Contrasting cases"): R3n with step c
; (the register-48 write) left out and step d kept, so the board is never
; mapped in at B1.

MUT_R3N equ 1
MUT_R3_NO48 equ 1
        include "prog_r3.s"
