; prog_tas_share.s -- 6-system-dma.c56 [p196]: measures whether any other
; RAM-port access lands between one TAS.B's own read and its own write
; under active bus contention (a continuously-demanding share-mode blit,
; BLTPRI clear, keeps requesting cycles throughout). The claim is
; withdrawn (TAS-WITHDRAWN in test_blitter_cpu_sharing.cpp): p.196's NOTE
; states a 68000 bus-protocol fact, not a chipset-observable requirement.

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
        tas.b   TAS_ADDR

        DONE
