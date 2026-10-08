; prog_c0_htotal.s -- contrast for F3 (SPECIFICATION.md 2.3): C0, but S2 enables the
; programmable beam counters (BEAMCON0 VARBEAMEN, PAL kept) and writes HTOTAL = $E0, two
; counts below the $E2 of an ordinary PAL line. Every line is then 225 memory cycles
; long. Nothing else changes: the Copper list is L0, no channel but refresh runs, the CPU
; sleeps in STOP, and the refresh set is attributed as in C0.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_END COP_MAIN
        move.w  #$00E0,HTOTAL(a0)
        move.w  #$00A0,BEAMCON0(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  0
        S5_ARM_INT
        S6_WAIT
        S7_FINISH
        VBLANK_HANDLER

        include "ch2build_code.i"
