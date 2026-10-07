; p5_vbr_mutc3.s -- p5.s with MUT_VBR and MUT_C3 set (VBR image): C3's contrast (SPECIFICATION.md 2.5): the S_END write is made twice.
MUT_VBR         equ     1
MUT_C3          equ     1
        include "p5.s"
