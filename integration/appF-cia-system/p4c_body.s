; p4c_body.s -- P4's live pulse source: P4's own R-SAMPLE(400; PRB, ICR) loop
; and running timer (PBON=0, so the timer does not drive the pin), with the
; CPU supplying the pulse on the pin. Each iteration reads the timer's own
; low byte x (the count of the timer under test, which is below 256), writes
; DDRB bit n's data level from a table (high when x == XHI, low otherwise),
; and then samples PRB and ICR as P4 does. The pulse therefore recurs at one
; fixed phase of the timer's own period, whatever that period is.
;
; Wrapper symbols: PRB,DDRB,CR,TLO,THI,ICR,M, LATCHVAL, CRVAL, XHI;
; optional XHI2 (a second high count value; default none).
;
; SPECIFICATION.md P4 (live pulse source).

P4_PRB          equ     RESULTS+8       ; 400 words
P4_ICR          equ     P4_PRB+400*2    ; 400 words

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #M,DDRB
        move.b  #$00,PRB
        move.b  #$00,CR
        RLATCH  TLO,THI,LATCHVAL
        moveq   #0,d0
        move.b  ICR,d0                  ; discard
        move.b  #CRVAL,CR
        lea     P4_PRB,a1
        lea     P4_ICR,a2
        lea     pulsetab(pc),a3
        move.w  #399,d2
.rs:    moveq   #0,d0
        move.b  TLO,d0
        move.b  (a3,d0.w),d1
        move.b  d1,PRB
        moveq   #0,d0
        move.b  PRB,d0
        move.w  d0,(a1)+
        moveq   #0,d0
        move.b  ICR,d0
        move.w  d0,(a2)+
        dbra    d2,.rs
        move.b  #$00,CR

        FINISH

        ifnd    XHI2
XHI2            equ     -1
        endc
pulsetab:
TABI    set     0
        rept    256
        if      (TABI==XHI)|(TABI==XHI2)
        dc.b    M
        else
        dc.b    $00
        endif
TABI    set     TABI+1
        endr
