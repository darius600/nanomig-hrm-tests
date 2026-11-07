; MUTATION support: P3-a spread contrast -- burst reduced from 600 to 150 reads, in
; the L1 (ECS, PAL) program: more than 100 reads fall inside the window, but they
; do not reach its end.
ECS_MODE         equ     1
LPEN1            equ     1
MUT_REDUCE_BURST equ     150
        include "prog_lpen_core.s"
