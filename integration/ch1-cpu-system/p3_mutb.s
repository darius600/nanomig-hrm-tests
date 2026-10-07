; p3_mutb.s -- p3.s with MUT_B set: P3.b's contrast (SPECIFICATION.md section 5): block 1's BRA.S targets an instruction behind the first guard word, the guard's S_X store runs, and the guard branches to L_1.
MUT_B           equ     1
        include "p3.s"
