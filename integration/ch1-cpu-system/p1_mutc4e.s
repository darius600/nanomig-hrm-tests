; p1_mutc4e.s -- p1.s with MUT_C4E set: C4's S_END ordering contrast (SPECIFICATION.md 2.5): S_END is written once, before case 21's S_B write, so it does not come after every window.
MUT_C4E         equ     1
        include "p1.s"
