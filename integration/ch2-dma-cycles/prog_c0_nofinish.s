; prog_c0_nofinish.s -- C1's own contrast (SPECIFICATION.md 2.6, precondition
; C1): as C0, but the STOP loop's own exit condition is removed
; (S6_WAIT_NOEXIT). FRAME_CTR keeps incrementing but nothing ever compares
; it, so the loop never ends and DONE is never reached: the completion
; word is missing.

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
        S6_WAIT_NOEXIT
        VBLANK_HANDLER

        include "ch2build_code.i"
