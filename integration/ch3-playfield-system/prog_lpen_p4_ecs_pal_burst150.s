; MUTATION support: P4-a spread contrast -- burst reduced to 150 reads, P4 base
; program (ECS, PAL, BPLCON0 never written): more than 100 reads fall inside the
; window, but they do not reach its end.
ECS_MODE         equ     1
OMIT_BPLCON0     equ     1
MUT_REDUCE_BURST equ     150
        include "prog_lpen_core.s"
