; prog_nodone.s -- the done marker's own contrasting case: a program that
; never writes the done marker, so a run that waits for it ends at its
; budget (the FIXTURE line "reached DONE" prints FAIL through its
; unchanged comparator). It programs no register.

        include "nanomig.i"
        include "ch5regs.i"

        org     PROGRAM_ORG

start:
.hang   bra.s   .hang
