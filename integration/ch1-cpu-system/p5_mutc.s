; p5_mutc.s -- p5.s with MUT_C set: P5.c's contrast (SPECIFICATION.md
; section 7): case 2 gets an extra MOVE.W D0,S_P before its instruction, a
; write outside the stack, inside W_2.
MUT_C           equ     1
        include "p5.s"
