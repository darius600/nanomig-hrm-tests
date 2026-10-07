; p5_muta.s -- p5.s with MUT_A set: P5.a's contrast (SPECIFICATION.md section 7): case 1 takes TRAP #1 through vector 33 instead of the installed vector 32 of TRAP #0; the exception is taken and the window stays populated.
MUT_A           equ     1
        include "p5.s"
