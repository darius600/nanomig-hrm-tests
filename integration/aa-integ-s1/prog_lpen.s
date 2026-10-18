; prog_lpen.s -- P10: S1 step 1, no write to BPLCON0, then three times:
; clear INTREQ bit 5, poll INTREQR bit 5 (R5), store a marker word, read
; VPOSR as a long word (w0 = VPOSR:VHPOSR), wait a fixed loop, read again (w1),
; store w0 and w1. Symbols defined before the include of this file select the
; contrast builds:
;   LPEN   BPLCON0 = $0008 (LPEN = 1) is written before step 4 (contrast C)
;   LATE   the first read follows the marker by LATE dbra iterations
;   VAR    VARBEAMEN with HTOTAL = $00E8 and VTOTAL = $0138: lines of 233 CCK
;   SAME   w1 is a copy of w0 (one read of VPOSR): a live input for the comparators
;   NOFIX  no fixture store
; Before the S1 writes the program stores CIAA PRA bits 7-6 at RESULTS+6.
        include "nanomig.i"

OUT     equ     $0B9000                 ; w0 and w1 as long words, three times
MARKS   equ     $0B9100                 ; one marker word per time
BEAMCON0 equ    $1DC
BPLCON0 equ     $100

        org     PROGRAM_ORG

start:
        ifnd    NOFIX
        move.w  #$BEEF,RESULTS
        endc
        moveq   #0,d5
        move.b  $BFE001,d5              ; CIAA PRA: bits 7-6 are /FIR1 /FIR0
        and.w   #$00C0,d5
        move.w  d5,RESULTS+6
        lea     CUSTOM,a0
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        ifd     VAR
        move.w  #$00A0,BEAMCON0(a0)
        move.w  #$00E8,$1C0(a0)
        move.w  #$0138,$1C8(a0)
        else
        move.w  #$0020,BEAMCON0(a0)     ; PAL, LPENDIS = 0
        endc
        ifd     LPEN
        move.w  #$0008,BPLCON0(a0)
        endc
        lea     OUT,a1
        lea     MARKS,a2
        moveq   #2,d3
.again: move.w  #$0020,INTREQ(a0)
.poll:  move.w  INTREQR(a0),d0
        btst    #5,d0
        beq.s   .poll
        move.w  d3,(a2)+                ; the marker: its port time is t_V
        ifd     LATE
        move.w  #LATE,d2
.late:  dbra    d2,.late
        endc
        move.l  VPOSR(a0),d1            ; w0
        moveq   #12,d2
.wait:  dbra    d2,.wait
        ifd     SAME
        move.l  d1,d4                   ; w1 = w0
        else
        move.l  VPOSR(a0),d4            ; w1
        endc
        move.l  d1,(a1)+
        move.l  d4,(a1)+
        dbra    d3,.again
        DONE
