; p5_vbr_mutpeek.s -- p5.s with MUT_VBR and MUT_PEEK set (VBR image): the peek_word fixture's contrast: S_START is overwritten with $0000 after the S_END write; the CPU write of $0001 stays in the log.
MUT_VBR         equ     1
MUT_PEEK        equ     1
        include "p5.s"
