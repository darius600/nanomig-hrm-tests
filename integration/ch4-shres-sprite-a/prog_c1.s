; prog_c1.s -- P1's PC1 contrast (SPECIFICATION.md 3.8, C1): BPLCON0 =
; $9000 (HIRES, one bitplane, SHRES=0) instead of $1040. NMH_MUTATE=C1.

MUT_C1  equ     1
        include "prog.s"
