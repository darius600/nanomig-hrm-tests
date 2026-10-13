; p2_mutvec.s -- p2.s with MUT_VEC set: P2.c's vector-area contrast (SPECIFICATION.md section 4): a TST.W of vector word $0010 is made between steps 6 and 7, adding a read inside the window in the vector area, not of word 0, and no write.
MUT_VEC         equ     1
        include "p2.s"
