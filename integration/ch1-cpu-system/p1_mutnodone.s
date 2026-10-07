; p1_mutnodone.s -- p1.s with MUT_NODONE set: the fixture's contrast: the program writes S_END and then stops in place without writing DONE.
MUT_NODONE      equ     1
        include "p1.s"
