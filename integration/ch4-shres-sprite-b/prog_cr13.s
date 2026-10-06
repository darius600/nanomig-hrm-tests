; prog_cr13.s -- MUTATION support: prog_main.s with MUT_CR13 defined. Sprite
; 1's POS word is $506E (HSTART 221) instead of $506C (HSTART 217)
; (sprb.i, SPR1POSVAL). See SPECIFICATION.md SU, contrast CR13.

MUT_CR13 equ 1
        include "prog_main.s"
