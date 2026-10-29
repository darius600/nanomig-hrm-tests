; p3_mutcunsup.s -- p3.s with MUT_CUNSUP set: P3.c's unsupported-analysis contrast (SPECIFICATION.md section 5): a SWAP before block 3 is an instruction the check does not decode.
MUT_CUNSUP      equ     1
        include "p3.s"
