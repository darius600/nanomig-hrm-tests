; prog_tas_share_mutant.s -- TAS mutation: prog_tas_share.s with MUTANT
; set, replacing the single TAS.B with an ordinary read-then-write pair of
; the same byte.

MUTANT  equ     1
        include "prog_tas_share.s"
