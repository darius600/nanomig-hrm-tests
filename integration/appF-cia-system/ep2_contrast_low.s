; ep2_contrast_low.s -- EP2's second contrast: Copper DMA started as in
; ep2_contrast.s, with COP1LC pointing at a list in low Chip memory ($000100),
; so the Copper's own fetches land at word addresses below $1000, where the
; memory-refresh reads are.
        include "nanomig.i"
        include "ciaregs.i"

COPLIST         equ     $000100

        org     PROGRAM_ORG
start:
        SETUP
        lea     CUSTOM,a0
        move.w  #(COPLIST)>>16,COP1LCH(a0)
        move.w  #(COPLIST)&$FFFF,COP1LCL(a0)
        move.w  #0,COPJMP1(a0)          ; strobe: reloads the Copper PC from COP1LC
        move.w  #$8280,DMACON(a0)       ; SET,DMAEN,COPEN
        move.w  #$0200,d7
.d1:    dbra    d7,.d1
        DONE
