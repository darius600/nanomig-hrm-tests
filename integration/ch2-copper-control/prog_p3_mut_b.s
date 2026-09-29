; prog_p3_mut_b.s -- P3 precondition (b) mutation: DMACON = $83FE (SET/CLR
; set, every channel and DMAEN on, AUD0EN off), graded against P = $3FF
; (Base pattern otherwise). See prog_p3_base.s.
P3_MUT_B        equ     1
        include "prog_p3_base.s"
