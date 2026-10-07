; p3_mutwin.s -- p3.s with MUT_WIN set: the main relation's contrast (SPECIFICATION.md section 5): the S_END write is made right after the setup, so the run window closes before the first block.
MUT_WIN         equ     1
        include "p3.s"
