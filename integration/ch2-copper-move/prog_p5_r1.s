; prog_p5_r1.s -- P5, run R1 (SPECIFICATION.md P5): MOVE.L #$00020004 to COP1LCH,
; the strobe, Copper DMA on, five fields; then MOVE.L #$00025A5C to COP1LCH
; without a strobe, five fields. The three end-WAIT lists are placed by the test.
;
; The program waits for line $80 before it writes the location registers, so the
; strobe and the Copper's first fetch fall in the middle of a field.
;
; Symbols this source answers to:
;   P5_SWAP   step 1 writes the two halves of $00020004 as separate words, exchanged
;   P5_EARLY  Copper DMA is enabled before markers 2 and 3
;   P5_IDLE   passages of the beam through line $A0 per phase (default 6)
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
        ifd     P5_SWAP
        move.w  #$0004,CUSTOM+COP1LCH
        move.w  #$0002,CUSTOM+COP1LCL
        else
        move.l  #$00020004,CUSTOM+COP1LCH
        endc
        MARK    1
        move.w  #0,CUSTOM+COPJMP1
        ifd     P5_EARLY
        move.w  #$8280,CUSTOM+DMACON
        MARK    2
        else
        MARK    2
        move.w  #$8280,CUSTOM+DMACON
        endc
        MARK    3
        IDLEFIELDS P5_IDLE
        move.l  #$00025A5C,CUSTOM+COP1LCH
        MARK    4
        IDLEFIELDS P5_IDLE
        MARK    5
        DONE
        ifd     P5_ATTR
        move.w  $20004,d1
        endc
