; prog_c2a_xdsk.s -- run X-dsk (SPECIFICATION.md P2.6): C2a with list L5
; instead of L0, and DSKPTH/L pointing at the start of the Copper block
; (COP_MAIN) instead of the disk block. The 16000 bytes from COP_MAIN hold
; the Copper block and its padding only, so the declared "disk block" for
; this one run is [COP_MAIN, COP_MAIN+16000) -- the C++ test's attribution
; reflects that for this run alone. As X-aud, L5's own sprite pointers are
; the null block and its bitplane pointers are zero (SPREN/BPLEN are never
; set for this run).

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_L5  COP_MAIN,xdsk_spr_tab,xdsk_bpl_tab,COP_SECOND,-1,0,-1
        BUILD_L5B COP_SECOND
        move.w  #$9100,ADKCON(a0)
        move.w  #(COP_MAIN)>>16,DSKPTH(a0)
        move.w  #(COP_MAIN)&$FFFF,DSKPTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  DSKEN
        S5_ARM_INT
        S6_WAIT_DISK $DF40
        S7_FINISH_DISK
        VBLANK_HANDLER

xdsk_spr_tab:   dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
                dc.l    CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL,CH_SPR_NULL
xdsk_bpl_tab:   dc.l    0,0,0,0,0,0

        include "ch2build_code.i"
