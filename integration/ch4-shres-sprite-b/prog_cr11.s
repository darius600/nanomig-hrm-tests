; prog_cr11.s -- MUTATION support: prog_main.s with MUT_CR11 defined. DMACON
; is written $8380 (SPREN not set) instead of $83A0, removing the sprite
; anchor the reference frame depends on. See SPECIFICATION.md section 4,
; contrast CR11.

MUT_CR11 equ 1
        include "prog_main.s"
