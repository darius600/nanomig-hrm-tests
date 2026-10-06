; prog_uhres.s -- P12: the register list of the run, then for every frame:
; rewrite the eight sprite pointers, poll VHPOSR until V >= 90, write the
; marker VMARK(90), stop the CPU until the next VERTB (SPECIFICATION.md P12).
;
; The test stores the run's parameters in Chip memory before the program
; starts, and the program reads them before it writes any register:
;   CFG+6   number of frames
;   CFG+8   number of entries of the register list
;   CFG+$10 the register list: pairs of (register offset, value)
; and the null sprite: two zero words at NULLSPR.

        include "nanomig.i"
        include "s2.i"

CFG     equ     $0B8000
NULLSPR equ     $1A0000
M_VMARK         equ     $F020
M_REGS          equ     $F003

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
; the ROM overlay: CIA-A DDRA bit 0 an output, then PRA bit 0 (OVL) low
        move.b  #$03,$BFE201
        move.b  #$00,$BFE001
        lea     irq3(pc),a1
        move.l  a1,($6C).w
        move.l  #0,NULLSPR
        bsr     fsync
        lea     CFG+$10,a1
        move.w  CFG+8,d7
        subq.w  #1,d7
.rl:    move.w  (a1)+,d0
        move.w  (a1)+,d1
        move.w  d1,0(a0,d0.w)
        dbra    d7,.rl
        move.w  #M_REGS,MARKER
        move.w  CFG+6,d6
        subq.w  #1,d6
.frame: bsr     stopframe               ; to the next VERTB
        bsr     sprptrs
.poll:  move.w  VHPOSR(a0),d0
        lsr.w   #8,d0
        cmp.w   #90,d0
        bcs.s   .poll
        move.w  #M_VMARK,MARKER
        dbra    d6,.frame
        bsr     stopframe
        DONE

; sprptrs: every sprite pointer to the null sprite
sprptrs:
        lea     $120(a0),a1
        moveq   #7,d2
.l:     move.w  #(NULLSPR>>16),(a1)+
        move.w  #(NULLSPR&$FFFF),(a1)+
        dbra    d2,.l
        rts

; fsync: wait for VERTB, clear it, write the frame marker.
fsync:  move.w  #$0020,INTREQ(a0)
.p:     move.w  INTREQR(a0),d0
        btst    #5,d0
        beq.s   .p
        move.w  #$0020,INTREQ(a0)
        move.w  #M_FRAME,MARKER
        rts

; stopframe: stop the CPU until the next VERTB, then write the frame marker.
stopframe:
        move.w  #$0020,INTREQ(a0)
        move.w  #$C020,INTENA(a0)
        stop    #$2000
        move.w  #M_FRAME,MARKER
        move.w  #$7FFF,INTENA(a0)
        rts

irq3:   move.w  #$0020,$DFF09C
        rte
