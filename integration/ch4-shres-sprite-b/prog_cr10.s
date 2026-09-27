; prog_cr10.s -- MUTATION support: prog_main.s with MUT_CR10 defined. The
; program never writes MARKER; it loops before the write. See
; SPECIFICATION.md, RUN, contrast CR10.

MUT_CR10 equ 1
        include "prog_main.s"
