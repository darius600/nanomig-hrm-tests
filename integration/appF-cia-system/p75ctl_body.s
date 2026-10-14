; p75ctl_body.s -- two-phase run on CIAA (P7.5's CIAA-INMODE line and control; P8.65 mode 01's control): one
; timer in INMODE=CNT, started twice with the same code, once with no CNT edge in the interval and once
; with the eight edges of one key code (R-KEY, sent by the C++ side after the first or the second ready
; marker). Each interval ends with a bracketed stop and a read of the stopped counter, so both reads are
; made on a stopped timer.
;
; Wrapper symbols: CR,TLO,THI,ICR -- the timer's own control, counter and ICR registers. Optional:
; C1_STARTVAL (the first interval's start write, default $21; $20 leaves the timer stopped),
; CTL_WAIT1 and CTL_WAIT2 (dbra counts of the two intervals).
;
; SPECIFICATION.md P7.5 and P8.65 mode 01 (controls).

        ifnd    C1_STARTVAL
C1_STARTVAL     equ     $21
        endc
        ifnd    CTL_WAIT1
CTL_WAIT1       equ     500
        endc
        ifnd    CTL_WAIT2
CTL_WAIT2       equ     2200
        endc

CTL_V1          equ     RESULTS+8       ; counter after the interval with no edge
CTL_I1          equ     RESULTS+10      ; ICR read before the second start
CTL_I2          equ     RESULTS+12      ; ICR read in the second interval
CTL_V2          equ     RESULTS+14      ; counter after the interval with the key

MK_C1_START_A   equ     $0C1002
MK_C1_READY     equ     $0C1004
MK_C1_STOP_A    equ     $0C1006
MK_C1_STOP_B    equ     $0C1008
MK_C2_START_A   equ     $0C100A
MK_C2_READY     equ     $0C100C
MK_C2_STOP_A    equ     $0C100E
MK_C2_STOP_B    equ     $0C1010

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CR
        RLATCH  TLO,THI,$1000
        moveq   #0,d0
        move.b  ICR,d0             ; discard

        move.w  #MARKER_SENTINEL,MK_C1_START_A
        move.b  #C1_STARTVAL,CR         ; START, CNT
        move.w  #MARKER_SENTINEL,MK_C1_READY
        move.w  #CTL_WAIT1,d7
.w1:    dbra    d7,.w1
        move.w  #MARKER_SENTINEL,MK_C1_STOP_A
        move.b  #$00,CR
        move.w  #MARKER_SENTINEL,MK_C1_STOP_B
        READ_TIMER THI,TLO,CTL_V1
        READ_STORE ICR,CTL_I1

        move.w  #MARKER_SENTINEL,MK_C2_START_A
        move.b  #$21,CR           ; START, CNT: the counter continues from its value
        move.w  #MARKER_SENTINEL,MK_C2_READY
        move.w  #CTL_WAIT2,d7
.w2:    dbra    d7,.w2
        READ_STORE ICR,CTL_I2
        move.w  #MARKER_SENTINEL,MK_C2_STOP_A
        move.b  #$00,CR
        move.w  #MARKER_SENTINEL,MK_C2_STOP_B
        READ_TIMER THI,TLO,CTL_V2

        FINISH
