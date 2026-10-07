; p5_mutpeek.s -- p5.s with MUT_PEEK set (68000-mode image): the peek_word fixture's contrast: S_START is overwritten with $0000 after the S_END write; the CPU write of $0001 stays in the log.
MUT_PEEK        equ     1
        include "p5.s"
