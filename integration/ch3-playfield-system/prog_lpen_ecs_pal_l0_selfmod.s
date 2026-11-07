; MUTATION support: P3-c contrast, fetched-word clause -- the L0 (ECS, PAL)
; program rewrites the immediate word of its BEAMCON0 write before it executes,
; so the word the CPU fetches differs from the word of the loaded image.
ECS_MODE        equ     1
MUT_SELFMOD     equ     1
        include "prog_lpen_core.s"
