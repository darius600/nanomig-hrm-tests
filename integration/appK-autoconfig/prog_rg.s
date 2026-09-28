; prog_rg.s -- run R_G: SPECIFICATION.md section 5.2, gate G. Reads IMG0 as
; the last action of its own run (its first read may be unanswered,
; section 2.2), stores it, and writes the completion marker.

        include "nanomig.i"
        include "common.i"

IMG0     equ     MK_NEXT

        org     PROGRAM_ORG

start:
        ENV_SETUP

        lea     AC_BASE,a0
        lea     IMG0,a2
        moveq   #15,d3
.imgl:  move.w  (a0)+,(a2)+
        move.w  (a0)+,(a2)+
        dbra    d3,.imgl
        DONE

        H_E_HANDLER
