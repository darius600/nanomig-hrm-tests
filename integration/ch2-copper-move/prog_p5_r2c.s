; prog_p5_r2c.s -- P5, run R2c (SPECIFICATION.md P5): COP1LC and COP2LC are both
; $00025A5C at the COP2 strobe, Copper DMA on; after the enable marker the program
; writes COP1LC <- $00020004 with no strobe. Five fields.
; The program waits for line $80 before it writes the location registers.
;
; Symbols this source answers to:
;   P5_NOLC1  the COP1LC write after the enable marker is omitted

        include "nanomig.i"
        include "ch2move.i"

        org     PROGRAM_ORG

start:
        BASELINE
        PARKPTRS
        WAITLINE $80
        move.l  #$00025A5C,CUSTOM+COP1LCH
        move.l  #$00025A5C,CUSTOM+COP2LCH
        MARK    1
        move.w  #0,CUSTOM+COPJMP2
        MARK    2
        move.w  #$8280,CUSTOM+DMACON
        MARK    3
        ifnd    P5_NOLC1
        move.l  #$00020004,CUSTOM+COP1LCH
        endc
        IDLEFIELDS 6
        MARK    5
        DONE
