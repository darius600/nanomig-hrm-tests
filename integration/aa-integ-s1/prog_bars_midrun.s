; prog_bars_midrun.s -- P4's shape: S1+S2+S3 exactly as prog_bars.s, but
; BPLCON1 is never written up front. Instead the program counts VERTB
; edges (R5: clear INTREQ bit 5, poll INTREQR bit 5) and, on reaching the
; configured edge (the VERTB that starts F5 -- AA p.25 "Start of Vertical
; blank"), writes BPLCON1 once, then keeps counting a few more edges
; before idling, so the observed frame (F5) fully elapses before the
; simulation is stopped from the harness side. Shares CFG's field layout
; with prog_bars.s for the common S1/S2 setup; BPLCON1 is deliberately
; left out of that block's write (CFG_BPLCON1_WR is not read here at all).

        include "nanomig.i"

BEAMCON0        equ     $1DC
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088
DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094
BPLCON0         equ     $100
BPLCON1         equ     $102
BPLCON2         equ     $104
BPLCON3         equ     $106
BPL1MOD         equ     $108
BPL2MOD         equ     $10A
BPLCON4         equ     $10C
BPL1PTH         equ     $0E0
BPL1PTL         equ     $0E2
BPL2PTH         equ     $0E4
BPL2PTL         equ     $0E6
FMODE           equ     $1FC
COLOR01         equ     $182
COLOR02         equ     $184
COLOR03         equ     $186

CFG             equ     $0B0000
CFG_BPLCON0     equ     CFG+0
CFG_BPLCON4     equ     CFG+6
CFG_FMODE       equ     CFG+8
CFG_DDFSTRT     equ     CFG+10
CFG_DDFSTOP     equ     CFG+12
CFG_DMACON      equ     CFG+14
CFG_BEAMCON0    equ     CFG+22  ; BEAMCON0 value (PAL $0020)
CFG_WAITLO      equ     CFG+24  ; low word of every Copper WAIT of the S3 reload ($0007: horizontal position $06)

CFG2_VAL        equ     CFG+16  ; value written to BPLCON1 at CFG2_EDGE
CFG2_EDGE       equ     CFG+18  ; VERTB edge count (1-based) to write at
CFG2_STOPEDGE   equ     CFG+20  ; VERTB edge count after which the program idles

BUF1            equ     $0B1000
BUF2            equ     $0B1100
COPLIST         equ     $0B2000

        org     PROGRAM_ORG

start:
        ifnd    NOFIX
        move.w  #$BEEF,RESULTS
        endc

        lea     CUSTOM,a0

        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  CFG_BEAMCON0,d0
        move.w  d0,BEAMCON0(a0)

        move.w  CFG_BPLCON0,d0
        move.w  d0,BPLCON0(a0)
        move.w  #$0024,BPLCON2(a0)
        move.w  #$0C00,BPLCON3(a0)
        move.w  CFG_BPLCON4,d0
        move.w  d0,BPLCON4(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$F0C1,DIWSTOP(a0)
        move.w  CFG_DDFSTRT,d0
        move.w  d0,DDFSTRT(a0)
        move.w  CFG_DDFSTOP,d0
        move.w  d0,DDFSTOP(a0)
        move.w  #$0000,BPL1MOD(a0)
        move.w  #$0000,BPL2MOD(a0)
        move.w  CFG_FMODE,d0
        move.w  d0,FMODE(a0)
        move.w  #$0222,COLOR00(a0)
        move.w  #$0F00,COLOR01(a0)
        move.w  #$00F0,COLOR02(a0)
        move.w  #$000F,COLOR03(a0)

        ; No BPLCON1 write here: the Copper list below also contains no
        ; MOVE to $102, exactly as SPECIFICATION.md P4 requires.

        lea     COPLIST,a1
        move.w  #$2C,d2
.genloop:
        move.w  d2,d0
        lsl.w   #8,d0
        or.w    CFG_WAITLO,d0
        move.w  d0,(a1)+
        move.w  #$FFFE,(a1)+
        move.w  #BPL1PTH,(a1)+
        move.w  #(BUF1>>16)&$FFFF,(a1)+
        move.w  #BPL1PTL,(a1)+
        move.w  #BUF1&$FFFF,(a1)+
        move.w  #BPL2PTH,(a1)+
        move.w  #(BUF2>>16)&$FFFF,(a1)+
        move.w  #BPL2PTL,(a1)+
        move.w  #BUF2&$FFFF,(a1)+
        addq.w  #1,d2
        cmp.w   #$F0,d2
        bne.s   .genloop
        move.w  #$FFFF,(a1)+
        move.w  #$FFFE,(a1)+

        move.w  #(COPLIST>>16)&$FFFF,COP1LCH(a0)
        move.w  #COPLIST&$FFFF,COP1LCL(a0)
        move.w  #0,COPJMP1(a0)

        move.w  CFG_DMACON,d0
        move.w  d0,DMACON(a0)

        ; R5: count VERTB edges. At CFG2_EDGE, write BPLCON1 = CFG2_VAL.
        moveq   #0,d3
.edgeloop:
        move.w  #$0020,INTREQ(a0)
.poll:  move.w  INTREQR(a0),d0
        btst    #5,d0
        beq.s   .poll
        addq.w  #1,d3
        cmp.w   CFG2_EDGE,d3
        bne.s   .notarget
        move.w  CFG2_VAL,d1
        move.w  d1,BPLCON1(a0)
.notarget:
        cmp.w   CFG2_STOPEDGE,d3
        bne.s   .edgeloop

.idle:  bra.s   .idle
