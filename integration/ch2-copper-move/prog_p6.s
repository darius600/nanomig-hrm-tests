; prog_p6.s -- P6: a 680x0 long-word write puts the most significant word at the
; lower word address (SPECIFICATION.md P6).
;
; Symbols this source answers to:
;   P6_VALUE  the long word written (default $12345678)
;   P6_NOPRE  the two word writes before marker 1 are omitted

        include "nanomig.i"
        include "ch2move.i"

        ifnd    P6_VALUE
P6_VALUE        equ     $12345678
        endc

        org     PROGRAM_ORG

start:
        BASELINE
        ifnd    P6_NOPRE
        move.w  #$AAAA,$46000
        move.w  #$5555,$46002
        endc
        MARK    1
        move.l  #P6_VALUE,$46000
        MARK    2
        move.w  $46000,RESULTS
        move.w  $46002,RESULTS+2
        DONE
