; p3_mutcidx.s -- p3.s with MUT_CIDX set: P3.c's indexed contrast (SPECIFICATION.md section 5): an indexed TST.W before block 3 reads a guard word.
MUT_CIDX        equ     1
        include "p3.s"
