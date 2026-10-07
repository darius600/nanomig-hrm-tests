; p5_mutspan.s -- p5.s with MUT_SPAN set: P5.b's static contrast (SPECIFICATION.md section 7): the image differs from the 68000-mode image in a four-byte span of the same size as step 3's, outside step 3.
MUT_SPAN        equ     1
        include "p5.s"
