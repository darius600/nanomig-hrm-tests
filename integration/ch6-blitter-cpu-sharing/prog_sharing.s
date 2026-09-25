; prog_sharing.s -- A2/A4/A5/A6/A8 [p169,186,194,196,198]: CPU/blitter DMA
; sharing. This file alone (no symbol set) is the BASELINE build: no
; blitter DMA, just the counting loop. prog_sharing_share.s (SHARE set)
; also runs a large, continuously-demanding A-to-D blit with BLTPRI clear
; (share mode). prog_sharing_nasty.s (NASTY set) runs the same blit with
; BLTPRI set (nasty mode, never yields). All three run forever; the test
; runs each for a fixed cycle budget and reads the RAM-port log, telling
; the CPU's own progress (writes to SHARE_SCRATCH) apart from the
; blitter's (writes to the SHARE_DST range) by address alone.

        include "nanomig.i"

SHARE_SCRATCH   equ     $0C0100
SHARE_SRC       equ     $0C2000
SHARE_DST       equ     $0C4000

        org     PROGRAM_ORG

start:
        ifd     SHARE
        lea     CUSTOM,a0
        move.w  #$8240,DMACON(a0)      ; SET DMAEN,BLTEN (BLTPRI stays clear: share)
        move.w  #$09F0,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$0000,BLTAMOD(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #SHARE_SRC,BLTAPTH(a0)
        move.l  #SHARE_DST,BLTDPTH(a0)
        move.w  #$7D01,BLTSIZE(a0)     ; height=500,width=1 -- starts the blit
        endif

        ifd     NASTY
        lea     CUSTOM,a0
        move.w  #$8640,DMACON(a0)      ; SET DMAEN,BLTEN,BLTPRI (nasty: never yields)
        move.w  #$09F0,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$0000,BLTAMOD(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #SHARE_SRC,BLTAPTH(a0)
        move.l  #SHARE_DST,BLTDPTH(a0)
        move.w  #$7D01,BLTSIZE(a0)     ; height=500,width=1 -- starts the blit
        endif

        moveq   #0,d0
        lea     SHARE_SCRATCH,a1
.loop:  addq.w  #1,d0
        move.w  d0,(a1)
        bra.s   .loop
