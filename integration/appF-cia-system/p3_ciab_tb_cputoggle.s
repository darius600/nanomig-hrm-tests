; p3_ciab_tb_cputoggle.s -- P3-GREENCTL: a proven-live input through P3's
; own pairing comparator. PBON=0 (the timer's own toggle output is not
; selected), DDRB bit 7 an output, and the CPU itself inverts PRB data bit
; 7 once after every ICR read that shows the TB flag. Same result layout
; as p3_body.s (RESULTS+8: 600 PRB words, then 600 ICR words), so the same
; grader applies.
;
; SPECIFICATION.md P3 ("Green input"), question 4 of the repair report.
                include "nanomig.i"
                include "ciaregs.i"
P3_PRB          equ     RESULTS+8
P3_ICR          equ     P3_PRB+600*2

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$80,CIAB_DDRB
        moveq   #0,d5
        move.b  #$FF,d5
        move.b  d5,CIAB_PRB
        move.b  #$00,CIAB_CRB
        RLATCH  CIAB_TBLO,CIAB_TBHI,60
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
        eori.b  #$80,d5
        move.b  d5,CIAB_PRB
.no:    dbra    d2,.rs
        move.b  #$00,CIAB_CRB

        DONE
