; p9_legw_body.s -- P9 leg W (width): P2 leg S's own single-start mechanics
; (DDRB bit n=1 / PR bit n=0 override, one bracketed start, R-SAMPLE(N;PRB)),
; with a configurable latch and sample count so the sampler passes the
; underflow for both L=40 and L=100.
;
; Wrapper symbols: PRB,DDRB,CR,TLO,THI,N,M,DDRB_INIT, SLATCH, NSAMP.
;
; SPECIFICATION.md P9 leg W (reusing P2 leg S).

P9W_S           equ     RESULTS+8       ; NSAMP words

MK_9W_PRE       equ     $0C1002
MK_9W_POST      equ     $0C1004

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #DDRB_INIT|M,DDRB
        move.b  #DDRB_INIT,PRB
        move.b  #$00,CR
        RLATCH  TLO,THI,SLATCH

        move.w  #MARKER_SENTINEL,MK_9W_PRE
        move.b  #$0F,CR                  ; START, PBON, toggle, one-shot
        move.w  #MARKER_SENTINEL,MK_9W_POST
        lea     P9W_S,a1
        move.w  #(NSAMP-1),d2
.rs:    moveq   #0,d0
        move.b  PRB,d0
        move.w  d0,(a1)+
        dbra    d2,.rs
        move.b  #$00,CR

        DONE
