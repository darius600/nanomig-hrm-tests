; prog_p4.s -- P4 [p25]: a 680x0 long-word write puts the most significant
; word at the lower address. No Copper. FRAME_INIT clears the chip
; registers (COPCON stays 0, unused here). The scratch area $0E6000-
; $0E607F is cleared to $0000 first (P4-p2's own anchor), then marker Ma
; ($0E7000), then the seven long-word forms W1-W7 (SPECIFICATION.md 5,
; P4.2), then marker Mb ($0E7002), then DONE.
;
; Symbols this source answers to:
;   P4_MUT_EXTRA    -- P4-p1's own mutation: one extra word write between
;                       Ma and Mb.
;   P4_MUT_CLEARLATE -- P4-p2's own mutation: the clearing loop moves after
;                        Mb instead of before Ma.
;   P4_MUT_SWAP     -- the P4 contrasting case: D0 and D1's halves
;                       exchanged, and W1's own immediate written
;                       pre-swapped too (so every one of the eight forms
;                       diverges from the unchanged, main-run comparator).

        include "nanomig.i"
        include "ch2move.i"

SCR             equ     $0E6000         ; P4 scratch, 64 words ($0E6000-$0E607F)
MARK_A          equ     $0E7000
MARK_B          equ     $0E7002

        ifd     P4_MUT_SWAP
D0_VAL          equ     $3C5AA5C3
D1_VAL          equ     $E1F00F1E
W1_VAL          equ     $3C5AA5C3
        else
D0_VAL          equ     $A5C33C5A
D1_VAL          equ     $0F1EE1F0
W1_VAL          equ     $A5C33C5A
        endif

        org     PROGRAM_ORG

start:
        FRAME_INIT

        ifnd    P4_MUT_CLEARLATE
        bsr     clear_scratch
        endif

        lea     MARK_A,a6
        move.w  VHPOSR(a0),(a6)+       ; marker Ma

        ; W1: MOVE.L #$A5C33C5A,(A0'), target $0E6000 -- immediate, not D0.
        movea.l #SCR,a1
        move.l  #W1_VAL,(a1)

        ; W2: MOVE.L D0,(A1)+, A1 = $0E6010 -> target $0E6010.
        move.l  #D0_VAL,d0
        movea.l #SCR+$10,a1
        move.l  d0,(a1)+

        ifd     P4_MUT_EXTRA
        move.w  #$1111,SCR+$70
        endif

        ; W3: MOVE.L D0,-(A2), A2 = $0E6024 -> predecrement to $0E6020.
        movea.l #SCR+$24,a2
        move.l  d0,-(a2)

        ; W4: MOVE.L D0,8(A3), A3 = $0E6028 -> target $0E6028+8=$0E6030.
        movea.l #SCR+$28,a3
        move.l  d0,8(a3)

        ; W5: MOVE.L D0,$0E6040 (absolute long).
        move.l  d0,SCR+$40

        ; W6: MOVEM.L D0/D1,(A4), A4 = $0E6050 -> D0 at $0E6050, D1 at $0E6054.
        move.l  #D1_VAL,d1
        movea.l #SCR+$50,a4
        movem.l d0-d1,(a4)

        ; W7: MOVE.L D0,(A5), A5 = $0E6062 (address = 2 mod 4).
        movea.l #SCR+$62,a5
        move.l  d0,(a5)

        move.w  VHPOSR(a0),(a6)+       ; marker Mb

        ifd     P4_MUT_CLEARLATE
        bsr     clear_scratch
        endif

        DONE

; Clears SCR..SCR+$7E (64 words) to $0000 with word writes.
clear_scratch:
        movea.l #SCR,a1
        move.w  #64-1,d7
.cl:    move.w  #0,(a1)+
        dbra    d7,.cl
        rts
