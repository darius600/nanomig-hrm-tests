; p1_mutc3.s -- p1.s with MUT_C3 set: C3's contrast (SPECIFICATION.md
; 2.5): a branch to the failure path is inserted just before the normal
; S_END write, so S_FAIL is written.
MUT_C3          equ     1
        include "p1.s"
