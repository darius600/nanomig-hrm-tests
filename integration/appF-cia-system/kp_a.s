; kp_a.s -- KP (keyboard idle), CIAA: probes timer B (CRB<-$21, CNT) while
; timer A runs the case's own start (CRA<-$21, CNT), no R-KEY sent. Used as
; the precondition for P7.5's CIAA leg and reused, as the same mechanism,
; for P8.65 modes 01 and 11's own KP (SPECIFICATION.md 1.5 KP: "The probe
; is always the other timer").
                include "nanomig.i"
                include "ciaregs.i"

KPA_V           equ     RESULTS+8

MK_KPA_READY    equ     $0C1002
MK_KPA_END      equ     $0C1004

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CIAA_CRA
        move.b  #$00,CIAA_CRB
        RLATCH  CIAA_TALO,CIAA_TAHI,$1000
        RLATCH  CIAA_TBLO,CIAA_TBHI,$1000
        moveq   #0,d0
        move.b  CIAA_ICR,d0             ; discard
        move.b  #$21,CIAA_CRA            ; case's own start (timer A, CNT)
        move.b  #$21,CIAA_CRB            ; probe start (timer B, CNT)
        move.w  #MARKER_SENTINEL,MK_KPA_READY
        move.w  #2200,d7
.wd:    dbra    d7,.wd
        move.w  #MARKER_SENTINEL,MK_KPA_END
        move.b  #$00,CIAA_CRA
        move.b  #$00,CIAA_CRB
        READ_TIMER CIAA_TBHI,CIAA_TBLO,KPA_V

        DONE
