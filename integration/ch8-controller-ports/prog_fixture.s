; prog_fixture.s -- proves the fixture can deliver a known value before any
; assertion reads one (SPECIFICATION.md, "the fixture proves it can deliver
; a known value"). Not an HRM claim.

        include "nanomig.i"
        org     PROGRAM_ORG

FIX_BASE        equ     $0C0100

start:
        lea     FIX_BASE,a1
        ifd     BROKEN
        move.w  #$FACE,(a1)+
        else
        move.w  #$FEED,(a1)+
        endif
        DONE
