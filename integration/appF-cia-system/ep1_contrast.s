; ep1_contrast.s -- EP1's own contrast (SPECIFICATION.md 1.2, EP1): a write
; to CIAA CRA precedes the read that SETUP normally makes first, so that
; read returns $0A instead of $00. Every other step of the shared
; environment (E2, E3) still runs, in the same order SETUP uses.
        include "nanomig.i"
        include "ciaregs.i"

        org     PROGRAM_ORG
start:
        move.w  #$2700,sr
        lea     CUSTOM,a0
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,INTENA(a0)
        move.b  #ICR_CLEARMASK,CIAA_ICR
        move.b  CIAA_ICR,d0
        move.b  #ICR_CLEARMASK,CIAB_ICR
        move.b  CIAB_ICR,d0
        move.b  #$0A,CIAA_CRA           ; the contrast: written before the read
        READ_STORE CIAA_CRA,EP1_CRA_A
        READ_STORE CIAA_CRB,EP1_CRB_A
        READ_STORE CIAB_CRA,EP1_CRA_B
        READ_STORE CIAB_CRB,EP1_CRB_B
        move.w  #MARKER_SENTINEL,MK_START
        FINISH
