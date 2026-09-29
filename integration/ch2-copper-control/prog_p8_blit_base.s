; prog_p8_blit_base.s -- P8 BLIT case: the 680x0 can poll the
; blitter-finished flag (SPECIFICATION.md P8). Included by the case files
; below, each setting one symbol first (default = main).
;
;   (no symbol)    -- main
;   P8_BLIT_NOSIZE -- device contrast: omit the BLTSIZE write. Bit 6 stays 0

        include "nanomig.i"
        include "copctl.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL_DDR
        lea     CUSTOM,a0
        CLEAR_DMA_INT
        move.w  INTENAR(a0),RESULTS+4    ; precondition (a): INTENAR bits 14-0 = 0

        ; step 1: DMACON = $8240 (SET/CLR, DMAEN, BLTEN); INTREQ = $0040; MC.
        move.w  #$8240,DMACON(a0)
        move.w  #$0040,INTREQ(a0)
        move.w  #7,MARKBUF+6              ; MC

        ; step 2: D-only blit into DATA1 (64 lines x 32 words).
        move.w  #$01FF,BLTCON0(a0)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$0000,BLTDMOD(a0)
        move.l  #DATA1,BLTDPTH(a0)
        ifnd    P8_BLIT_NOSIZE
        move.w  #$1020,BLTSIZE(a0)        ; last
        endif

        ; step 3: poll for one complete frame.
        lea     RECBUF,a6
        moveq   #0,d7
        move.w  #3000-1,d3
.pd:    SAMPLE
        addq.l  #1,d7
        dbra    d3,.pd

        move.l  d7,RESULTS+8
        move.w  INTREQR(a0),RESULTS+12    ; precondition (c): bit 6 set after the frame

        DONE
