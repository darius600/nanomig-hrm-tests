; prog_cr5.s -- MUTATION support: prog_main.s with MUT_CR5 defined. The
; two-word terminator of the sprite descriptor table (sprb_data.i) is
; $0001,$0000 instead of $0000,$0000. See SPECIFICATION.md PC2, contrast CR5.

MUT_CR5 equ 1
        include "prog_main.s"
