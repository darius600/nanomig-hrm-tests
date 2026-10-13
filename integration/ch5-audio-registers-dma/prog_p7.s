; prog_p7.s -- P7: with BLTPRI (bit 10) set, the CPU gets no Chip-memory
; cycle while blitter DMA runs (SPECIFICATION.md P7). The blit uses the
; channels A, B and C and not D (USE code E, Table 6-2, p.188: A0 B0 C0 A1
; B1 C1 A2 B2 C2), so no cycle of it is free of a blitter access; the
; window is anchored on the blitter's first and last source read. This file
; alone (no symbol set) is the MAIN build (BLTPRI set, full 2048-iteration
; loop). Variants (each its own source, including this one with a symbol
; set):
;   prog_p7_c1.s    (C1 set)       -- BLTPRI clear (share mode)
;   prog_p7_gearly.s (GEARLY set)  -- the CPU's logging loop runs before the
;                                     BLTSIZE write and the CPU then waits for
;                                     the blit to end, so no CPU write into G
;                                     follows the blit
;   prog_p7_g1.s    (G1_BREAK)     -- branch-to-self before END
;   prog_p7_g2.s    (G2_BREAK)     -- DMAEN (bit 9) left out of the enabling write
;   prog_p7_g3.s    (G3_NOBLIT)    -- no blit starts (BLTSIZE write replaced by a COLOR00 write)

        include "ch5.i"

        org     PROGRAM_ORG

OM      equ     $0090
CM      equ     $0091
ENDM    equ     $00FF

start:
        move.w  #$2700,sr
        CLEAR_OVL
        lea     CUSTOM,a0
        lea     MK,a2
        lea     RSTORE,a3
        lea     GLOG,a4

        move.w  #$07FF,DMACON(a0)
        move.w  #$0EFF,BLTCON0(a0)      ; USEA, USEB, USEC (code E), LF $FF
        move.w  #0,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTCMOD(a0)
        move.w  #0,BLTDMOD(a0)
        move.l  #SRCA,BLTAPTH(a0)
        move.l  #SRCB,BLTBPTH(a0)
        move.l  #SRCC,BLTCPTH(a0)

        ifd     C1
        move.w  #$8240,DMACON(a0)       ; BLTPRI clear (share)
        else
        ifd     G2_BREAK
        move.w  #$8440,DMACON(a0)       ; BLTPRI and BLTEN set, DMAEN (bit 9) left out
        else
        move.w  #$8640,DMACON(a0)       ; SET,BLTPRI,DMAEN,BLTEN
        endif
        endif
        move.w  DMACONR(a0),(a3)+

        ifd     GEARLY
        moveq   #0,d1                   ; precondition mutation: the logging loop runs before the blit
        move.w  #2047,d2
.E:     move.w  d1,(a4)+
        addq.w  #1,d1
        dbra    d2,.E
        endif

        move.w  #OM,(a2)
        ifd     G3_NOBLIT
        move.w  #$2020,COLOR00(a0)      ; no blit starts
        else
        move.w  #$2020,BLTSIZE(a0)      ; height=128,width=32 -- starts the blit
        endif

        ifd     GEARLY
        move.l  #12000,d0               ; wait for the blit to end; nothing is written to G
.W:     subq.l  #1,d0
        bne.s   .W
        endif

        ifnd    GEARLY
        moveq   #0,d1
        move.w  #2047,d2
.L:     move.w  d1,(a4)+
        addq.w  #1,d1
        dbra    d2,.L
        endif

        move.w  #CM,(a2)
        ifd     G1_BREAK
.hang:  bra.s   .hang
        endif
        move.w  #ENDM,(a2)
        DONE
