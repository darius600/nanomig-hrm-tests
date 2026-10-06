; prog_p2.s -- P2: one-channel blits whose pointer is V1 = $155554 or
; V2 = $0AAAAA for each of A, B, C, D, then the same V1 with PTH bit 5 set
; (SPECIFICATION.md P2).
;
; Symbols defined before this file is included select the variants:
;   DMA_BLT  DMACON value of step 1 ($8200: DMAEN without BLTEN)
;   SIZE     BLTSIZE value ($0041 = height 1, width 1; $00C1 = height 3)
;   V1H V1L V2H V2L  the PTH and PTL words of the two pointer values
;   P5H      the PTH word of the contrast blits ($0035: V1's PTH with bit 5 set)
;   FIXVAL   the constant stored first

        ifnd    SIZE
SIZE    equ     $0041
        endc
        ifnd    V1H
V1H     equ     $0015
        endc
        ifnd    V1L
V1L     equ     $5554
        endc
        ifnd    V2H
V2H     equ     $000A
        endc
        ifnd    V2L
V2L     equ     $AAAA
        endc
        ifnd    P5H
P5H     equ     $0035
        endc

        include "blit.i"

; neutral pointers: C $0B0000, B $0D0000, A $0E0000, D $0F0000
; each blit: the channel under test takes the pointer, the others stay neutral
blit_table:
        dc.w    11
; ---- A: V1, V2
        BLIT_HDR  0,$09F0,0,SIZE
        BLIT_PTRW $000B,$0000,$000D,$0000,V1H,V1L,$000F,$0000
        BLIT_MOD  8,16,24,32
        BLIT_HDR  0,$09F0,0,SIZE
        BLIT_PTRW $000B,$0000,$000D,$0000,V2H,V2L,$000F,$0000
        BLIT_MOD  8,16,24,32
; ---- B: V1, V2
        BLIT_HDR  0,$05CC,0,SIZE
        BLIT_PTRW $000B,$0000,V1H,V1L,$000E,$0000,$000F,$0000
        BLIT_MOD  8,16,24,32
        BLIT_HDR  0,$05CC,0,SIZE
        BLIT_PTRW $000B,$0000,V2H,V2L,$000E,$0000,$000F,$0000
        BLIT_MOD  8,16,24,32
; ---- C: V1, V2
        BLIT_HDR  0,$03AA,0,SIZE
        BLIT_PTRW V1H,V1L,$000D,$0000,$000E,$0000,$000F,$0000
        BLIT_MOD  8,16,24,32
        BLIT_HDR  0,$03AA,0,SIZE
        BLIT_PTRW V2H,V2L,$000D,$0000,$000E,$0000,$000F,$0000
        BLIT_MOD  8,16,24,32
; ---- D: V1, V2
        BLIT_HDR  0,$01FF,0,SIZE
        BLIT_PTRW $000B,$0000,$000D,$0000,$000E,$0000,V1H,V1L
        BLIT_MOD  8,16,24,32
        BLIT_HDR  0,$01FF,0,SIZE
        BLIT_PTRW $000B,$0000,$000D,$0000,$000E,$0000,V2H,V2L
        BLIT_MOD  8,16,24,32
; ---- contrast from above: V1 with PTH bit 5 set, A, B, C, D
        BLIT_HDR  0,$09F0,0,SIZE
        BLIT_PTRW $000B,$0000,$000D,$0000,P5H,V1L,$000F,$0000
        BLIT_MOD  8,16,24,32
        BLIT_HDR  0,$05CC,0,SIZE
        BLIT_PTRW $000B,$0000,P5H,V1L,$000E,$0000,$000F,$0000
        BLIT_MOD  8,16,24,32
        BLIT_HDR  0,$03AA,0,SIZE
        BLIT_PTRW P5H,V1L,$000D,$0000,$000E,$0000,$000F,$0000
        BLIT_MOD  8,16,24,32
        BLIT_HDR  0,$01FF,0,SIZE
        BLIT_PTRW $000B,$0000,$000D,$0000,$000E,$0000,P5H,V1L
        BLIT_MOD  8,16,24,32
