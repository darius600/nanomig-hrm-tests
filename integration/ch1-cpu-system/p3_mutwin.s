; p3_mutwin.s -- p3.s with MUT_WIN set: the main relation's contrast (SPECIFICATION.md section 5): the four blocks run before S_START is written, the markers and S_END follow, so every precondition holds and the run window contains no block.
MUT_WIN         equ     1
MUT_C4B         equ     1
        include "p3.s"
