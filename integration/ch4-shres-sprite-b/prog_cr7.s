; prog_cr7.s -- MUTATION support: prog_main.s with MUT_CR7 defined. The
; program stores INTENAR instead of DENISEID at RESULTS+4 (sprb.i,
; IDREG_DENISE). See SPECIFICATION.md PC0, contrast CR7.

MUT_CR7 equ 1
        include "prog_main.s"
