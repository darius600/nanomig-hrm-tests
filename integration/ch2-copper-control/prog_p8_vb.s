; prog_p8_vb.s -- P8 VB case: the 680x0 can poll the start-of-vertical-
; blank flag (SPECIFICATION.md P8).
;
;   (no symbol)   -- main
;   P8_VB_MUT_B   -- precondition (b) mutation: skip the clear (write
;                    nothing before polling begins)
;   P8_VB_MUT_A   -- precondition (a) mutation: INTENA = $8020 (bit 5 set,
;                    master enable left clear)
;   P8_VB_MUT_BND -- boundary-record mutation: a long delay between the
;                    poll records of phase 2

        include "nanomig.i"
        include "copctl.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL_DDR
        lea     CUSTOM,a0
        CLEAR_DMA_INT
        ifd     P8_VB_MUT_A
        move.w  #$8020,INTENA(a0)        ; precondition (a) mutation: bit 5
                                          ; set, master enable (bit 14) left
                                          ; clear: INTENAR is non-zero
        endif
        move.w  INTENAR(a0),RESULTS+4    ; precondition (a): INTENAR bits 14-0 = 0

        lea     RECBUF,a6
        moveq   #0,d7

        ; Phase 1: wait for a frame wrap (V >= 256 to V <= 1).
        SAMPLE
        addq.l  #1,d7
        move.l  -6(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1
        move.w  d1,d4                    ; d4 = previous V
.wwrap: SAMPLE
        addq.l  #1,d7
        move.l  -6(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1                 ; d1 = current V
        cmp.w   #256,d4
        blt.s   .nowrap
        cmp.w   #1,d1
        bgt.s   .nowrap
        bra.s   .wrapfound
.nowrap:
        move.w  d1,d4
        bra.s   .wwrap
.wrapfound:

        ; Phase 1 continued: then a record shows V >= $F0.
.wf0:   SAMPLE
        addq.l  #1,d7
        move.l  -6(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1
        cmpi.w  #$F0,d1
        blt.s   .wf0

        ifnd    P8_VB_MUT_B
        move.w  #$0020,INTREQ(a0)
        endif
        move.w  #7,MARKBUF+6              ; MC

        ; Phase 2: poll through the next frame wrap and beyond -- a
        ; generous fixed span, like every other proposition's poll phase.
        ifd     P8_VB_MUT_BND
        move.w  #40-1,d3                  ; boundary-record mutation: few, widely spaced records
        else
        move.w  #4000-1,d3
        endif
.pd:    SAMPLE
        addq.l  #1,d7
        ifd     P8_VB_MUT_BND
        move.w  #2500,d6                  ; boundary-record mutation: a long
.bd:    dbra    d6,.bd                    ; delay between the poll records
        endif
        dbra    d3,.pd

        move.l  d7,RESULTS+8

        DONE
