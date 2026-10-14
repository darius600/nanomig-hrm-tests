; p87_ciab_body.s -- P8.7, CIAB (CRB bit 7, ALARM). Run X and run Y are separate runs from reset:
; run X sets the alarm (CRB<-$80 for the TOD writes) with A=T0+12 and expects the ALRM flag to
; appear; run Y writes TOD directly (CRB<-$00) with A=$001000 and expects it not to. The CRB value
; of the TOD writes is read from the parameter block (E7). SPECIFICATION.md P8.7.
;
; Wrapper symbols: RUN_X or RUN_Y. Optional: MUT_NOLSB (the LSB of the setup's TOD<-0 write is
; never written, so TOD stays stopped), SAMPLES (iterations of the sampler, default 450).
                include "nanomig.i"
                include "ciaregs.i"

        ifnd    SAMPLES
SAMPLES         equ     450
        endc

P87_T0          equ     RESULTS+8       ; long
P87_T1X         equ     RESULTS+12      ; long
P87_T1Y         equ     RESULTS+16      ; long
P87_XTOD        equ     RESULTS+20      ; 450 words
P87_XICR        equ     P87_XTOD+450*2  ; 450 words
P87_YTOD        equ     P87_XICR+450*2  ; 450 words
P87_YICR        equ     P87_YTOD+450*2  ; 450 words

MK_87_WR        equ     $0C1010         ; right after the TOD<-0 write (Pre8.7's own start mark)
MK_87_T0_A      equ     $0C1002
MK_87_T0_B      equ     $0C1004
MK_87_T1X       equ     $0C1006
MK_87_T1Y       equ     $0C1008

        org     PROGRAM_ORG

start:
        SETUP

; ---- setup: CRB<-$00, TOD<-0, wait >=20 lines, read T0 -----------------
        move.b  #$00,CIAB_CRB
        ifd     MUT_NOLSB
        move.b  #$00,CIAB_TODHI         ; Pre8.7 contrast: LSB never written, TOD stays stopped (p344)
        move.b  #$00,CIAB_TODMID
        else
        WRTOD   CIAB_TODHI,CIAB_TODMID,CIAB_TODLO,$000000
        endc
        move.w  #MARKER_SENTINEL,MK_87_WR
        move.w  #$1388,d7              ; generous margin over 20 lines
.wd0:   dbra    d7,.wd0
        move.w  #MARKER_SENTINEL,MK_87_T0_A
        RDTOD24 CIAB_TODHI,CIAB_TODMID,CIAB_TODLO
        move.l  d0,P87_T0
        move.w  #MARKER_SENTINEL,MK_87_T0_B
        moveq   #0,d0
        move.b  CIAB_ICR,d0             ; discard

        ifd     RUN_X
; ---- run X: CRB<-$80 (alarm), A = T0+12 ---------------------------------
        move.b  PARAM_A+1,CIAB_CRB      ; $80 (E7)
        move.l  P87_T0,d1
        addi.l  #12,d1
        move.l  d1,d2
        lsr.l   #8,d2
        lsr.l   #8,d2
        move.b  d2,CIAB_TODHI
        move.l  d1,d2
        lsr.l   #8,d2
        move.b  d2,CIAB_TODMID
        move.b  d1,CIAB_TODLO
        move.b  #$00,CIAB_CRB
        move.w  #MARKER_SENTINEL,MK_87_T1X
        RDTOD24 CIAB_TODHI,CIAB_TODMID,CIAB_TODLO
        move.l  d0,P87_T1X
        RSAMPLE2 SAMPLES,P87_XTOD,CIAB_TODLO,P87_XICR,CIAB_ICR
        endc

        ifd     RUN_Y
; ---- run Y: CRB<-$00 (TOD clock), A = $001000 ----------------------------
        move.b  PARAM_A+1,CIAB_CRB      ; $00 (E7)
        move.b  #$00,CIAB_TODHI
        move.b  #$10,CIAB_TODMID
        move.b  #$00,CIAB_TODLO
        move.b  #$00,CIAB_CRB
        move.w  #MARKER_SENTINEL,MK_87_T1Y
        RDTOD24 CIAB_TODHI,CIAB_TODMID,CIAB_TODLO
        move.l  d0,P87_T1Y
        RSAMPLE2 SAMPLES,P87_YTOD,CIAB_TODLO,P87_YICR,CIAB_ICR
        endc

        FINISH
