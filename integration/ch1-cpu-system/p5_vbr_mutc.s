; p5_vbr_mutc.s -- p5.s with MUT_VBR and MUT_C set (VBR image): P5.c's contrast (SPECIFICATION.md section 7) for the 68010 and 68020 runs: case 2 gets an extra MOVE.W D0,S_P before its instruction, a write outside the stack, inside W_2.
MUT_VBR         equ     1
MUT_C           equ     1
        include "p5.s"
