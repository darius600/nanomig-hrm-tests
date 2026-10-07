; p6_mutb.s -- p6.s with MUT_B set: the reused P3.b's contrast (SPECIFICATION.md section 8): block 1's BRA.S targets an instruction behind the first guard word and the guard's S_X store runs.
MUT_B           equ     1
        include "p6.s"
