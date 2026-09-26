; prog_cr3.s -- MUTATION support: prog_main.s with MUT_CR3 defined. Band 4's
; COLOR16-31 group holds S0-c18 (colour 18 complemented) instead of S0x
; (colour 16 complemented). See SPECIFICATION.md P4 section 6, contrast CR3.

MUT_CR3 equ 1
        include "prog_main.s"
