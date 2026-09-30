; prog_c6_nospren.s -- T3's own contrast (SPECIFICATION.md P7.6): C6
; without SPREN. Sprite fetches are then absent.

        include "nanomig.i"
        include "ch2dma.i"
        include "ch2_skel.i"
        include "ch2build_macros.i"

        org     PROGRAM_ORG

start:
        S0_QUIESCE
        BUILD_L5  COP_MAIN,c6_spr_tab,c6_bpl_tab,COP_SECOND,-1,0,-1
        BUILD_L5B COP_SECOND
        bsr     build_structs_c3
        move.w  #(CH_AUD0)>>16,AUD0LCH(a0)
        move.w  #(CH_AUD0)&$FFFF,AUD0LCL(a0)
        move.w  #16,AUD0LEN(a0)
        move.w  #64,AUD0VOL(a0)
        move.w  #124,AUD0PER(a0)
        move.w  #(CH_AUD1)>>16,AUD1LCH(a0)
        move.w  #(CH_AUD1)&$FFFF,AUD1LCL(a0)
        move.w  #16,AUD1LEN(a0)
        move.w  #64,AUD1VOL(a0)
        move.w  #124,AUD1PER(a0)
        move.w  #(CH_AUD2)>>16,AUD2LCH(a0)
        move.w  #(CH_AUD2)&$FFFF,AUD2LCL(a0)
        move.w  #16,AUD2LEN(a0)
        move.w  #64,AUD2VOL(a0)
        move.w  #124,AUD2PER(a0)
        move.w  #(CH_AUD3)>>16,AUD3LCH(a0)
        move.w  #(CH_AUD3)&$FFFF,AUD3LCL(a0)
        move.w  #16,AUD3LEN(a0)
        move.w  #64,AUD3VOL(a0)
        move.w  #124,AUD3PER(a0)
        move.w  #$9100,ADKCON(a0)
        move.w  #(CH_DISK)>>16,DSKPTH(a0)
        move.w  #(CH_DISK)&$FFFF,DSKPTL(a0)
        move.w  #(CH_SPR0)>>16,SPR0PTH(a0)
        move.w  #(CH_SPR0)&$FFFF,SPR0PTL(a0)
        move.w  #(CH_SPR0+$0400)>>16,SPR1PTH(a0)
        move.w  #(CH_SPR0+$0400)&$FFFF,SPR1PTL(a0)
        move.w  #(CH_SPR0+$0800)>>16,SPR2PTH(a0)
        move.w  #(CH_SPR0+$0800)&$FFFF,SPR2PTL(a0)
        move.w  #(CH_SPR0+$0C00)>>16,SPR3PTH(a0)
        move.w  #(CH_SPR0+$0C00)&$FFFF,SPR3PTL(a0)
        move.w  #(CH_SPR0+$1000)>>16,SPR4PTH(a0)
        move.w  #(CH_SPR0+$1000)&$FFFF,SPR4PTL(a0)
        move.w  #(CH_SPR0+$1400)>>16,SPR5PTH(a0)
        move.w  #(CH_SPR0+$1400)&$FFFF,SPR5PTL(a0)
        move.w  #(CH_SPR0+$1800)>>16,SPR6PTH(a0)
        move.w  #(CH_SPR0+$1800)&$FFFF,SPR6PTL(a0)
        move.w  #(CH_SPR0+$1C00)>>16,SPR7PTH(a0)
        move.w  #(CH_SPR0+$1C00)&$FFFF,SPR7PTL(a0)
        move.w  #$4200,BPLCON0(a0)
        move.w  #0,BPLCON1(a0)
        move.w  #0,BPL1MOD(a0)
        move.w  #0,BPL2MOD(a0)
        move.w  #$2C81,DIWSTRT(a0)
        move.w  #$2CC1,DIWSTOP(a0)
        move.w  #$0038,DDFSTRT(a0)
        move.w  #$00D0,DDFSTOP(a0)
        move.w  #(CH_BPL1)>>16,BPL1PTH(a0)
        move.w  #(CH_BPL1)&$FFFF,BPL1PTL(a0)
        move.w  #(CH_BPL2)>>16,BPL2PTH(a0)
        move.w  #(CH_BPL2)&$FFFF,BPL2PTL(a0)
        move.w  #(CH_BPL3)>>16,BPL3PTH(a0)
        move.w  #(CH_BPL3)&$FFFF,BPL3PTL(a0)
        move.w  #(CH_BPL4)>>16,BPL4PTH(a0)
        move.w  #(CH_BPL4)&$FFFF,BPL4PTL(a0)
        S3_COPPER COP_MAIN
        S4_DMAEN  AUD0123EN|DSKEN|BPLEN
        S5_ARM_INT
        S6_WAIT_DISK $DF40
        S7_FINISH_DISK
        VBLANK_HANDLER

c6_spr_tab:     dc.l    CH_SPR0,CH_SPR0+$0400,CH_SPR0+$0800,CH_SPR0+$0C00
                dc.l    CH_SPR0+$1000,CH_SPR0+$1400,CH_SPR0+$1800,CH_SPR0+$1C00
c6_bpl_tab:     dc.l    CH_BPL1,CH_BPL2,CH_BPL3,CH_BPL4,0,0

        include "ch2build_code.i"
