; p3_mutwin.s -- p3.s with MUT_WIN set: P3.d's contrast (SPECIFICATION.md section 5): the four blocks run before S_START is written, the markers and S_END follow, so the markers, C4 and the static checks hold, the run window contains no block, and no CPU read of X_k lies inside it.
MUT_WIN         equ     1
MUT_C4B         equ     1
        include "p3.s"
