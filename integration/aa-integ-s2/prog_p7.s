; prog_p7.s -- P7: the Copper writes the twelve Blitter pointer and modulo
; registers, then a blit shows what each register holds (SPECIFICATION.md P7).
;
; Symbols defined before this file is included select the variants:
;   COPCONV  COPCON value (0 or 2)
;   COLOR    the Copper's MOVE to each register is a MOVE to COLOR00
;   NOCOPER  the Copper's MOVE to INTREQ is a MOVE to the no-operation register
;   DMA_BLT  DMACON value of the blit's step 1 ($8200: DMAEN without BLTEN)
;   FIXVAL   the constant stored first

        ifnd    COPCONV
COPCONV equ     0
        endc
HOOK    equ     1

        include "blit.i"

blit_table:
        dc.w    0
        BLIT_HDR  2,$0FF0,0,$0082
        BLIT_PTRW 0,0,0,0,0,0,0,0
        BLIT_MOD  0,0,0,0
