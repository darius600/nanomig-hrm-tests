; p87_ciaa.s -- P8.7, CIAA (CRB bit 7, ALARM). No wait before T0, no
; sampler (SPECIFICATION.md P8.7, "Stimulus, CIAA").
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

; ---- run X: CRA-wait no; CRB<-$80 (alarm), A = T0+$1000 -----------------
        move.b  #$80,CIAA_CRB
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

; ---- run Y: CRB stays $00, A = $001000 ----------------------------------
        move.b  #$00,CIAA_CRB
        move.b  #$00,CIAA_TODHI
        move.b  #$10,CIAA_TODMID
        move.b  #$00,CIAA_TODLO
        move.w  #MARKER_SENTINEL,MK_87_T1Y
        RDTOD24 CIAA_TODHI,CIAA_TODMID,CIAA_TODLO
        move.l  d0,P87_T1Y

        DONE
