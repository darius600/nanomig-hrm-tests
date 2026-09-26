; p3_mutc.s -- p3.s with MUT_C set: P3.c's contrast (SPECIFICATION.md
; section 5): a TST.W W_3 is inserted before block 3, reading the guard
; word for real, before block 3's own S_P write.
MUT_C           equ     1
        include "p3.s"
