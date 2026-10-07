; p5_mutc1.s -- p5.s with MUT_C1 set (68000-mode image): C1's contrast (SPECIFICATION.md 2.5): an extra INTENA write follows step 2, so R_INT shows bit 14 set.
MUT_C1          equ     1
        include "p5.s"
