; p9_legw_ctl_body.s -- P9 leg W's live width source: the same start, marker
; and R-SAMPLE(NSAMP; PRB) as p9_legw_body.s with PBON=0, so the timer does
; not drive the pin; the CPU sets PRB bit n right after the start marker and
; clears it after the ICR read that shows the underflow. Each iteration also
; reads ICR. The pin is therefore high from the start until the iteration
; that sees the underflow. Result layout as p9_legw_body.s; P9W_I holds the
; ICR value that showed the underflow (0 if none), P9W_POLL the stop-poll
; count after the sampler.
;
; Wrapper symbols: PRB,DDRB,CR,TLO,THI,ICR,M,DDRB_INIT, SLATCH, NSAMP.
; Optional: LVL1 (DDRB bit n an input for the read before the start).
;
; SPECIFICATION.md P9 leg W (live width source).

P9W_S           equ     RESULTS+8       ; NSAMP words
P9W_POLL        equ     RESULTS+8+300*2 ; stop-poll count
P9W_I           equ     P9W_POLL+2      ; ICR value that showed the underflow
P9W_S0          equ     P9W_POLL+4      ; PRB read right before the start

MK_9W_PRE       equ     $0C1002
MK_9W_POST      equ     $0C1004

        org     PROGRAM_ORG

start:
        SETUP
        ifd     LVL1
        move.b  #DDRB_INIT,DDRB          ; the level before the start is the pull-up's
        else
        move.b  #DDRB_INIT|M,DDRB
        endc
        move.b  #DDRB_INIT,PRB
        move.b  #$00,CR
        READ_STORE PRB,P9W_S0
        move.b  #DDRB_INIT|M,DDRB
        RLATCH  TLO,THI,SLATCH
        clr.w   P9W_I

        move.w  #MARKER_SENTINEL,MK_9W_PRE
        move.b  #$09,CR                  ; START, PBON=0, pulse, one-shot
        move.w  #MARKER_SENTINEL,MK_9W_POST
        move.b  #DDRB_INIT|M,PRB         ; the CPU drives the pin high
        lea     P9W_S,a1
        move.w  #(NSAMP-1),d2
.rs:    moveq   #0,d0
        move.b  PRB,d0
        move.w  d0,(a1)+
        moveq   #0,d3
        move.b  ICR,d3
        btst    #1,d3
        beq.s   .no
        move.w  d3,P9W_I
        move.b  #DDRB_INIT,PRB           ; the CPU drives the pin low
.no:    dbra    d2,.rs
        moveq   #0,d1
.poll:  move.b  CR,d0
        addq.w  #1,d1
        btst    #0,d0
        beq.s   .pd
        cmp.w   #1000,d1
        blt.s   .poll
.pd:    move.w  d1,P9W_POLL
        move.b  #$00,CR

        FINISH
