; p6_mutc2.s -- p6.s with MUT_C2 set: C2's contrast (SPECIFICATION.md 2.5): an extra DMACON write follows step 4, so R_DMA shows bits 9 and 6 set.
MUT_C2          equ     1
        include "p6.s"
