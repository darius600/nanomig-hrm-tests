; p3_mutcpc.s -- p3.s with MUT_CPC set: P3.c's PC-relative contrast (SPECIFICATION.md section 5): a MOVE.W W_3(PC),D2 reads the guard word as data before block 3.
MUT_CPC         equ     1
        include "p3.s"
