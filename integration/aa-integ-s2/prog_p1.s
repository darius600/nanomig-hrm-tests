; prog_p1.s -- P1: four blits, one per channel pair, every pointer and modulo
; register rewritten before each (SPECIFICATION.md P1).
;
; Symbols defined before this file is included select the variants:
;   DMA_BLT  DMACON value of step 1 ($8200: DMAEN without BLTEN)
;   SIZE     BLTSIZE value ($0082 = height 2, width 2; $00C2 = height 3)
;   TOUCH    a CPU read of this byte address right after each BLTSIZE write
;   FIXVAL   the constant stored first
;   PTRC PTRB PTRA PTRD  the pointer values; MODC MODB MODA MODD  the modulos

        ifnd    SIZE
SIZE    equ     $0082
        endc
        ifnd    PTRC
PTRC    equ     $0B0000
        endc
        ifnd    PTRB
PTRB    equ     $0D0000
        endc
        ifnd    PTRA
PTRA    equ     $0E0000
        endc
        ifnd    PTRD
PTRD    equ     $0F0000
        endc
        ifnd    MODC
MODC    equ     8
        endc
        ifnd    MODB
MODB    equ     16
        endc
        ifnd    MODA
MODA    equ     24
        endc
        ifnd    MODD
MODD    equ     32
        endc

        include "blit.i"

blit_table:
        dc.w    3
        BLIT_HDR  0,$09F0,0,SIZE
        BLIT_PTR  PTRC,PTRB,PTRA,PTRD
        BLIT_MOD  MODC,MODB,MODA,MODD
        BLIT_HDR  0,$05CC,0,SIZE
        BLIT_PTR  PTRC,PTRB,PTRA,PTRD
        BLIT_MOD  MODC,MODB,MODA,MODD
        BLIT_HDR  0,$03AA,0,SIZE
        BLIT_PTR  PTRC,PTRB,PTRA,PTRD
        BLIT_MOD  MODC,MODB,MODA,MODD
        BLIT_HDR  0,$01FF,0,SIZE
        BLIT_PTR  PTRC,PTRB,PTRA,PTRD
        BLIT_MOD  MODC,MODB,MODA,MODD
