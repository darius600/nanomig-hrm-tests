; prog_c2.s -- PC2's contrast (SPECIFICATION.md 3.8, C2): SPR1PTH points at
; the null structure instead of sprite 1's own data. NMH_MUTATE=C2; the
; C++ side points the Copper list's own SPR1 MOVE the same way.

MUT_C2  equ     1
        include "prog.s"
