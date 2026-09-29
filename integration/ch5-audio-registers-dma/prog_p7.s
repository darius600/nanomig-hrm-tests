; prog_p7.s -- P7: with BLTPRI (bit 10) set, the CPU gets no Chip-memory
; cycle while blitter DMA runs (SPECIFICATION.md P7). No markers: the
; window is anchored on Q's own first/last destination write. This file
; alone (no symbol set) is the MAIN build (nasty mode, full 2048-iteration
; loop). Variants (each its own source, including this one with a symbol
; set):
;   prog_p7_c1.s   (C1 set)       -- BLTPRI clear (share mode)
;   prog_p7_g1.s   (G1_BREAK)     -- branch-to-self before DONE
;   prog_p7_g2.s   (G2_BREAK)     -- BLTPRI/DMAEN left out of the enabling write
;   prog_p7_g3.s   (G3_TINY)      -- a one-word blit (fails "at least 2 writes")
;   prog_p7_gstop.s(G_STOP)       -- loop runs once, then STOP #$2700

        include "ch5.i"

        org     PROGRAM_ORG

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     RSTORE,a3
        lea     GLOG,a4

        move.w  #$07FF,DMACON(a0)
        move.w  #$01FF,BLTCON0(a0)
        move.w  #0,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTDMOD(a0)
        move.l  #QQ,BLTDPTH(a0)

        ifd     C1
        move.w  #$8240,DMACON(a0)       ; BLTPRI clear (share)
        else
        ifd     G2_BREAK
        move.w  #$8600,DMACON(a0)       ; BLTEN left out
        else
        move.w  #$8640,DMACON(a0)       ; SET,BLTPRI,DMAEN,BLTEN
        endif
        endif
        move.w  DMACONR(a0),(a3)+

        ifd     G3_TINY
        move.w  #$0041,BLTSIZE(a0)      ; height=1,width=1 -- one word only
        else
        move.w  #$2020,BLTSIZE(a0)      ; height=128,width=32 -- starts the blit
        endif

        moveq   #0,d1
        ifd     G_STOP
        move.w  #0,d2
        else
        move.w  #2047,d2
        endif
.L:     move.w  d1,(a4)+
        addq.w  #1,d1
        dbra    d2,.L

        ifd     G_STOP
        stop    #$2700
        endif

        ifd     G1_BREAK
.hang:  bra.s   .hang
        endif
        DONE
