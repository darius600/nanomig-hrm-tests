; prog_minper_mc5stop.s -- prog_minper.s with MUT_MC5STOP set: precondition MC5's contrasting case: DMACON <- $0001 in line 200 of the wait, when the beam's horizontal counter (VHPOSR bits 7-0) has reached MUT_H.
MUT_H       equ     8
MUT_MC5STOP  equ     1
        include "prog_minper.s"
