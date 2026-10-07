; p6_mutc.s -- p6.s with MUT_C set: the reused P3.c's contrast (SPECIFICATION.md section 8): a MOVE.W W_3(PC),D2 reads the guard word as data before block 3.
MUT_C           equ     1
        include "p6.s"
