; prog_cr12.s -- MUTATION support: prog_main.s with MUT_CR12 defined. The
; program stores INTENAR instead of VPOSR at RESULTS+2 (sprb.i,
; IDREG_VPOSR). See SPECIFICATION.md PC0, contrast CR12.

MUT_CR12 equ 1
        include "prog_main.s"
