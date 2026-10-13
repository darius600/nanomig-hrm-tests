; prog_p5_r2c.s -- P5, run R2c (SPECIFICATION.md P5): COP1LC and COP2LC are both
; $00025A5C at the COP2 strobe, Copper DMA on; after the enable marker the program
; writes COP1LC <- $00020004 with no strobe. Five fields.
; The program waits for line $80 before it writes the location registers.
;
; Symbols this source answers to:
;   P5_NOLC1  the COP1LC write after the enable marker is omitted
;   P5_EARLY  Copper DMA is enabled before markers 2 and 3
;   P5_IDLE   passages of the beam through line $A0 (default 6)
;   P5_ATTR   a CPU read of the list at $20004 stands behind DONE, where it is never executed

        include "nanomig.i"
        include "ch2move.i"

        ifnd    P5_IDLE
P5_IDLE         equ     6
        endc

        org     PROGRAM_ORG

start:
        BASELINE
        PARKPTRS
        WAITLINE $80
        move.l  #$00025A5C,CUSTOM+COP1LCH
        move.l  #$00025A5C,CUSTOM+COP2LCH
        MARK    1
        move.w  #0,CUSTOM+COPJMP2
        ifd     P5_EARLY
        move.w  #$8280,CUSTOM+DMACON
        MARK    2
        else
        MARK    2
        move.w  #$8280,CUSTOM+DMACON
        endc
        MARK    3
        ifnd    P5_NOLC1
        move.l  #$00020004,CUSTOM+COP1LCH
        endc
        IDLEFIELDS P5_IDLE
        MARK    5
        DONE
        ifd     P5_ATTR
        move.w  $20004,d1
        endc
