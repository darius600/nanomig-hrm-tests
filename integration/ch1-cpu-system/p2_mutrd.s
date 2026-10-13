; p2_mutrd.s -- p2.s with MUT_RD set: P2.c's read-clause contrast (SPECIFICATION.md section 4): a TST.W of a word outside the program image and outside SAVE_SP is made between steps 6 and 7, adding a read inside the window and no write.
MUT_RD          equ     1
        include "p2.s"
