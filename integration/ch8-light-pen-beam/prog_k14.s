; prog_k14.s -- K14: directly after the wait150 that ends phase B, one
; extra write sets VHPOSW = $6400 (line 100, H 0), before the phase-C
; BPLCON0 write.
NMH_K14 equ     1
        include "prog.s"
