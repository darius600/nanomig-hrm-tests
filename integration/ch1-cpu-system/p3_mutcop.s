; p3_mutcop.s -- p3.s with MUT_COP set: P3.c's fail-safe contrast (SPECIFICATION.md section 5): an ADDQ.W before block 3 is an instruction the check does not decode, so the check fails.
MUT_COP         equ     1
        include "p3.s"
