; MUTATION support: P3-a contrast -- burst reduced from 600 to 20 reads, in
; the L1 (ECS, PAL) program: fewer than 100 reads fall inside the window.
ECS_MODE         equ     1
LPEN1            equ     1
MUT_REDUCE_BURST equ     1
        include "prog_lpen_core.s"
