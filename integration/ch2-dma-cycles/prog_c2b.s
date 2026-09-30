; prog_c2b.s -- configuration C2b (SPECIFICATION.md 2.9): disk read
; direction (the channel writes memory). As C2a but the S6 value has no
; WRITE bit; WORDSYNC stays 0 (D9).

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
        S6_WAIT_DISK $9F40                     ; DMAEN|8000, no WRITE bit (read)
        S7_FINISH_DISK
        VBLANK_HANDLER

        include "ch2build_code.i"
