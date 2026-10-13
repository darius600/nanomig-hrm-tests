; p5_mutspan.s -- p5.s with MUT_VBR and MUT_SPAN set: P5.b's static contrast (SPECIFICATION.md section 7): the VBR image (step 3's MOVEC kept) that also differs from the 68000-mode image at case 3's MOVE.W #1,CCR, outside step 3.
MUT_VBR         equ     1
MUT_SPAN        equ     1
        include "p5.s"
