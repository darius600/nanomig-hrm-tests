; prog_minper_mc4cpu.s -- prog_minper.s with MUT_MC4CPU set: precondition MC4's contrasting case: one CPU read of the word at T+64 in the middle of the wait.
MUT_MC4CPU  equ     1
        include "prog_minper.s"
