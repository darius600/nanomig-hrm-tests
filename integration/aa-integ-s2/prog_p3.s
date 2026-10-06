; prog_p3.s -- P3: two blits of all four channels, ascending then descending
; (BLTCON1 bit 1), every pointer preloaded and every modulo 0
; (SPECIFICATION.md P3).
;
; Symbols defined before this file is included select the variants:
;   DMA_BLT  DMACON value of step 1 ($8200: DMAEN without BLTEN)
;   SIZE     BLTSIZE value ($0083 = height 2, width 3; $00C3 = height 3)
;   PTRA     channel A's pointer; PTRA0 and PTRA1 that of the first and the second blit
;   FIXVAL   the constant stored first

        ifnd    SIZE
SIZE    equ     $0083
        endc
        ifnd    PTRC
PTRC    equ     $0B0100
        endc
        ifnd    PTRB
PTRB    equ     $0D0100
        endc
        ifnd    PTRA
PTRA    equ     $0E0100
        endc
        ifnd    PTRA0
PTRA0   equ     PTRA
        endc
        ifnd    PTRA1
PTRA1   equ     PTRA
        endc
        ifnd    PTRD
PTRD    equ     $0F0100
        endc

        include "blit.i"

blit_table:
        dc.w    1
        BLIT_HDR  0,$0FF0,$0000,SIZE
        BLIT_PTR  PTRC,PTRB,PTRA0,PTRD
        BLIT_MOD  0,0,0,0
        BLIT_HDR  0,$0FF0,$0002,SIZE
        BLIT_PTR  PTRC,PTRB,PTRA1,PTRD
        BLIT_MOD  0,0,0,0
