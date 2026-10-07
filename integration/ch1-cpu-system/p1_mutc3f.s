; p1_mutc3f.s -- p1.s with MUT_C3F set: C3's S_FAIL contrast (SPECIFICATION.md
; 2.5): a branch to the failure path is made just before the normal S_END write.
MUT_C3F         equ     1
        include "p1.s"
