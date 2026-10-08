; prog_p4.s -- P4: a MOVE that follows a WAIT is performed only once the WAIT is
; satisfied (SPECIFICATION.md P4). The Copper list at $38000 is placed by the
; test; this program starts the Copper at P4_T and idles.
;
; Symbols this source answers to (set by the wrapper that includes it):
;   P4_T     address the Copper starts at (default $38000)
;   P4_ATTR  a CPU read of the list region before the Copper starts

        include "nanomig.i"
        include "ch2move.i"

        ifnd    P4_T
P4_T            equ     $38000
        endc

        org     PROGRAM_ORG

start:
        BASELINE
        VIDEOREGS
        PARKPTRS
        ifd     P4_ATTR
        move.w  $38000,d1
        endc
        COPSTART P4_T
        IDLEFIELDS 8
        DONE
