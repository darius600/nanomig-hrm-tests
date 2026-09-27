; p1_body.s -- P1 (PB on/off): generic body shared by all four instances.
; A wrapper (p1_ciaa_ta.s, p1_ciaa_tb.s, p1_ciab_ta.s, p1_ciab_tb.s) sets
; the equ symbols below, then includes this file. Mutation wrappers set
; one extra symbol (MUT_PRE11, MUT_PRE12, MUT_PRE13 or MUT_PBON0) first.
;
; Wrapper symbols (SPECIFICATION.md P1):
;   PRB,DDRB,CR,TLO,THI,ICR  -- this instance's CIA register addresses
;   N                        -- the tested bit, 6 or 7
;   M                        -- equ (1<<N), $40 or $80
;   Q1,Q2                    -- this CIA's bits-0-5 patterns
;   DDRB_INIT                -- $00 (CIAA) or $08 (CIAB)
;   CIAB_PREWRITE            -- defined (to anything) only for CIAB
;   DO_O2                    -- defined only for the CIAB/timer-B instance
;
; Results (word each, RESULTS+8 up; EP1_* above already used RESULTS+0..6):
P1_R0           equ     RESULTS+8
P1_A2           equ     RESULTS+10
P1_POLL1        equ     RESULTS+12
P1_I1           equ     RESULTS+14
P1_A1           equ     RESULTS+16
P1_B1           equ     RESULTS+18
P1_C1           equ     RESULTS+20
P1_I2           equ     RESULTS+22
P1_D1           equ     RESULTS+24
P1_D2           equ     RESULTS+26

