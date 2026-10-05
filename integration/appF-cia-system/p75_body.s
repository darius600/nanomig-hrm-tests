; p75_body.s -- P7.5/P8.65 mode 01 (bit 5, INMODE: CNT vs phi2). Generic
; body shared by CRA (timer A, P7.5) and CRB (timer B, P8.65 mode 01), on
; both CIAs. CIAA's own run also serves P7.6's input-mode leg and P8.65
; mode 01/10/11's own KP probe timer (the OTHER timer, unchanged over the
; same window) is exercised by the wrapper, not this body.
;
; Wrapper symbols: CR,TLO,THI,ICR -- this instance's own timer.
;   START_BRACKET -- defined by the P7.5 wrappers: a marker is stored before the
;                    start write as well as after it
;   DO_KEY   -- defined only for the CIAA instance (R-KEY is sent)
;   STARTVAL -- $21 (baseline, CNT) or $01 (contrast, phi2)
;
; The wait loop (WAITCNT dbra iterations) is sized so that the measured W
; (SPECIFICATION.md P7.5's own Window precondition) falls in [1500,3000] E
; periods; the C++ side measures and asserts it.
;
; SPECIFICATION.md P7.5, P8.65 mode 01.

        ifnd    WAITCNT
WAITCNT         equ     2200
        endc

P75_I           equ     RESULTS+8
P75_V           equ     RESULTS+10

MK_75_READY     equ     $0C1002
MK_75_WAITEND   equ     $0C1004
MK_75_STOP_A    equ     $0C1006
MK_75_STOP_B    equ     $0C1008
MK_75_START_A   equ     $0C100A

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CR
        RLATCH  TLO,THI,$1000
        moveq   #0,d0
        move.b  ICR,d0                  ; discard
        ifd     START_BRACKET
        move.w  #MARKER_SENTINEL,MK_75_START_A
        endc
        move.b  #STARTVAL,CR
        move.w  #MARKER_SENTINEL,MK_75_READY
        move.w  #WAITCNT,d7
.wd:    dbra    d7,.wd
        move.w  #MARKER_SENTINEL,MK_75_WAITEND
        READ_STORE ICR,P75_I
        move.w  #MARKER_SENTINEL,MK_75_STOP_A
        move.b  #$00,CR
        move.w  #MARKER_SENTINEL,MK_75_STOP_B
        READ_TIMER THI,TLO,P75_V

        DONE
