; prog_cr11.s -- MUTATION support: prog_main.s with MUT_CR11 defined. Every
; COLOR16-31 data word of the Copper list (sprb_data.i) is ANDed with $F333,
; clearing bits 3-2 of each component, so no sprite sample is lit after the
; test's mask. See SPECIFICATION.md section 5, REF, contrast CR11.

MUT_CR11 equ 1
        include "prog_main.s"
