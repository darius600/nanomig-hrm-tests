; prog_p8_vb_narrow.s -- P8 VB case, device contrast: poll only from V =
; $40 to V = $F0 of one frame. Bit 5 stays 0 (p.219 places the event at
; line 0); this removes the event as well. See prog_p8_vb.s.

        include "nanomig.i"
        include "copctl.i"

        org     PROGRAM_ORG

start:
        CLEAR_OVL_DDR
        lea     CUSTOM,a0
        CLEAR_DMA_INT
        move.w  INTENAR(a0),RESULTS+4

        lea     RECBUF,a6
        moveq   #0,d7

        ; wait for V >= $40
.w1:    SAMPLE
        addq.l  #1,d7
        move.l  -6(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1
        cmpi.w  #$40,d1
        blt.s   .w1

        move.w  #$0020,INTREQ(a0)         ; clear bit 5, as the main VB case does
        move.w  #7,MARKBUF+6              ; MC

        ; poll (SAMPLE) only until V reaches $F0, then stop.
.w2:    SAMPLE
        addq.l  #1,d7
        move.l  -6(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1
        cmpi.w  #$F0,d1
        blt.s   .w2

        move.l  d7,RESULTS+8

        DONE
