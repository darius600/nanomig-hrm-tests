; prog_c2a_dskpt.s -- contrast for K2b (SPECIFICATION.md P2.5): C2a with
; DSKPTH/L at $00C000, outside the declared disk block. Both DSKLEN writes
; keep bit 15 set, so DMAON in DSKBYTR is 1 (K2a holds), and the transfer
; runs, but none of its accesses falls into the disk block: K2b fails.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_END COP_MAIN
        move.w  #$9100,ADKCON(a0)
        move.w  #($00C000)>>16,DSKPTH(a0)
        move.w  #($00C000)&$FFFF,DSKPTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  DSKEN
        S5_ARM_INT
        S6_WAIT_DISK $DF40
        S7_FINISH_DISK
        VBLANK_HANDLER

        include "ch2build_code.i"
