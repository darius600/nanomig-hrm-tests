; prog_minper_mc5stop.s -- prog_minper.s with MUT_MC5STOP set: precondition MC5's contrasting case: DMACON <- $0001 after 200 lines of the wait and a delay.
MUT_DELAY   equ     30
MUT_MC5STOP  equ     1
        include "prog_minper.s"
