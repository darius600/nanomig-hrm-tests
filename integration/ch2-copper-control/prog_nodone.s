; prog_nodone.s -- stimulus mutation for every "reaches DONE" line: a
; program that never stores the DONE mark. Graded in place of the line's
; own program by NMH_MUTATE=<label>-NODONE.

        include "nanomig.i"

        org     PROGRAM_ORG

start:
.hang:  bra.s   .hang
