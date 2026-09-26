; prog_cr5.s -- MUTATION support: prog_main.s with MUT_CR5 defined. DMACON
; is written $8380 (SPREN not set) instead of $83A0. See SPECIFICATION.md
; PC2, contrast CR5.

MUT_CR5 equ 1
        include "prog_main.s"
