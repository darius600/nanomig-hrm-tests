; prog_p6_mut_p6p1.s -- P6-p1's own mutation (NMH_MUTATE=P6-P1): L6a is
; `$6401,$FF00` (WAIT line $64) followed by a park at $0E3004, instead of a
; park at $0E3000. See prog_p6.s.

P6_MUT_P6P1     equ     1
        include "prog_p6.s"
