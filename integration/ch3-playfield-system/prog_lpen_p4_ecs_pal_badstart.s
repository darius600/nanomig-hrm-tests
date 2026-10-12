; MUTATION support: P4 fixture contrast -- the program stores a start word other than $5A03 (ECS, PAL).
ECS_MODE        equ     1
OMIT_BPLCON0    equ     1
MUT_BAD_START   equ     1
        include "prog_lpen_core.s"
