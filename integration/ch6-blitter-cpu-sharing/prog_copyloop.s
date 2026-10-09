; prog_copyloop.s -- ORDER [p169]: a plain 68000 MOVE.W copy loop moving
; CL_N words from CL_SRC to CL_DST, no DMA competing for the bus. The test
; measures its first-to-last destination-write span the same way it
; measures the nasty-mode blit's own span (prog_sharing_nasty.bin,
; established by A4), so the two can be compared on the same yardstick for
; the same word count without assuming an exact speed factor.

        include "nanomig.i"

CL_SRC  equ     $0C8000
CL_DST  equ     $0C8800
        ifd     SHORT
CL_N    equ     100                     ; ORDER's mutation: fewer words than the 500 the test expects
        else
CL_N    equ     500
        endif

        org     PROGRAM_ORG

start:
        lea     CL_SRC,a0
        lea     CL_DST,a1
        move.w  #CL_N-1,d0
.loop:  move.w  (a0)+,(a1)+
        dbra    d0,.loop

        DONE