; O2 markers ($0C1000 is MK_START; DONE is always the run's last marker).
MK_LP_ON_SEL    equ     $0C1002
MK_LP_ON_END    equ     $0C1004
MK_LP_OFF_SEL   equ     $0C1006
MK_LP_OFF_END   equ     $0C1008
MK_A1_SEL       equ     $0C100A
MK_A1_END       equ     $0C100C
MK_C_SEL        equ     $0C100E
MK_C_END        equ     $0C1010
MK_D1_SEL       equ     $0C1012
MK_D1_END       equ     $0C1014
MK_D2_SEL       equ     $0C1016

        org     PROGRAM_ORG

start:
        SETUP

        ifd     DO_O2
; ---- O2/LP: calibration, at the start of the run (before step 1) ------
; LP writes no control register (CRB stays $00, EP1's own reset value),
; and DDRB = $88 (bit 7 and bit 3 both outputs) for both legs.
        move.b  #$88,DDRB
        ifd     MUT_LP
; LP's own contrast: change bit 7 without running R-SELECT. The LED must
; keep its previous state (off, from program entry) in both legs.
        move.b  #$00,PRB
        move.w  #MARKER_SENTINEL,MK_LP_ON_SEL
        move.w  #$0BB8,d7
.mlpd1: dbra    d7,.mlpd1
        move.w  #MARKER_SENTINEL,MK_LP_ON_END
        move.b  #$80,PRB
        move.w  #MARKER_SENTINEL,MK_LP_OFF_SEL
        move.w  #$0BB8,d7
.mlpd2: dbra    d7,.mlpd2
        move.w  #MARKER_SENTINEL,MK_LP_OFF_END
        else
; leg 2: PRB bit 7 = 0, R-SELECT -> LED turns on
        move.b  #$00,PRB
        move.b  #$08,PRB
        move.b  #$00,PRB
        move.w  #MARKER_SENTINEL,MK_LP_ON_SEL
        move.w  #$0BB8,d7
.lpd1:  dbra    d7,.lpd1
        move.w  #MARKER_SENTINEL,MK_LP_ON_END
        move.b  #$08,PRB
; leg 3: PRB bit 7 = 1, R-SELECT -> LED turns off
        move.b  #$80,PRB
        move.b  #$88,PRB
        move.b  #$80,PRB
        move.w  #MARKER_SENTINEL,MK_LP_OFF_SEL
        move.w  #$0BB8,d7
.lpd2:  dbra    d7,.lpd2
        move.w  #MARKER_SENTINEL,MK_LP_OFF_END
        move.b  #$88,PRB
        endc
        endc

; ---- step 1: environment (SPECIFICATION.md 1.2) -----------------------
        ifd     CIAB_PREWRITE
        move.b  #$FF,PRB
        ifd     MUT_PRE11
        move.b  #$FF-M,PRB             ; Pre1.1 contrast: clear bit n first
        endc
        endc
        ifd     MUT_PRE11
        move.b  #DDRB_INIT|M,DDRB
        else
        move.b  #DDRB_INIT,DDRB
        endc
        move.b  #$00,CR

; ---- step 2: pull-up reference -----------------------------------------
        READ_STORE PRB,P1_R0

; ---- step 3: leg A2 (pulse output, stopped) ----------------------------
        ifd     MUT_PBON0
        move.b  #$00,CR
        else
        move.b  #$02,CR
        endc
        READ_STORE PRB,P1_A2

; ---- step 4: make the toggle output low by exactly one underflow ------
        move.b  #$00,CR
        ifd     MUT_PRE12
        RLATCH  TLO,THI,$FFFF
        else
        RLATCH  TLO,THI,$0010
        endc
        ifd     MUT_PBON0
        move.b  #$0D,CR                 ; START,RUNMODE=one-shot,PBON=0
        else
        move.b  #$0F,CR                 ; START,PBON,toggle,one-shot
        endc
        moveq   #0,d1
.poll1: move.b  CR,d0
        addq.w  #1,d1
        btst    #0,d0
        beq.s   .polldone1
        cmp.w   #1000,d1
        blt.s   .poll1
.polldone1:
        move.w  d1,P1_POLL1
        READ_STORE ICR,P1_I1

; ---- step 5: leg A1 (DDRB override, line an input) ---------------------
        ifd     DO_O2
        READ_STORE PRB,P1_A1
        move.b  #$08,PRB
        move.b  #$00,PRB
        move.w  #MARKER_SENTINEL,MK_A1_SEL
        move.w  #$0BB8,d7
.a1d:   dbra    d7,.a1d
        move.w  #MARKER_SENTINEL,MK_A1_END
        move.b  #$08,PRB
        else
        READ_STORE PRB,P1_A1
        endc

; ---- step 6: leg B (PR-data override) ----------------------------------
        move.b  #$3F|M,DDRB
        move.b  #M|Q1,PRB
        READ_STORE PRB,P1_B1

; ---- step 7: leg C (timer output high) ---------------------------------
        move.b  #Q2,PRB
        move.b  #$00,CR
        ifd     MUT_PRE13
        RLATCH  TLO,THI,$0004
        else
        RLATCH  TLO,THI,$FFFF
        endc
        ifd     MUT_PBON0
        move.b  #$05,CR                  ; START,toggle,continuous,PBON=0
        else
        move.b  #$07,CR                  ; START,PBON,toggle,continuous
        endc
        move.w  #$0032,d7
.c1d:   dbra    d7,.c1d
        READ_STORE PRB,P1_C1
        READ_STORE ICR,P1_I2
        ifd     DO_O2
        move.b  #$08,PRB
        move.b  #$00,PRB
        move.w  #MARKER_SENTINEL,MK_C_SEL
        move.w  #$0BB8,d7
.c1sd:  dbra    d7,.c1sd
        move.w  #MARKER_SENTINEL,MK_C_END
        move.b  #$08,PRB
        endc

; ---- step 8: leg D (normal operation) ----------------------------------
        move.b  #$00,CR
        READ_STORE PRB,P1_D1
        ifd     DO_O2
        move.b  #$08,PRB
        move.b  #$00,PRB
        move.w  #MARKER_SENTINEL,MK_D1_SEL
        move.w  #$0BB8,d7
.d1sd:  dbra    d7,.d1sd
        move.w  #MARKER_SENTINEL,MK_D1_END
        move.b  #$08,PRB
        endc
        move.b  #$3F,DDRB
        READ_STORE PRB,P1_D2
        ifd     DO_O2
        move.b  #$08,PRB
        move.b  #$00,PRB
        move.w  #MARKER_SENTINEL,MK_D2_SEL
        move.w  #$0BB8,d7
.d2sd:  dbra    d7,.d2sd
        move.b  #$08,PRB                 ; deselect; DONE is the last marker
        endc

        DONE
