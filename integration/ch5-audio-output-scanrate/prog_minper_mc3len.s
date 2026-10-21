; prog_minper_mc3len.s -- prog_minper.s with MUT_MC3LEN set: precondition MC3's contrasting case (levels): AUD0LEN <- 32, so the channel plays 32 words and repeats.
MUT_MC3LEN  equ     1
        include "prog_minper.s"
