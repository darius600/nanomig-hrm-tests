; prog_r1_priv.s -- "no exception" precondition contrast: prog_r1.s with SR
; set to user mode ($0700) immediately before the RESET sequence, so RESET
; itself is a privilege violation (section 5.1.6; Motorola, not the
; manual).

MUT_PRIV equ 1
        include "prog_r1.s"
