; p865_ciaa_mode11.s -- P8.65 mode 11, CIAA only (CIAB's CNT has no
; driver). The CRB value (mode 10, $41, vs mode 11, $61) is read from the
; parameter block (E7), so runs X and Y are the same assembled program; the
; C++ side pokes PARAM_A's low byte before boot, and optionally sends
; R-KEY during the window (pair K vs pair N).
;
; SPECIFICATION.md P8.65 mode 11.
                include "nanomig.i"
                include "ciaregs.i"

        ifnd    WAITCNT
WAITCNT         equ     2200
        endc

P811_I          equ     RESULTS+8
P811_V          equ     RESULTS+10

MK_811_A        equ     $0C1002         ; before the bracketed CRA<-$01 write
MK_811_READY    equ     $0C1004         ; after it (the "ready" marker)
MK_811_WAITEND  equ     $0C1006
MK_811_STOP_A   equ     $0C1008         ; before the bracketed CRA<-$00 write
MK_811_STOP_B   equ     $0C100A

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CIAA_CRA
        RLATCH  CIAA_TALO,CIAA_TAHI,3
        move.b  #$00,CIAA_CRB
        RLATCH  CIAA_TBLO,CIAA_TBHI,$FFFF
        move.b  PARAM_A+1,CIAA_CRB      ; $41 (mode 10) or $61 (mode 11), E7
        move.w  #MARKER_SENTINEL,MK_811_A
        move.b  #$01,CIAA_CRA
        move.w  #MARKER_SENTINEL,MK_811_READY
        move.w  #WAITCNT,d7
.wd:    dbra    d7,.wd
        move.w  #MARKER_SENTINEL,MK_811_WAITEND
        READ_STORE CIAA_ICR,P811_I
        move.w  #MARKER_SENTINEL,MK_811_STOP_A
        move.b  #$00,CIAA_CRA
        move.w  #MARKER_SENTINEL,MK_811_STOP_B
        move.b  #$00,CIAA_CRB
        READ_TIMER CIAA_TBHI,CIAA_TBLO,P811_V

        FINISH
