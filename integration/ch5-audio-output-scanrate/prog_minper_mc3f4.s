; prog_minper_mc3f4.s -- prog_minper.s with MUT_MC3F4 set: precondition MC3's contrasting case (anchor F4): one CPU read of the word at T+64 at the end of the port window.
MUT_LINE    equ     1140
MUT_MC3F4  equ     1
        include "prog_minper.s"
