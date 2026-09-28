; prog_p7_noaud0_mut.s -- G1's own mutation for P7 (NMH_MUTATE=P7-G1),
; layered on the no-AUD0EN baseline: the CPU also reads $0E4000 once (a
; read inside an audio region). See prog_p7.s.

P7_NOAUD0       equ     1
P7_NOAUD0_MUT   equ     1
        include "prog_p7.s"
