; prog_rg.s -- run R_G: SPECIFICATION.md section 5.2, gate G. Reads IMG0 as
; the first AUTOCONFIG access of its own run, stores it, writes $00 to
; register 4A and $20 to register 48 (the configuring writes of P3, p.436),
; writes the pre-access marker (the second read is of configuration space the
; board has left, which no page says completes), reads the image again into
; IMG_B (G4) and writes the completion marker.
;
;   MUT_PRE48     -- GATE-G-PRESENCE's mutation: the two writes also come before
;                    the first read of IMG0 (the reads are unchanged)
;   MUT_HANG_AFTER -- apparatus contrast of the no-answer branch: the stream
;                    stops right after the pre-access marker

        include "nanomig.i"
        include "common.i"

IMG0     equ     MK_NEXT
IMG_B    equ     MK_NEXT+64
PRE      equ     MK_NEXT+200     ; pre-access marker, written immediately before the second image read
PRE_RGIMG equ    $B5B5

        org     PROGRAM_ORG

start:
        ENV_SETUP

        ifd     MUT_PRE48
        move.b  #$00,AC_BASE+$4A
        move.b  #$20,AC_BASE+$48
        endif

        lea     AC_BASE,a0
        lea     IMG0,a2
        moveq   #15,d3
.imgl:  move.w  (a0)+,(a2)+
        move.w  (a0)+,(a2)+
        dbra    d3,.imgl

        move.b  #$00,AC_BASE+$4A
        move.b  #$20,AC_BASE+$48

        lea     AC_BASE,a0
        lea     IMG_B,a2
        moveq   #15,d3
        move.w  #PRE_RGIMG,PRE
        ifd     MUT_HANG_AFTER
.stop:  bra.s   .stop                   ; no-answer contrast: the stream stops after the marker
        endif
.imgb:  move.w  (a0)+,(a2)+
        move.w  (a0)+,(a2)+
        dbra    d3,.imgb
        DONE

        H_E_HANDLER
