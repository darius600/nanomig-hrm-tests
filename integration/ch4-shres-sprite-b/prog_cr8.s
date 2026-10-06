; prog_cr8.s -- MUTATION support: prog_main.s with MUT_CR8 defined.
; FRAME_COUNT (sprb.i) is 3 instead of 6: the program writes MARKER after
; three frame starts, not six. See SPECIFICATION.md PC4, contrast CR8.

MUT_CR8 equ 1
        include "prog_main.s"
