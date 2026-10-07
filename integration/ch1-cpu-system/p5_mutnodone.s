; p5_mutnodone.s -- p5.s with MUT_NODONE set (68000-mode image): the fixture's contrast: the program writes S_END and then stops in place without writing DONE.
MUT_NODONE      equ     1
        include "p5.s"
