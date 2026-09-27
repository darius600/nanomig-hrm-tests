; prog_cr9.s -- MUTATION support: prog_main.s with MUT_CR9 defined. RESULTS
; stores $DEAD instead of $BEEF. See SPECIFICATION.md, FIXTURE, contrast CR9.

MUT_CR9 equ 1
        include "prog_main.s"
