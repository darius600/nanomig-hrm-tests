; MUTATION support: P4-a contrast -- burst reduced to 20 reads, P4 base
; program (ECS, PAL, BPLCON0 never written).
ECS_MODE         equ     1
OMIT_BPLCON0     equ     1
MUT_REDUCE_BURST equ     1
        include "prog_lpen_core.s"
