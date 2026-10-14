; p5a_body.s -- P5 leg (a), differential: CR<-$00; R-LATCH($FFFF);
; bracketed CR<-$01 (START, phi2, continuous); a delay; bracketed CR<-$00;
; read HI,LO->V (twice, for Pre5.2); read ICR->i. Generic body shared by
; CRA (timer A) and CRB (timer B), on both CIAs.
;
; Wrapper symbols: CR,TLO,THI,ICR, DELAYCNT (the dbra count for the delay).
; Optional: MUT_PRE51 (R-LATCH($1000) instead of $FFFF)
;           MUT_PRE52 (no stop write before the second V read)
;           MUT_CNT (CR<-$21, CNT, instead of $01, phi2 -- the contrasting case)
;           MUT_NOSTART (CR<-$00 instead of the start write: the timer never runs)
;
; SPECIFICATION.md P5.

P5A_V           equ     RESULTS+8
P5A_V2          equ     RESULTS+10
P5A_I           equ     RESULTS+12

MK_5A_A         equ     $0C1002         ; before the start bracket's write
MK_5A_B         equ     $0C1004         ; after it
MK_5A_C         equ     $0C1006         ; before the stop bracket's write
MK_5A_D         equ     $0C1008         ; after it

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CR
        ifd     MUT_PRE51
        RLATCH  TLO,THI,$1000
        else
        RLATCH  TLO,THI,$FFFF
        endc
        move.w  #MARKER_SENTINEL,MK_5A_A
        ifd     MUT_CNT
        move.b  #$21,CR                 ; contrast: CNT instead of phi2
        else
        ifd     MUT_NOSTART
        move.b  #$00,CR                 ; no start: the counter holds
        else
        move.b  #$01,CR                 ; START, phi2, continuous
        endc
        endc
        move.w  #MARKER_SENTINEL,MK_5A_B
        move.w  #DELAYCNT,d7
.d1:    dbra    d7,.d1
        move.w  #MARKER_SENTINEL,MK_5A_C
        ifd     MUT_PRE52
        else
        move.b  #$00,CR                 ; stop
        endc
        move.w  #MARKER_SENTINEL,MK_5A_D
        READ_TIMER THI,TLO,P5A_V
        READ_TIMER THI,TLO,P5A_V2
        READ_STORE ICR,P5A_I

        FINISH
