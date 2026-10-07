; p3_mutcreg.s -- p3.s with MUT_CREG set: P3.c's register-indirect contrast (SPECIFICATION.md section 5): a LEA W_3(PC),A1 and a TST.W (A1) read the guard word as data before block 3.
MUT_CREG        equ     1
        include "p3.s"
