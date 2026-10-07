; p1_mutc4b.s -- p1.s with MUT_C4B set: C4's ordering contrast (SPECIFICATION.md 2.5): S_START is written after case 1's S_A, so it does not come before every window.
MUT_C4B         equ     1
        include "p1.s"
