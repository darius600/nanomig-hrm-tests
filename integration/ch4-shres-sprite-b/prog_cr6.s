; prog_cr6.s -- MUTATION support: prog_main.s with MUT_CR6 defined. The
; band-1 WAIT becomes $5C07 (line 92) instead of $5807 (line 88). See
; SPECIFICATION.md PC3, contrast CR6.

MUT_CR6 equ 1
        include "prog_main.s"
