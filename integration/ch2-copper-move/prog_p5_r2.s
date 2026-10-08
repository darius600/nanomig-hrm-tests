; prog_p5_r2.s -- P5, run R2 (SPECIFICATION.md P5): COP1LC = $00020004 and
; COP2LC = $00025A5C by long writes, the COP2 strobe, Copper DMA on, five fields.
;
; Symbols this source answers to:
;   P5_STROBE1  the strobe is COPJMP1 instead of COPJMP2 (run R2')

        include "nanomig.i"
        include "ch2move.i"

        org     PROGRAM_ORG

start:
        BASELINE
        PARKPTRS
        move.l  #$00020004,CUSTOM+COP1LCH
        move.l  #$00025A5C,CUSTOM+COP2LCH
        MARK    1
        ifd     P5_STROBE1
        move.w  #0,CUSTOM+COPJMP1
        else
        move.w  #0,CUSTOM+COPJMP2
        endc
        MARK    2
        move.w  #$8280,CUSTOM+DMACON
        MARK    3
        IDLEFIELDS 6
        MARK    5
        DONE
