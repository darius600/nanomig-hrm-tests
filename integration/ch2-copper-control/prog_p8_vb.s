; prog_p8_vb.s -- P8 VB case: the 680x0 can poll the start-of-vertical-
; blank flag (SPECIFICATION.md P8).
;
;   (no symbol)     -- main
;   P8_VB_MUT_B     -- precondition (b) mutation: skip the clear (write
;                      nothing before polling begins)
;   P8_VB_MUT_A     -- precondition (a) mutation: INTENA = $8020 (bit 5 set,
;                      master enable left clear)
;   P8_VB_MUT_BND_A -- boundary mutation: VERTB is set from the program
;                      (INTREQ = $8020) once a record shows V >= $130, a few
;                      lines before the frame wrap
;   P8_VB_MUT_BND_B -- boundary mutation: INTREQ = $0020 is written before
;                      every poll record, so the flag is cleared again before
;                      each read
;   P8_VB_MUT_III   -- check (iii) contrast: once a poll record reads bit 5
;                      = 1, INTREQ = $0020 is written again (once)
;   P8_VB_MUT_D     -- population mutation: the clear and MC come at V >=
;                      $130, and no record is stored until line 0 or 1 has
;                      been reached, so no record lies between MC and the
;                      frame wrap

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
        ifd     P8_VB_MUT_D
        cmpi.w  #$130,d1
        else
        cmpi.w  #$F0,d1
        endif
        blt.s   .wf0

        ifnd    P8_VB_MUT_B
        move.w  #$0020,INTREQ(a0)
        endif
        move.w  #7,MARKBUF+6              ; MC

        ifd     P8_VB_MUT_D
        ; population mutation: no record until line 0 or 1, so the wrap
        ; passes between MC and the first record stored after it.
.wl:    move.l  VPOSR(a0),d0
        lsr.l   #8,d0
        and.l   #$1FF,d0
        cmpi.w  #2,d0
        bgt.s   .wl
        endif

        ifd     P8_VB_MUT_BND_A
        ; boundary mutation: poll until a record shows V >= $130, then set
        ; VERTB from the program.
.wa:    SAMPLE
        addq.l  #1,d7
        move.l  -6(a6),d0
        move.l  d0,d1
        lsr.l   #8,d1
        and.l   #$1FF,d1
        cmpi.w  #$130,d1
        blt.s   .wa
        move.w  #$8020,INTREQ(a0)
        endif

        ; Phase 2: poll through the next frame wrap and beyond -- a
        ; generous fixed span, like every other proposition's poll phase.
        ifd     P8_VB_MUT_III
        moveq   #0,d6                    ; 0 until the re-clear below has fired
        move.w  #4000-1,d3
.pd:    SAMPLE
        addq.l  #1,d7
        move.w  -8(a6),d0                ; the record's own INTREQR word
        btst    #5,d0
        beq.s   .pd_next
        tst.w   d6
        bne.s   .pd_next
        move.w  #$0020,INTREQ(a0)        ; check (iii) contrast: clear again
        moveq   #1,d6
.pd_next:
        dbra    d3,.pd
        else
        move.w  #4000-1,d3
.pd:
        ifd     P8_VB_MUT_BND_B
        move.w  #$0020,INTREQ(a0)        ; boundary mutation: cleared before every read
        endif
        SAMPLE
        addq.l  #1,d7
        dbra    d3,.pd
        endif

        move.l  d7,RESULTS+8

        DONE
