; p5_vbr_mutc2.s -- p5.s with MUT_VBR and MUT_C2 set (VBR image): C2's contrast (SPECIFICATION.md 2.5): an extra DMACON write follows step 4, so R_DMA shows bits 9 and 6 set.
MUT_VBR         equ     1
MUT_C2          equ     1
        include "p5.s"
