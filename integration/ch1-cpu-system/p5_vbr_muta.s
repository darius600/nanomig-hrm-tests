; p5_vbr_muta.s -- p5.s with MUT_VBR and MUT_A set (VBR image): P5.a's contrast (SPECIFICATION.md section 7) for the 68010 and 68020 runs: case 1 takes TRAP #1 through vector 33 instead of the installed vector 32 of TRAP #0.
MUT_VBR         equ     1
MUT_A           equ     1
        include "p5.s"
