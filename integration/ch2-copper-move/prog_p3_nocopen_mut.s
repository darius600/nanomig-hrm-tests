; prog_p3_nocopen_mut.s -- G1's own mutation (NMH_MUTATE=P3-G1): the
; baseline (no COPEN) plus one CPU read inside the Copper region. See
; prog_p3.s.

P3_NOCOPEN      equ     1
P3_NOCOPEN_MUT  equ     1
        include "prog_p3.s"
