; MUTATION support: P3-M contrast -- four blocks in the measurement buffer
; instead of five, in the L1 (ECS, PAL) program.
ECS_MODE        equ     1
LPEN1           equ     1
MUT_FOURBLOCKS  equ     1
        include "prog_lpen_core.s"
