; blit.i -- the blit engine of P1-P6 (template of SPECIFICATION.md 2.3).
;
; The including program defines blit_table and any variant symbols before
; this file is included:
;   DMA_BLT  value written to DMACON in step 1 (default $8240: DMAEN, BLTEN)
;   TOUCH    byte address a CPU read touches right after BLTSIZE is written
;   HOOK     defined: p7.i is included and its subroutine hook is called once,
;            after the initial state and before the first blit
;
; A table entry is 16 words:
;   flags BLTCON0 BLTCON1 BLTSIZE  CPTH CPTL BPTH BPTL APTH APTL DPTH DPTL
;   CMOD BMOD AMOD DMOD
; flags bit 0 set: no register is written for this blit except BLTSIZE.
; flags bit 1 set: no pointer and no modulo register is written for this blit.
; The table is preceded by its entry count minus one.
;
; Results, from RESULTS: +0 fixture $BEEF; +$10 + 4*i: blit i's outcome
; (1 = BLIT request seen, 0 = timed out), +$12 + 4*i: DMACONR read after it.

        include "nanomig.i"
        include "s2.i"

        ifnd    DMA_BLT
DMA_BLT equ     $8240
        endc
        ifnd    FIXVAL
FIXVAL  equ     $BEEF
        endc

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
        ifd     HOOK
        bsr     hook
        endc
        lea     blit_table(pc),a1
        lea     RESULTS+$10,a3
        move.w  (a1)+,d7
        moveq   #0,d6
.next:  move.w  (a1),d5                 ; flags
        move.w  6(a1),d4                ; BLTSIZE value
        btst    #0,d5
        bne.s   .norw
        move.w  #DMA_BLT,DMACON(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  4(a1),BLTCON1(a0)
        move.w  2(a1),BLTCON0(a0)
        btst    #1,d5
        bne.s   .norw
        move.w  8(a1),BLTCPTH(a0)
        move.w  10(a1),BLTCPTL(a0)
        move.w  12(a1),BLTBPTH(a0)
        move.w  14(a1),BLTBPTL(a0)
        move.w  16(a1),BLTAPTH(a0)
        move.w  18(a1),BLTAPTL(a0)
        move.w  20(a1),BLTDPTH(a0)
        move.w  22(a1),BLTDPTL(a0)
        move.w  24(a1),BLTCMOD(a0)
        move.w  26(a1),BLTBMOD(a0)
        move.w  28(a1),BLTAMOD(a0)
        move.w  30(a1),BLTDMOD(a0)
.norw:  move.w  #$0040,INTREQ(a0)       ; clear BLIT
        move.w  #M_BLIT_GO,d0
        add.w   d6,d0
        move.w  d0,MARKER
        move.w  d4,BLTSIZE(a0)
        ifd     TOUCH
        move.w  TOUCH,d2
        endc
        move.w  #$0040,d2
        bsr     waitirq
        move.w  d0,(a3)+
        move.w  DMACONR(a0),(a3)+
        move.w  #M_BLIT_DONE,d0
        add.w   d6,d0
        move.w  d0,MARKER
        lea     32(a1),a1
        addq.w  #1,d6
        dbra    d7,.next
        DONE

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

        ifd     HOOK
        include "p7.i"
        endc

; the table follows at a fixed address, so that a test can read what was written
        org     $080C00
