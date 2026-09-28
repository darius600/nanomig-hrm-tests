; kp_b.s -- KP (keyboard idle), CIAA: probes timer A (CRA<-$21, CNT) while
; timer B runs the case's own start (CRB<-$21, mode 01 CNT), no R-KEY sent.
; Used as the precondition for P8.65 mode 01's CIAA leg and reused, as the
; same mechanism, for P8.65 mode 11's own KP.
                include "nanomig.i"
                include "ciaregs.i"

KPB_V           equ     RESULTS+8

MK_KPB_READY    equ     $0C1002
MK_KPB_END      equ     $0C1004

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CIAA_CRA
        move.b  #$00,CIAA_CRB
        RLATCH  CIAA_TALO,CIAA_TAHI,$1000
        RLATCH  CIAA_TBLO,CIAA_TBHI,$1000
        moveq   #0,d0
        move.b  CIAA_ICR,d0             ; discard
        move.b  #$21,CIAA_CRB            ; case's own start (timer B, mode 01 CNT)
        move.b  #$21,CIAA_CRA            ; probe start (timer A, CNT)
        move.w  #MARKER_SENTINEL,MK_KPB_READY
        move.w  #2200,d7
.wd:    dbra    d7,.wd
        move.w  #MARKER_SENTINEL,MK_KPB_END
        move.b  #$00,CIAA_CRB
        move.b  #$00,CIAA_CRA
        READ_TIMER CIAA_TAHI,CIAA_TALO,KPB_V

        DONE
