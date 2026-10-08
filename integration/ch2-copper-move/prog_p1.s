; prog_p1.s -- P1: a Copper MOVE writes its second word into the register its
; first word names; for COLOR00 bits 11-0 appear bit for bit as the displayed
; colour (SPECIFICATION.md P1). The Copper list at $30000 is placed by the test.
;
; Symbols this source answers to (set by the wrapper that includes it):
;   P1_T     address the Copper starts at (default $30000)
;   P1_SPR   the sprite DMA channels are enabled (they fetch from the parked region)
;   P1_ATTR  a CPU read of the list region before the Copper starts

        include "nanomig.i"
        include "ch2move.i"

        ifnd    P1_T
P1_T            equ     $30000
        endc

        org     PROGRAM_ORG

start:
        BASELINE
        VIDEOREGS
        PARKPTRS
        ifd     P1_SPR
        move.w  #$8020,CUSTOM+DMACON
        endc
        ifd     P1_ATTR
        move.w  $30000,d1
        endc
        COPSTART P1_T
        IDLEFIELDS 8
        DONE
