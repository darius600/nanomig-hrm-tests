; prog_p1a_clear_c1.s -- the clear run's own contrasting case (NMH_MUTATE=
; P1-CC1): b_0's IR2 is $0002 instead of $0001 in the clear list, which
; predicts $7FFD instead of $7FFE. See prog_p1a.s.

P1A_CLEAR       equ     1
P1A_CLEAR_C1    equ     1
        include "prog_p1a.s"
