; ctl_toggle_body.s -- a live toggle source: PBON=0 (the timer does not drive
; the pin), DDRB bit 7 an output, and the CPU inverts PRB data bit 7 once
; after every ICR read that shows the TB flag. Result layout as p3_body.s
; (RESULTS+8: 600 PRB words, then 600 ICR words, then one more PRB word), so the same graders apply.
;
; Wrapper symbols: LATCHVAL; INITLEVEL (PRB value written before the start);
; optional SKIP (invert only after every second flagged read); optional CTLM (the PB bit, default $80),
; CTLCR, CTLLO, CTLHI (the timer's control, low and high count registers, default CIAB timer B) and
; CTLFLAG (the ICR bit number of the timer's flag, default 1).
;
; SPECIFICATION.md P3 and P9 leg F (live toggle source).
                include "nanomig.i"
                include "ciaregs.i"
        ifnd    CTLM
CTLM            equ     $80
CTLCR           equ     CIAB_CRB
CTLLO           equ     CIAB_TBLO
CTLHI           equ     CIAB_TBHI
CTLFLAG         equ     1
        endc
P3_PRB          equ     RESULTS+8
P3_ICR          equ     P3_PRB+600*2
P3_PRB_LAST     equ     P3_ICR+600*2

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #CTLM,CIAB_DDRB
        moveq   #0,d5
        move.b  #INITLEVEL,d5
        move.b  d5,CIAB_PRB
        moveq   #0,d7
        move.b  #$00,CTLCR
        RLATCH  CTLLO,CTLHI,LATCHVAL
        moveq   #0,d0
        move.b  CIAB_ICR,d0             ; discard
        move.b  #$01,CTLCR             ; START, PBON=0, continuous
        lea     P3_PRB,a1
        lea     P3_ICR,a2
        move.w  #599,d2
.rs:    moveq   #0,d0
        move.b  CIAB_PRB,d0
        move.w  d0,(a1)+
        moveq   #0,d0
        move.b  CIAB_ICR,d0
        move.w  d0,(a2)+
        btst    #CTLFLAG,d0
        beq.s   .no
        ifd     SKIP
        addq.b  #1,d7
        btst    #0,d7
        beq.s   .no
        endc
        eori.b  #CTLM,d5
        move.b  d5,CIAB_PRB
.no:    dbra    d2,.rs
        moveq   #0,d0
        move.b  CIAB_PRB,d0
        move.w  d0,P3_PRB_LAST
        move.b  #$00,CTLCR

        FINISH
