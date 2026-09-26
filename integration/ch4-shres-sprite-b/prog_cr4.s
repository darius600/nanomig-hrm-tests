; prog_cr4.s -- MUTATION support: prog_main.s with MUT_CR4 defined. BPLCON0
; is written $9000 (HIRES=1, SHRES=0) instead of $1040. See
; SPECIFICATION.md PC1, contrast CR4.

MUT_CR4 equ 1
        include "prog_main.s"
