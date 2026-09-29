; prog_p3_mut_c.s -- P3 precondition (c) mutation: SPR1PT points at AUD0's
; region start; AUD0LC stays at its own Base placement, 256 bytes above
; that same region start (Base pattern otherwise). See prog_p3_base.s.
P3_MUT_C        equ     1
        include "prog_p3_base.s"
