; ctl_toggle_body.s -- a live toggle source: PBON=0 (the timer does not drive
; the pin), DDRB bit 7 an output, and the CPU inverts PRB data bit 7 once
; after every ICR read that shows the TB flag. Result layout as p3_body.s
; (RESULTS+8: 600 PRB words, then 600 ICR words), so the same graders apply.
;
; Wrapper symbols: LATCHVAL; INITLEVEL (PRB value written before the start);
; optional SKIP (invert only after every second flagged read).
;
; SPECIFICATION.md P3 and P9 leg F (live toggle source).
                include "nanomig.i"
                include "ciaregs.i"
P3_PRB          equ     RESULTS+8
P3_ICR          equ     P3_PRB+600*2

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$80,CIAB_DDRB
        moveq   #0,d5
        move.b  #INITLEVEL,d5
        move.b  d5,CIAB_PRB
        moveq   #0,d7
        move.b  #$00,CIAB_CRB
        RLATCH  CIAB_TBLO,CIAB_TBHI,LATCHVAL
        moveq   #0,d0
        move.b  CIAB_ICR,d0             ; discard
        move.b  #$01,CIAB_CRB           ; START, PBON=0, continuous
        lea     P3_PRB,a1
        lea     P3_ICR,a2
        move.w  #599,d2
.rs:    moveq   #0,d0
        move.b  CIAB_PRB,d0
        move.w  d0,(a1)+
        moveq   #0,d0
        move.b  CIAB_ICR,d0
        move.w  d0,(a2)+
        btst    #1,d0
        beq.s   .no
        ifd     SKIP
        addq.b  #1,d7
        btst    #0,d7
        beq.s   .no
        endc
        eori.b  #$80,d5
        move.b  d5,CIAB_PRB
.no:    dbra    d2,.rs
        move.b  #$00,CIAB_CRB

        DONE
