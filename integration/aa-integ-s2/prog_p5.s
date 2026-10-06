; prog_p5.s -- P5: five blits of all four channels, each channel meeting every
; modulo of the set {V0 V1 V2 V3 V4} (SPECIFICATION.md P5).
;
; Symbols defined before this file is included select the variants:
;   DMA_BLT  DMACON value of step 1 ($8200: DMAEN without BLTEN)
;   SIZE     BLTSIZE value ($0102 = height 4, width 2; $0142 = height 5)
;   V0..V4   the modulo values
;   MC0 MB0 MA0 MD0  the modulos of the first blit's channels C, B, A, D (default: the set's own)
;   DESC     defined: the third blit is descending (BLTCON1 bit 1) with its four
;            pointers $10 higher, so that its accesses stay in the channel regions
;   FIXVAL   the constant stored first

        ifnd    SIZE
SIZE    equ     $0102
        endc
        ifnd    V0
V0      equ     $0000
        endc
        ifnd    V1
V1      equ     $0006
        endc
        ifnd    V2
V2      equ     $FFFC
        endc
        ifnd    V3
V3      equ     $3FFE
        endc
        ifnd    V4
V4      equ     $C002
        endc

        ifnd    MC0
MC0     equ     V2
        endc
        ifnd    MB0
MB0     equ     V1
        endc
        ifnd    MA0
MA0     equ     V0
        endc
        ifnd    MD0
MD0     equ     V3
        endc
        ifd     DESC
DCON1   equ     2
DOFF    equ     $10
        else
DCON1   equ     0
DOFF    equ     0
        endc

        include "blit.i"

; blit j gives A, B, C, D the values with indices j, j+1, j+2, j+3 (mod 5)
blit_table:
        dc.w    4
        BLIT_HDR  0,$0FF0,0,SIZE
        BLIT_PTR  $150000,$130000,$110000,$170000
        BLIT_MOD  MC0,MB0,MA0,MD0
        BLIT_HDR  0,$0FF0,0,SIZE
        BLIT_PTR  $150000,$130000,$110000,$170000
        BLIT_MOD  V3,V2,V1,V4
        BLIT_HDR  0,$0FF0,DCON1,SIZE
        BLIT_PTR  $150000+DOFF,$130000+DOFF,$110000+DOFF,$170000+DOFF
        BLIT_MOD  V4,V3,V2,V0
        BLIT_HDR  0,$0FF0,0,SIZE
        BLIT_PTR  $150000,$130000,$110000,$170000
        BLIT_MOD  V0,V4,V3,V1
        BLIT_HDR  0,$0FF0,0,SIZE
        BLIT_PTR  $150000,$130000,$110000,$170000
        BLIT_MOD  V1,V0,V4,V2
