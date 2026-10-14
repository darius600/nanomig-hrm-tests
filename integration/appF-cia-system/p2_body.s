; p2_body.s -- P2 (toggle low after RES, high on every start): generic
; body shared by all four instances. A wrapper sets the equ symbols below,
; then includes this file.
;
; Wrapper symbols (SPECIFICATION.md P2):
;   PRB,DDRB,CR,TLO,THI,ICR  -- this instance's CIA register addresses
;   N,M                      -- tested bit (6 or 7) and its mask
;   DDRB_INIT                -- $00 (CIAA) or $08 (CIAB)
;   CIAB_PREWRITE            -- defined only for CIAB
;   DO_O2                    -- defined only for the CIAB/timer-B instance
; Optional (default if not set by the wrapper):
;   SLATCH  (default $0020)  -- leg S's own latch value (Pre2.2's contrast: $FFFF)
;   HLATCH  (default $FFFF)  -- leg H's own latch value (Pre2.4's contrast: $0008)
;   S1VAL..S4VAL (default $0F each) -- leg S's four start-write values
;   H1VAL (default $07), H3VAL (default $07), STOPVAL (default $06) -- leg H's
;                               start, second start and stop writes
;   MUT_PRE23_Z1             -- Pre2.3's own contrast: DDRB bit n is released for
;                               z_1's read only (with S1VAL=$0D, PBON off)
;
; Control mode (the CPU stands in for the timer's output on the pin):
;   CPU_DRIVE                -- PBON is 0 in every CR value the wrapper sets
;                               ($04/$0D/$05 in place of $06/$0F/$07), DDRB bit n
;                               is an output, and the CPU writes PRB bit n: low
;                               before any start, high right after every start
;                               write, low again after a one-shot has ended.
;   NORAISE1..NORAISE4       -- no CPU raise after leg S's start k
;   NORAISE_H1               -- no CPU raise after leg H's first start
;   MUT_LEGR_STARTED         -- a start, with the CPU raising the line, before
;                               leg R's own write
;
; Pre2.1's contrast:
;   MUT_PRE21                -- R-LATCH($FFFF) and CR<-$07 (START, PBON, toggle,
;                               continuous) before leg R's own write

        ifnd    SLATCH
SLATCH          equ     $0020
        endc
        ifnd    HLATCH
HLATCH          equ     $FFFF
        endc
        ifnd    S1VAL
S1VAL           equ     $0F
        endc
        ifnd    S2VAL
S2VAL           equ     $0F
        endc
        ifnd    S3VAL
S3VAL           equ     $0F
        endc
        ifnd    S4VAL
S4VAL           equ     $0F
        endc
        ifnd    H1VAL
H1VAL           equ     $07
        endc
        ifnd    H3VAL
H3VAL           equ     $07
        endc
        ifnd    STOPVAL
STOPVAL         equ     $06
        endc

P2_R1           equ     RESULTS+8
P2_S            equ     RESULTS+10      ; 4*64 words = 512 bytes
P2_Z            equ     P2_S+4*64*2     ; 4 words
P2_I            equ     P2_Z+4*2        ; 4 words
P2_H1           equ     P2_I+4*2
P2_H2           equ     P2_H1+2
P2_H3           equ     P2_H2+2
P2_IH           equ     P2_H3+2
P2_POLL         equ     P2_IH+2         ; 4 words, one per start (Pre2.2)
P2_S0           equ     P2_POLL+4*2     ; the driven level right before start 1
P2_CR0          equ     P2_S0+2         ; CR, read right before leg R's own write (Pre2.1)

MK_R_SEL        equ     $0C1002
MK_R_END        equ     $0C1004
MK_S1_PRE       equ     $0C1006
MK_S1_POST      equ     $0C1008
MK_S2_PRE       equ     $0C100A
MK_S2_POST      equ     $0C100C
MK_S3_PRE       equ     $0C100E
MK_S3_POST      equ     $0C1010
MK_S4_PRE       equ     $0C1012
MK_S4_POST      equ     $0C1014

; ---- one leg-S start, its own 64-sample loop, its own stop-poll --------
LEGS_K          macro   ; \1=k \2=S-array addr \3=Z addr \4=I addr \5=CR start value \6=poll-count addr
                move.w  #MARKER_SENTINEL,MK_S\1_PRE
                move.b  #\5,CR
                move.w  #MARKER_SENTINEL,MK_S\1_POST
                ifd     CPU_DRIVE
                ifnd    NORAISE\1
                move.b  #DDRB_INIT|M,PRB        ; the CPU sets the line high at the start
                endc
                endc
                lea     \2,a1
                moveq   #63,d2
.rs\1:          moveq   #0,d0
                move.b  PRB,d0
                move.w  d0,(a1)+
                dbra    d2,.rs\1
                moveq   #0,d1
.poll\1:        move.b  CR,d0
                addq.w  #1,d1
                btst    #0,d0
                beq.s   .pd\1
                cmp.w   #1000,d1
                blt.s   .poll\1
.pd\1:
                move.w  d1,\6
                ifd     CPU_DRIVE
                move.b  #DDRB_INIT,PRB          ; the CPU toggles the line low at the underflow
                endc
                ifd     MUT_PRE23_Z\1
                move.b  #DDRB_INIT,DDRB         ; the line is released for z_k's read
                endc
                READ_STORE PRB,\3
                ifd     MUT_PRE23_Z\1
                move.b  #DDRB_INIT|M,DDRB       ; and driven again
                move.b  #DDRB_INIT,PRB
                endc
                READ_STORE ICR,\4
                endm

        org     PROGRAM_ORG

start:
        SETUP

; ---- leg R (first actions on this CIA's timer after reset) -------------
        ifd     CIAB_PREWRITE
        move.b  #$FF,PRB
        endc
        ifd     CPU_DRIVE
        move.b  #DDRB_INIT|M,DDRB
        move.b  #DDRB_INIT,PRB           ; the CPU drives the line low
        ifd     MUT_LEGR_STARTED
        RLATCH  TLO,THI,$FFFF
        move.b  #$05,CR                  ; START, PBON off, toggle, continuous
        move.b  #DDRB_INIT|M,PRB         ; the CPU raises the line at the start
        endc
        READ_STORE CR,P2_CR0             ; Pre2.1: CR before leg R's own write
        move.b  #$04,CR                  ; toggle, not started
        else
        move.b  #DDRB_INIT,DDRB
        ifd     MUT_PRE21
        RLATCH  TLO,THI,$FFFF
        move.b  #$07,CR                  ; START,PBON,toggle,continuous
        endc
        READ_STORE CR,P2_CR0             ; Pre2.1: CR before leg R's own write
        move.b  #$06,CR                  ; PBON,toggle,not started
        endc
        READ_STORE PRB,P2_R1
        ifd     DO_O2
        move.b  #$08,PRB
        move.b  #$00,PRB
        move.w  #MARKER_SENTINEL,MK_R_SEL
        move.w  #$0BB8,d7
.rd:    dbra    d7,.rd
        move.w  #MARKER_SENTINEL,MK_R_END
        move.b  #$08,PRB
        endc

; ---- leg S (start from low, four times) --------------------------------
; DDRB bit n=1 / PR bit n=0 (SPECIFICATION.md P2): the level before each
; start is a driven 0, not an undriven pull-up.
        move.b  #$00,CR
        move.b  #DDRB_INIT|M,DDRB
        move.b  #DDRB_INIT,PRB
        READ_STORE PRB,P2_S0           ; the level right before start 1
        RLATCH  TLO,THI,SLATCH
        LEGS_K  1,P2_S+0*128,P2_Z+0*2,P2_I+0*2,S1VAL,P2_POLL+0*2
        LEGS_K  2,P2_S+1*128,P2_Z+1*2,P2_I+1*2,S2VAL,P2_POLL+1*2
        LEGS_K  3,P2_S+2*128,P2_Z+2*2,P2_I+2*2,S3VAL,P2_POLL+2*2
        LEGS_K  4,P2_S+3*128,P2_Z+3*2,P2_I+3*2,S4VAL,P2_POLL+3*2

; ---- leg H (start from high) --------------------------------------------
; Same DDRB bit n = 1 / PR bit n = 0 override as leg S (still holds from
; leg S; restated here so leg H does not depend on leg S's own ordering).
        move.b  #STOPVAL,CR
        move.b  #DDRB_INIT|M,DDRB
        move.b  #DDRB_INIT,PRB
        RLATCH  TLO,THI,HLATCH
        move.b  #H1VAL,CR
        ifd     CPU_DRIVE
        ifnd    NORAISE_H1
        move.b  #DDRB_INIT|M,PRB         ; the CPU sets the line high at the start
        endc
        endc
        READ_STORE PRB,P2_H1
        move.b  #STOPVAL,CR
        READ_STORE PRB,P2_H2
        move.b  #H3VAL,CR
        ifd     CPU_DRIVE
        move.b  #DDRB_INIT|M,PRB         ; the CPU sets the line high at the start
        endc
        READ_STORE PRB,P2_H3
        READ_STORE ICR,P2_IH

        FINISH
