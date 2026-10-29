; p3_mutcop.s -- p3.s with MUT_COP set: P3.c's read-modify-write contrast (SPECIFICATION.md section 5): an ADDQ.W before block 3 reads and writes a guard word.
MUT_COP         equ     1
        include "p3.s"
