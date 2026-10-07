; p1_mutpeek.s -- p1.s with MUT_PEEK set: the peek_word fixture's contrast: S_START is overwritten with $0000 after the S_END write; the CPU write of $0001 stays in the log.
MUT_PEEK        equ     1
        include "p1.s"
