; p5_vbr_mutnodone.s -- p5.s with MUT_VBR and MUT_NODONE set (VBR image): the fixture's contrast: the program writes S_END and then stops in place without writing DONE.
MUT_VBR         equ     1
MUT_NODONE      equ     1
        include "p5.s"
