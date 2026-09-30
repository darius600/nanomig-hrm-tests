; prog_c0.s -- configuration C0 (SPECIFICATION.md 2.9): DMACON bits none
; besides DMAEN|COPEN; Copper list L0 (END only), built by this program's
; own S1 step; COP1LC points at it.

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
        S6_WAIT
        S7_FINISH
        VBLANK_HANDLER

        include "ch2build_code.i"
