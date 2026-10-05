; prog_tas_span_mut.s -- TAS-SPAN-PRE mutation: prog_tas_span.s with the
; TAS.B replaced by NOP, so the TAS address shows three cycles, not five.

TAS_MUT         equ     1
        include "prog_tas_span.s"
