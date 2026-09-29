; P7.6-CIAB-GREENCTL: P7.6-CIAB's own program (SPMODE output, SDR<-$A5,
; R-SAMPLE(300; ICR)), run on CIA-A instead of CIA-B. Shows whether the
; same sampler and the same "at least one ICR sample has SP=1" comparator
; can go green through a proven-live input.
                include "nanomig.i"
                include "ciaregs.i"

P76_ICR         equ     RESULTS+8       ; 300 words

        org     PROGRAM_ORG

start:
        SETUP
        move.b  #$00,CIAA_CRA
        RLATCH  CIAA_TALO,CIAA_TAHI,10
        moveq   #0,d0
        move.b  CIAA_ICR,d0             ; discard
        move.b  #$41,CIAA_CRA           ; START, SPMODE output, continuous
        move.b  #$A5,CIAA_SDR
        RSAMPLE1 300,P76_ICR,CIAA_ICR
        move.b  #$00,CIAA_CRA

        DONE
