; prog_p5_mut_p5p2.s -- P5-p2's own mutation (NMH_MUTATE=P5-P2): the park
; at $0C0020 is replaced by a WAIT (line 0) followed by a park at
; $0C0024. See prog_p5.s.

P5_MUT_P5P2     equ     1
        include "prog_p5.s"
