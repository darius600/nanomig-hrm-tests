; prog_p4.s -- P4: a blit of all four channels, then a second blit started
; with BLTSIZE alone, no pointer or modulo register written in between
; (SPECIFICATION.md P4).
;
; Symbols defined before this file is included select the variants:
;   DMA_BLT  DMACON value of step 1 ($8200: DMAEN without BLTEN)
;   SIZE     BLTSIZE value of both blits ($0083 = height 2, width 3; $00C3 = height 3)
;   MODA MODB MODC MODD  the modulos of the first blit
;   TOUCH    a CPU read of this byte address right after each BLTSIZE write
;   FIXVAL   the constant stored first

        ifnd    SIZE
SIZE    equ     $0083
        endc
        ifnd    MODA
MODA    equ     6
        endc
        ifnd    MODB
MODB    equ     -2
        endc
        ifnd    MODC
MODC    equ     0
        endc
        ifnd    MODD
MODD    equ     10
        endc

        include "blit.i"

blit_table:
        dc.w    1
        BLIT_HDR  0,$0FF0,$0000,SIZE
        BLIT_PTR  $0B0000,$0D0000,$0E0000,$0F0000
        BLIT_MOD  MODC,MODB,MODA,MODD
        BLIT_HDR  1,0,0,SIZE
        BLIT_PTRW 0,0,0,0,0,0,0,0
        BLIT_MOD  0,0,0,0
