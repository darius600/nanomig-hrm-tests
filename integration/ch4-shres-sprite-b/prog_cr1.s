; prog_cr1.s -- MUTATION support: prog_main.s with MUT_CR1 defined. Section
; T's COLOR16-31 group holds S2 instead of S0 in band 0. See SPECIFICATION.md
; P1/P2/P3 section 6, contrast CR1.

MUT_CR1 equ 1
        include "prog_main.s"
