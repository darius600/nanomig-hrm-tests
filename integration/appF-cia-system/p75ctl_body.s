; p75ctl_body.s -- P7.5-CIAB-GREENCTL, CIAA: one timer A in CRA INMODE=CNT, started
; twice with the same code, once with no CNT edge in the interval and once with the
; eight edges of one key code (R-KEY, sent by the C++ side after the second ready
; marker). Each interval ends with a bracketed stop and a read of the stopped counter,
; so both reads are made on a stopped timer.
;
; Wrapper symbols: none. CTL_WAIT1 and CTL_WAIT2 (dbra counts of the two intervals)
; have defaults.
;
; SPECIFICATION.md P7.5 (control).

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
        move.b  #$00,CIAA_CRA
        RLATCH  CIAA_TALO,CIAA_TAHI,$1000
        moveq   #0,d0
        move.b  CIAA_ICR,d0             ; discard

        move.w  #MARKER_SENTINEL,MK_C1_START_A
        move.b  #$21,CIAA_CRA           ; START, CNT
        move.w  #MARKER_SENTINEL,MK_C1_READY
        move.w  #CTL_WAIT1,d7
.w1:    dbra    d7,.w1
        move.w  #MARKER_SENTINEL,MK_C1_STOP_A
        move.b  #$00,CIAA_CRA
        move.w  #MARKER_SENTINEL,MK_C1_STOP_B
        READ_TIMER CIAA_TAHI,CIAA_TALO,CTL_V1
        READ_STORE CIAA_ICR,CTL_I1

        move.w  #MARKER_SENTINEL,MK_C2_START_A
        move.b  #$21,CIAA_CRA           ; START, CNT: the counter continues from its value
        move.w  #MARKER_SENTINEL,MK_C2_READY
        move.w  #CTL_WAIT2,d7
.w2:    dbra    d7,.w2
        READ_STORE CIAA_ICR,CTL_I2
        move.w  #MARKER_SENTINEL,MK_C2_STOP_A
        move.b  #$00,CIAA_CRA
        move.w  #MARKER_SENTINEL,MK_C2_STOP_B
        READ_TIMER CIAA_TAHI,CIAA_TALO,CTL_V2

        DONE
