; prog_cr10.s -- MUTATION support: prog_main.s with MUT_CR10 defined. After
; the frame count the program runs nanomig.i's DONE instead of writing
; MARKER (prog_main.s, end of step 8). See SPECIFICATION.md, RUN, contrast CR10.

MUT_CR10 equ 1
        include "prog_main.s"
