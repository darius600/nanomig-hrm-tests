; p5_mutb.s -- p5.s with MUT_VBR and MUT_B set: P5.b's contrast
; (SPECIFICATION.md section 7), in the 68010/68020-vector image: SUBQ.L
; #4,A7 is inserted before case 1, so PRE_SP differs from the other runs.
MUT_VBR         equ     1
MUT_B           equ     1
        include "p5.s"
