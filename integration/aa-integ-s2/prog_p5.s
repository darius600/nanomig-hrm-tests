; prog_p5.s -- P5: five blits of all four channels, each channel meeting every
; modulo of the set {V0 V1 V2 V3 V4} (SPECIFICATION.md P5).
;
; Symbols defined before this file is included select the variants:
;   DMA_BLT  DMACON value of step 1 ($8200: DMAEN without BLTEN)
;   SIZE     BLTSIZE value ($0102 = height 4, width 2; $0142 = height 5)
;   V0..V4   the modulo values
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

        include "blit.i"

; blit j gives A, B, C, D the values with indices j, j+1, j+2, j+3 (mod 5)
blit_table:
        dc.w    4
        BLIT_HDR  0,$0FF0,0,SIZE
        BLIT_PTR  $150000,$130000,$110000,$170000
        BLIT_MOD  V2,V1,V0,V3
        BLIT_HDR  0,$0FF0,0,SIZE
        BLIT_PTR  $150000,$130000,$110000,$170000
        BLIT_MOD  V3,V2,V1,V4
        BLIT_HDR  0,$0FF0,0,SIZE
        BLIT_PTR  $150000,$130000,$110000,$170000
        BLIT_MOD  V4,V3,V2,V0
        BLIT_HDR  0,$0FF0,0,SIZE
        BLIT_PTR  $150000,$130000,$110000,$170000
        BLIT_MOD  V0,V4,V3,V1
        BLIT_HDR  0,$0FF0,0,SIZE
        BLIT_PTR  $150000,$130000,$110000,$170000
        BLIT_MOD  V1,V0,V4,V2
