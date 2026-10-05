; p9_legw_body.s -- P9 leg W (width): P2 leg S's own single-start mechanics
; (DDRB bit n=1 / PR bit n=0 override, one bracketed start, R-SAMPLE(N;PRB)),
; with a configurable latch and sample count so the sampler passes the
; underflow for both L=40 and L=100. The level before the start (Pre2.3) is
; read and stored; after the sampler the one-shot's stop-poll and ICR read are
; stored (Pre2.2), as in P2 leg S.
;
; Wrapper symbols: PRB,DDRB,CR,TLO,THI,N,M,DDRB_INIT, SLATCH, NSAMP.
; Optional: LVL1 (leave DDRB bit n an input, so the level before the start
; is the pull-up's 1).
;           MUT_CHIP (each sample reads a Chip-memory byte, not PRB: no CIA access
;           in the sampler loop)
;
; SPECIFICATION.md P9 leg W (reusing P2 leg S).

P9W_S           equ     RESULTS+8       ; NSAMP words
P9W_POLL        equ     RESULTS+8+300*2 ; stop-poll count
P9W_I           equ     P9W_POLL+2      ; ICR read after the stop-poll
P9W_S0          equ     P9W_POLL+4      ; PRB read right before the start

MK_9W_PRE       equ     $0C1002
MK_9W_POST      equ     $0C1004

        org     PROGRAM_ORG

start:
        SETUP
        ifd     LVL1
        move.b  #DDRB_INIT,DDRB
        else
        move.b  #DDRB_INIT|M,DDRB
        endc
        move.b  #DDRB_INIT,PRB
        move.b  #$00,CR
        READ_STORE PRB,P9W_S0
        RLATCH  TLO,THI,SLATCH

        move.w  #MARKER_SENTINEL,MK_9W_PRE
        move.b  #$0F,CR                  ; START, PBON, toggle, one-shot
        move.w  #MARKER_SENTINEL,MK_9W_POST
        lea     P9W_S,a1
        move.w  #(NSAMP-1),d2
.rs:    moveq   #0,d0
        ifd     MUT_CHIP
        move.b  CHIPBYTE_K3,d0
        else
        move.b  PRB,d0
        endc
        move.w  d0,(a1)+
        dbra    d2,.rs
        moveq   #0,d1
.poll:  move.b  CR,d0
        addq.w  #1,d1
        btst    #0,d0
        beq.s   .pd
        cmp.w   #1000,d1
        blt.s   .poll
.pd:    move.w  d1,P9W_POLL
        READ_STORE ICR,P9W_I
        move.b  #$00,CR

        DONE
