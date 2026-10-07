; p5_vbr_mutc4.s -- p5.s with MUT_VBR and MUT_C4 set (VBR image): C4's contrast (SPECIFICATION.md 2.5): the S_START write is made twice.
MUT_VBR         equ     1
MUT_C4          equ     1
        include "p5.s"
