; prog_c2a.s -- configuration C2a (SPECIFICATION.md 2.9): disk write
; direction (the channel reads memory). List L0; DMACON bit DSKEN.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_END COP_MAIN
        move.w  #$9100,ADKCON(a0)              ; SET, MFMPREC, FAST (p.249)
        move.w  #(CH_DISK)>>16,DSKPTH(a0)
        move.w  #(CH_DISK)&$FFFF,DSKPTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  DSKEN
        S5_ARM_INT
        S6_WAIT_DISK $DF40                     ; DMAEN|WRITE|8000 (p.246 Table 8-6)
        S7_FINISH_DISK
        VBLANK_HANDLER

        include "ch2build_code.i"
