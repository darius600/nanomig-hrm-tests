; prog_t2s_fixwritemut.s -- prog_t2s.s with MUT_FIXWRITE set: the FIXTURE-WRITE
; line's own contrasting case (the ADKCONR store into the A2 slot is left out).
MUT_FIXWRITE    equ     1
        include "prog_t2s.s"
