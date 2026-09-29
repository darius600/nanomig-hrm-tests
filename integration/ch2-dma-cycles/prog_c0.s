; prog_c0.s -- configuration C0 (SPECIFICATION.md 2.9): DMACON bits none
; besides DMAEN|COPEN; Copper list L0 (END only). The Copper list content
; (just $FFFF,$FFFE) is poked by the C++ test before boot(); this program
; only points COP1LC at it.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"

        org     PROGRAM_ORG
        PROG_HEADER

start:
        S0_QUIESCE
        S3_COPPER COP_MAIN
        S4_DMAEN  0
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
