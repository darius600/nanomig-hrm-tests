; prog_c2a_earlyread.s -- contrast for K2a (SPECIFICATION.md P2.6): C2a with DSKBYTR read right
; after DSKLEN <- $4000 (p.247 step 2), before the two writes that start the transfer. DSKLEN
; bit 15 is clear at that point; both p.248 and Appendix A p.278 predict DMAON = 0 in the value
; read. The transfer is started afterwards exactly as in C2a, so the disk block keeps its
; accesses.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_END COP_MAIN
        move.w  #$9100,ADKCON(a0)
        move.w  #(CH_DISK)>>16,DSKPTH(a0)
        move.w  #(CH_DISK)&$FFFF,DSKPTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  DSKEN
        S5_ARM_INT
        S6_WAIT_DISK_EARLY $DF40
        S7_FINISH_DISK
        VBLANK_HANDLER

        include "ch2build_code.i"
