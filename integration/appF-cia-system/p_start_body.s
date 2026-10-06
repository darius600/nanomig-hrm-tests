; p_start_body.s -- P7.0/P8.0 (bit 0, START): generic body shared by CRA
; (timer A) and CRB (timer B), on both CIAs. A wrapper sets CR,TLO,THI,ICR,
; ICRMASK, then includes this file.
;
; Wrapper symbols:
;   CR,TLO,THI,ICR  -- this instance's control/timer/ICR register addresses
;   ICRMASK         -- ICR_TA or ICR_TB
; Optional: MUT_ST_PRE (R-LATCH($0100): an underflow happens), MUT_ST_CONTRAST (CR<-$01 during the
;           first delay), MUT_ST_NOSTART (CR<-$00 as the start write)
;
; SPECIFICATION.md P7.0/P8.0.

PST_U1          equ     RESULTS+8
PST_U2          equ     RESULTS+10
PST_U3          equ     RESULTS+12
PST_ICR         equ     RESULTS+14

MK_ST_A         equ     $0C1002
MK_ST_B         equ     $0C1004
MK_SP_A         equ     $0C1006
MK_SP_B         equ     $0C1008

        org     PROGRAM_ORG

start:
        SETUP

        move.b  #$00,CR
        ifd     MUT_ST_PRE
        RLATCH  TLO,THI,$0100           ; Pre-underflow contrast: TA/TB sets before u3
        else
        RLATCH  TLO,THI,$8000
        endc
        READ_TIMER THI,TLO,PST_U1

        move.w  #$0BB8,d7              ; ~1000 E periods
.d1:    dbra    d7,.d1
        ifd     MUT_ST_CONTRAST
        move.b  #$01,CR                 ; contrast: started during the first delay, u2<>u1
        endc
        READ_TIMER THI,TLO,PST_U2

        move.w  #MARKER_SENTINEL,MK_ST_A
        ifd     MUT_ST_NOSTART
        move.b  #$00,CR                 ; contrast: the start write leaves START=0, u3=u2
        else
        move.b  #$01,CR                 ; START
        endc
        move.w  #MARKER_SENTINEL,MK_ST_B

        move.w  #$0BB8,d7              ; ~1000 E periods
.d2:    dbra    d7,.d2

        move.w  #MARKER_SENTINEL,MK_SP_A
        move.b  #$00,CR                 ; STOP
        move.w  #MARKER_SENTINEL,MK_SP_B
        READ_TIMER THI,TLO,PST_U3
        READ_STORE ICR,PST_ICR

        DONE
