; p5_mutb.s -- p5.s with MUT_VBR and MUT_B set: P5.b's contrast
; (SPECIFICATION.md section 7), in the 68010/68020-vector image: SUBQ.L
; #4,A7 and a NOP take the place of step 3's MOVEC, so PRE_SP differs from
; the other runs while the image differs from the 68000-mode image only
; inside step 3's four bytes.
MUT_VBR         equ     1
MUT_B           equ     1
        include "p5.s"
