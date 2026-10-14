; p87_ciaa_body.s -- P8.7, CIAA (CRB bit 7, ALARM). Run X and run Y are separate runs from reset; no wait
; before T0 and no sampler (SPECIFICATION.md P8.7, "Stimulus, CIAA"). The CRB value of the TOD writes is
; read from the parameter block (E7): $80 in run X (alarm), $00 in run Y (TOD clock).
;
; Wrapper symbols: RUN_X or RUN_Y.
                include "nanomig.i"
                include "ciaregs.i"

P87_T0          equ     RESULTS+8       ; long
P87_T1X         equ     RESULTS+12      ; long
P87_T1Y         equ     RESULTS+16      ; long

MK_87_T0_A      equ     $0C1002
MK_87_T0_B      equ     $0C1004
MK_87_T1X       equ     $0C1006
MK_87_T1Y       equ     $0C1008

        org     PROGRAM_ORG

start:
        SETUP

        move.b  #$00,CIAA_CRB
        WRTOD   CIAA_TODHI,CIAA_TODMID,CIAA_TODLO,$000000
        move.w  #MARKER_SENTINEL,MK_87_T0_A
        RDTOD24 CIAA_TODHI,CIAA_TODMID,CIAA_TODLO
        move.l  d0,P87_T0
        move.w  #MARKER_SENTINEL,MK_87_T0_B
        moveq   #0,d0
        move.b  CIAA_ICR,d0             ; discard

        ifd     RUN_X
; ---- run X: CRB<-$80 (alarm), A = T0+$1000 -------------------------------
        move.b  PARAM_A+1,CIAA_CRB      ; $80 (E7)
        move.l  P87_T0,d1
        addi.l  #$1000,d1
        move.l  d1,d2
        lsr.l   #8,d2
        lsr.l   #8,d2
        move.b  d2,CIAA_TODHI
        move.l  d1,d2
        lsr.l   #8,d2
        move.b  d2,CIAA_TODMID
        move.b  d1,CIAA_TODLO
        move.b  #$00,CIAA_CRB
        move.w  #MARKER_SENTINEL,MK_87_T1X
        RDTOD24 CIAA_TODHI,CIAA_TODMID,CIAA_TODLO
        move.l  d0,P87_T1X
        endc

        ifd     RUN_Y
; ---- run Y: CRB<-$00 (TOD clock), A = $001000 ----------------------------
        move.b  PARAM_A+1,CIAA_CRB      ; $00 (E7)
        move.b  #$00,CIAA_TODHI
        move.b  #$10,CIAA_TODMID
        move.b  #$00,CIAA_TODLO
        move.b  #$00,CIAA_CRB
        move.w  #MARKER_SENTINEL,MK_87_T1Y
        RDTOD24 CIAA_TODHI,CIAA_TODMID,CIAA_TODLO
        move.l  d0,P87_T1Y
        endc

        FINISH
