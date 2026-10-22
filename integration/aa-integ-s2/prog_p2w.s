; prog_p2w.s -- P2: one-channel blits whose pointer has exactly one address bit
; set (walking one) or exactly one address bit clear (walking zero) among the
; address bits 1-20, for each of A, B, C, D (SPECIFICATION.md P2).
;
; Blit i = 40*channel + 20*mode + (position-1); channel 0..3 = A, B, C, D, mode
; 0 = walking one ($000002 << ...), mode 1 = walking zero ($1FFFFE with the
; bit clear), position 1..20 = the address bit.
;
; Symbols defined before this file is included select the variants:
;   DMA_BLT  DMACON value of step 1 ($8200: DMAEN without BLTEN)
;   SIZE     BLTSIZE value ($0041 = height 1, width 1; $00C1 = height 3)
;   FIXVAL   the constant stored first
;   MUTIDX   the blit whose pointer is shifted one bit up
;   MAPIDX   the blit whose pointer is $0C0000 (the results area)
;   SPARSE   only the address bits 1 and 20 of every channel and mode (16 blits)
;
; Results: RESULTS+0 fixture; RESULTS+$10+4*i the outcome of blit i (1 = BLIT
; seen, 0 = timed out) and DMACONR after it; RESULTS+$400+4*i the pointer
; written to the channel under test.

        include "nanomig.i"
        include "s2.i"

        ifnd    DMA_BLT
DMA_BLT equ     $8240
        endc
        ifnd    SIZE
SIZE    equ     $0041
        endc
        ifnd    FIXVAL
FIXVAL  equ     $BEEF
        endc

PBUF    equ     RESULTS+$800            ; the four pointers of the blit: C, B, A, D

        org     PROGRAM_ORG

start:
        move.w  #FIXVAL,RESULTS         ; the fixture: a known constant, stored first
        lea     CUSTOM,a0
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  #$0000,BPLCON0(a0)
        move.w  #$0000,COPCON(a0)
        move.w  #$0020,BEAMCON0(a0)
        bsr     vsync
        lea     RESULTS+$10,a5
        lea     RESULTS+$400,a4
        moveq   #0,d6                   ; blit index
        moveq   #0,d5                   ; channel under test: 0 A, 1 B, 2 C, 3 D
.chan:  moveq   #0,d4                   ; mode: 0 walking one, 1 walking zero
.mode:  moveq   #1,d7                   ; address bit 1..20
.pos:   moveq   #1,d0
        lsl.l   d7,d0                   ; 1 << position
        tst.w   d4
        beq.s   .one
        move.l  #$1FFFFE,d1
        eor.l   d0,d1
        move.l  d1,d0
.one:
        ifd     MUTIDX
        cmp.w   #MUTIDX,d6
        bne.s   .nomut
        lsl.l   #1,d0
.nomut:
        endc
        ifd     MAPIDX
        cmp.w   #MAPIDX,d6
        bne.s   .nomap
        move.l  #$0C0000,d0
.nomap:
        endc
        lea     PBUF,a2
        move.l  #$0B0000,(a2)           ; C
        move.l  #$0D0000,4(a2)          ; B
        move.l  #$0E0000,8(a2)          ; A
        move.l  #$0F0000,12(a2)         ; D
        lea     chmap(pc),a3
        moveq   #0,d1
        move.b  0(a3,d5.w),d1           ; offset of the channel under test in PBUF
        move.l  d0,0(a2,d1.w)
        move.l  d0,(a4)+                ; the pointer written to the channel under test
        move.w  d5,d2
        add.w   d2,d2
        lea     bc0(pc),a3
        move.w  #DMA_BLT,DMACON(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  0(a3,d2.w),BLTCON0(a0)
        move.w  (a2),BLTCPTH(a0)
        move.w  2(a2),BLTCPTL(a0)
        move.w  4(a2),BLTBPTH(a0)
        move.w  6(a2),BLTBPTL(a0)
        move.w  8(a2),BLTAPTH(a0)
        move.w  10(a2),BLTAPTL(a0)
        move.w  12(a2),BLTDPTH(a0)
        move.w  14(a2),BLTDPTL(a0)
        move.w  #8,BLTCMOD(a0)
        move.w  #16,BLTBMOD(a0)
        move.w  #24,BLTAMOD(a0)
        move.w  #32,BLTDMOD(a0)
        move.w  #$0040,INTREQ(a0)       ; clear BLIT
        move.w  #M_BLIT_GO,d0
        add.w   d6,d0
        move.w  d0,MARKER
        move.w  #SIZE,BLTSIZE(a0)
        move.w  #$0040,d2
        bsr     waitirq
        move.w  d0,(a5)+
        move.w  DMACONR(a0),(a5)+
        move.w  #M_BLIT_DONE,d0
        add.w   d6,d0
        move.w  d0,MARKER
        addq.w  #1,d6
        addq.w  #1,d7
        ifd     SPARSE
        cmp.w   #2,d7
        bne.s   .sp
        moveq   #20,d7
.sp:
        endc
        cmp.w   #21,d7
        bne     .pos
        addq.w  #1,d4
        cmp.w   #2,d4
        bne     .mode
        addq.w  #1,d5
        cmp.w   #4,d5
        bne     .chan
        DONE

chmap:  dc.b    8,4,0,12                ; A, B, C, D: offset in PBUF
bc0:    dc.w    $09F0,$05CC,$03AA,$01FF ; BLTCON0 of A (USEA, USED), B, C, D

; vsync: wait for VERTB, clear it, write the frame marker.
vsync:  move.w  #$0020,INTREQ(a0)
.p:     move.w  INTREQR(a0),d0
        btst    #5,d0
        beq.s   .p
        move.w  #$0020,INTREQ(a0)
        move.w  #M_FRAME,MARKER
        rts

; waitirq: wait for the INTREQR bit(s) in d2; give up after VERTB has been
; seen set three times, each time cleared. d0 = 1 seen, 0 timed out.
waitirq:
        moveq   #3,d3
.l:     move.w  INTREQR(a0),d1
        move.w  d1,d0
        and.w   d2,d0
        bne.s   .seen
        btst    #5,d1
        beq.s   .l
        move.w  #$0020,INTREQ(a0)
        subq.w  #1,d3
        bne.s   .l
        moveq   #0,d0
        rts
.seen:  moveq   #1,d0
        rts
