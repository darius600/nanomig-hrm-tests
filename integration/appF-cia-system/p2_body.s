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
;                               (P2's contrasting case sets S4VAL=$0E;
;                               Pre2.3's contrast sets S1VAL=$0D)
;   MUT_PRE21                -- Pre2.1's own contrast (an extra start+latch
;                               before leg R's own write)

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

P2_R1           equ     RESULTS+8
P2_S            equ     RESULTS+10      ; 4*64 words = 512 bytes
P2_Z            equ     P2_S+4*64*2     ; 4 words
P2_I            equ     P2_Z+4*2        ; 4 words
P2_H1           equ     P2_I+4*2
P2_H2           equ     P2_H1+2
P2_H3           equ     P2_H2+2
P2_IH           equ     P2_H3+2

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
LEGS_K          macro   ; \1=k \2=S-array addr \3=Z addr \4=I addr \5=CR start value
                move.w  #MARKER_SENTINEL,MK_S\1_PRE
                move.b  #\5,CR
                move.w  #MARKER_SENTINEL,MK_S\1_POST
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
                READ_STORE PRB,\3
                READ_STORE ICR,\4
                endm

        org     PROGRAM_ORG

start:
        SETUP

; ---- leg R (first actions on this CIA's timer after reset) -------------
        ifd     MUT_PRE21
        RLATCH  TLO,THI,$FFFF
        move.b  #$07,CR
        endc
        ifd     CIAB_PREWRITE
        move.b  #$FF,PRB
        endc
        move.b  #DDRB_INIT,DDRB
        move.b  #$06,CR                  ; PBON,toggle,not started
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
        move.b  #$00,CR
        RLATCH  TLO,THI,SLATCH
        LEGS_K  1,P2_S+0*128,P2_Z+0*2,P2_I+0*2,S1VAL
        LEGS_K  2,P2_S+1*128,P2_Z+1*2,P2_I+1*2,S2VAL
        LEGS_K  3,P2_S+2*128,P2_Z+2*2,P2_I+2*2,S3VAL
        LEGS_K  4,P2_S+3*128,P2_Z+3*2,P2_I+3*2,S4VAL

; ---- leg H (start from high) --------------------------------------------
        move.b  #$06,CR
        RLATCH  TLO,THI,HLATCH
        move.b  #$07,CR
        READ_STORE PRB,P2_H1
        move.b  #$06,CR
        READ_STORE PRB,P2_H2
        move.b  #$07,CR
        READ_STORE PRB,P2_H3
        READ_STORE ICR,P2_IH

        DONE
