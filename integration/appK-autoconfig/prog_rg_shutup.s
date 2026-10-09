; prog_rg_shutup.s -- R_G' (section 5.2, gate G contrast): writes $00 to
; the shut-up address $00E8004C, then reads IMG as the last action of its
; own run, expected to differ from R_G's IMG0 ("Anything written to 4C
; will cause a board that supports shut-up to completely disappear until
; the next reset").

        include "nanomig.i"
        include "common.i"

IMG      equ     MK_NEXT
PRE      equ     MK_NEXT+200     ; pre-access marker, written immediately before the image read
PRE_RG   equ     $B4B4

        org     PROGRAM_ORG

start:
        ENV_SETUP

        move.b  #$00,AC_BASE+$4C
        lea     AC_BASE,a0
        lea     IMG,a2
        moveq   #15,d3
        move.w  #PRE_RG,PRE
        ifd     MUT_HANG_AFTER
.stop:  bra.s   .stop                   ; no-answer contrast: the stream stops after the marker
        endif
.imgl:  move.w  (a0)+,(a2)+
        move.w  (a0)+,(a2)+
        dbra    d3,.imgl
        DONE

        H_E_HANDLER
