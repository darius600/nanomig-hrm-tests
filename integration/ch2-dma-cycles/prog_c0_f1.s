; prog_c0_f1.s -- contrast for F1 (SPECIFICATION.md 2.1): C0, but after every
; STOP wake-up the CPU reads one word of the Copper block (COP_MAIN+$100).
; That CPU access is attributed to a declared DMA block inside the observed
; frames, and the CPU's accesses lie on another phase of the memory cycle than
; the DMA channels', so F1 fails while F2 and F3 hold.

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
.waitf  stop    #$2000
        tst.w   COP_MAIN+$100
        cmp.w   #7,FRAME_CTR
        bne.s   .waitf
        S7_FINISH
        VBLANK_HANDLER

        include "ch2build_code.i"
