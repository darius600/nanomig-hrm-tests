; MUTATION support: P3-d contrast -- an extra write to $DFF100 (BPLCON0)
; after the first VERTB detection, in the L1 (ECS, PAL) program.
ECS_MODE        equ     1
LPEN1           equ     1
MUT_EXTRA_WRITE equ     1
        include "prog_lpen_core.s"
