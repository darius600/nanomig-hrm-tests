; MUTATION support: P3-c contrast -- BEAMCON0 with LPENDIS=1 (bit 13) in the
; L1 (ECS, PAL) program: the light pen latch's output is ignored on a read.
ECS_MODE        equ     1
LPEN1           equ     1
MUT_LPENDIS     equ     1
        include "prog_lpen_core.s"
