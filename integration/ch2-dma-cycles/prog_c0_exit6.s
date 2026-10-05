; prog_c0_exit6.s -- the contrast of the FIXTURE-PEEK check: as C0
; (SPECIFICATION.md 2.9), but the wait loop exits when FRAME_CTR reaches 6
; instead of 7. The program still reaches DONE, and the stored FRAME_CTR is 6.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_END COP_MAIN
        S3_COPPER COP_MAIN
        S4_DMAEN  0
        S5_ARM_INT
        lea     CUSTOM,a0
.wait6  stop    #$2000
        cmp.w   #6,FRAME_CTR
        bne.s   .wait6
        S7_FINISH
        VBLANK_HANDLER

        include "ch2build_code.i"
