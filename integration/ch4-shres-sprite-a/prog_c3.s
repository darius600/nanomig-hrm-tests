; prog_c3.s -- PC3's contrast (SPECIFICATION.md 3.8, C3): SPR2PTH points at
; sprite 3's own null structure instead of sprite 2's. NMH_MUTATE=C3; the
; C++ side points the Copper list's own SPR2 MOVE the same way.

MUT_C3  equ     1
        include "prog.s"
