; MUTATION support: P3-c contrast, write-count clause -- the L1 (ECS, PAL)
; program writes BEAMCON0 a second time, with LPENDIS=1.
ECS_MODE        equ     1
LPEN1           equ     1
MUT_SECONDBEAM  equ     1
        include "prog_lpen_core.s"
