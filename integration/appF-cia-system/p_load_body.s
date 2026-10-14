; p_load_body.s -- P7.4/P8.4 (bit 4, LOAD / force load strobe): stopped leg
; then running leg, in one program. Generic body shared by CRA (timer A)
; and CRB (timer B), on both CIAs.
;
; Wrapper symbols: CR,TLO,THI,ICR.
; Optional: MUT_LD0 (contrast: CR<-$00 instead of $10 in the stopped leg)
;           MUT_RUN01 (contrast: CR<-$01 instead of $11 in the running leg)
;           MUT_NOSTART (contrast: CR<-$00 instead of $01 in the stopped leg: the
;                        counter never moves)
;
; SPECIFICATION.md P7.4/P8.4.

PLD_V1          equ     RESULTS+8
PLD_V2          equ     RESULTS+10
PLD_C           equ     RESULTS+12
PLD_V3          equ     RESULTS+14

MK_LD_A         equ     $0C1002         ; before the load/start write
MK_LD_B         equ     $0C1004         ; after the load/start write
MK_LD_C         equ     $0C1006         ; before the stop write
MK_LD_D         equ     $0C1008         ; after the stop write
MK_LD_RUNSTART  equ     $0C100A         ; after the running leg's own start write

        org     PROGRAM_ORG

start:
        SETUP

; ---- stopped leg --------------------------------------------------------
        move.b  #$00,CR
        RLATCH  TLO,THI,$4000
        ifd     MUT_NOSTART
        move.b  #$00,CR
        else
        move.b  #$01,CR
        endc
        move.w  #$07D0,d7              ; ~2000 E periods
.d1:    dbra    d7,.d1
        move.b  #$00,CR
        READ_TIMER THI,TLO,PLD_V1
        ifd     MUT_LD0
        move.b  #$00,CR                ; contrast: "writing a 0 has no effect"
        else
        move.b  #$10,CR                ; LOAD, START=0
        endc
        READ_TIMER THI,TLO,PLD_V2
        READ_STORE CR,PLD_C

; ---- running leg ---------------------------------------------------------
        move.b  #$00,CR
        RLATCH  TLO,THI,$FFFF
        move.b  #$01,CR
        move.w  #MARKER_SENTINEL,MK_LD_RUNSTART
        move.w  #$1388,d7              ; ~5000 E periods
.d2:    dbra    d7,.d2

        move.w  #MARKER_SENTINEL,MK_LD_A
        ifd     MUT_RUN01
        move.b  #$01,CR                 ; contrast: no LOAD, still running
        else
        move.b  #$11,CR                 ; START, LOAD (strobe)
        endc
        move.w  #MARKER_SENTINEL,MK_LD_B
        move.w  #$0064,d7              ; ~100 E periods
.d3:    dbra    d7,.d3
        move.w  #MARKER_SENTINEL,MK_LD_C
        move.b  #$00,CR                 ; stop
        move.w  #MARKER_SENTINEL,MK_LD_D
        READ_TIMER THI,TLO,PLD_V3

        FINISH
