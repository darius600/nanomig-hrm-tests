; prog_empty.s -- stimulus mutation for every "markers and records exist"
; line: a program that reaches DONE and writes no marker, no beam record and
; no result. Graded in place of the line's own program by
; NMH_MUTATE=<label>-EMPTY.

        include "nanomig.i"

        org     PROGRAM_ORG

start:
        DONE
