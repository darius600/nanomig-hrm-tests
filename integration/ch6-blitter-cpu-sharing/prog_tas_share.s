; prog_tas_share.s -- TAS [p196] 6-system-dma.c56: TAS's read-modify-write
; cycle is indivisible even under active bus contention. Runs a
; continuously-demanding share-mode blit (BLTPRI clear, so the blitter
; keeps requesting cycles throughout) concurrently with one TAS.B, then
; DONE. prog_tas_share_mutant.s (MUTANT set, TAS's mutation) replaces the
; single TAS.B with an ordinary, separately-arbitrated read-then-write
; pair of the same byte, which a genuinely contended bus can interrupt.

        include "nanomig.i"

TAS_ADDR        equ     $0C2000
TAS_SRC         equ     $0C4000
TAS_DST         equ     $0C6000

        org     PROGRAM_ORG

start:
        lea     CUSTOM,a0
        move.w  #$8240,DMACON(a0)      ; SET DMAEN,BLTEN (BLTPRI stays clear: share)
        move.w  #$09F0,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$0000,BLTAMOD(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #TAS_SRC,BLTAPTH(a0)
        move.l  #TAS_DST,BLTDPTH(a0)
        move.w  #$7D01,BLTSIZE(a0)     ; height=500,width=1 -- long-running, share-mode blit

        move.b  #$00,TAS_ADDR
        ifd     MUTANT
        move.b  TAS_ADDR,d1             ; ordinary, separately-arbitrated read...
        move.b  d1,TAS_ADDR             ; ...then write -- not one indivisible cycle
        else
        tas.b   TAS_ADDR
        endif

        DONE
