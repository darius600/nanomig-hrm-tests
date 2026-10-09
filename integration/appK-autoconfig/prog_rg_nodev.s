; prog_rg_nodev.s -- R_G with the image read from words no AUTOCONFIG device
; answers: the program zeroes 32 words of its own results page and reads
; them where R_G reads $00E80000. Gate G's presence line must go red.

        include "nanomig.i"
        include "common.i"

IMG0     equ     MK_NEXT
SRC      equ     MK_NEXT+256

        org     PROGRAM_ORG

start:
        ENV_SETUP

        moveq   #0,d4
        lea     SRC,a0
        moveq   #31,d3
.clr:   move.w  d4,(a0)+
        dbra    d3,.clr

        lea     SRC,a0
        lea     IMG0,a2
        moveq   #15,d3
.imgl:  move.w  (a0)+,(a2)+
        move.w  (a0)+,(a2)+
        dbra    d3,.imgl
        DONE

        H_E_HANDLER
