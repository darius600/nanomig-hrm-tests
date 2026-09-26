; prog_cr8.s -- MUTATION support: prog_main.s with MUT_CR8 defined. MARKER
; is written after one frame start instead of six. See SPECIFICATION.md
; PC4, contrast CR8.

MUT_CR8 equ 1
        include "prog_main.s"
