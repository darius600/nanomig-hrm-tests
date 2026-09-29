; prog_c2a_noenable.s -- contrast for K2a/K2b (SPECIFICATION.md P2.5): C2a
; with both DSKLEN writes of S6 set to $4000|8000 (bit 15 clear). Both
; p.248 and Appendix A p.278 predict DMAON = 0, and no disk accesses occur.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"

        org     PROGRAM_ORG
        PROG_HEADER

start:
        S0_QUIESCE
        move.w  #$9100,ADKCON(a0)
        move.w  #(CH_DISK)>>16,DSKPTH(a0)
        move.w  #(CH_DISK)&$FFFF,DSKPTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  DSKEN
        S5_ARM_INT
        S6_WAIT_DISK $5F40             ; bit15 clear: DMAEN=0 in both S6 writes
        S7_FINISH_DISK
