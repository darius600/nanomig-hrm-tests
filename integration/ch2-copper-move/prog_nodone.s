; prog_nodone.s -- fixture contrast shared by every proposition: the baseline,
; then the program stops without writing the DONE mark.

        include "nanomig.i"
        include "ch2move.i"

        org     PROGRAM_ORG

start:
        BASELINE
.hang   bra.s   .hang
