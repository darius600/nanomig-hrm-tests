; prog_p6.s -- P6: one blit of all four channels with four different modulos
; (SPECIFICATION.md P6).
;
; Symbols defined before this file is included select the variants:
;   DMA_BLT  DMACON value of step 1 ($8200: DMAEN without BLTEN)
;   MODB     channel B's modulo
;   CON0     BLTCON0 ($0DF0: channel C off)
;   SIZE     BLTSIZE value ($00C2 = height 3, width 2; $0102 = height 4)
;   FIXVAL   the constant stored first

        ifnd    MODB
MODB    equ     -4
        endc
        ifnd    CON0
CON0    equ     $0FF0
        endc
        ifnd    SIZE
SIZE    equ     $00C2
        endc

        include "blit.i"

blit_table:
        dc.w    0
        BLIT_HDR  0,CON0,0,SIZE
        BLIT_PTR  $0B0000,$0D0000,$0E0000,$0F0000
        BLIT_MOD  0,MODB,8,20
