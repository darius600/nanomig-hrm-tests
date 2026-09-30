; MUTATION support: P4-c contrast -- BEAMCON0 bit 5 opposite the configured
; standard, P4 base program (ECS, PAL, BPLCON0 never written).
ECS_MODE        equ     1
OMIT_BPLCON0    equ     1
MUT_WRONGSTD    equ     1
        include "prog_lpen_core.s"
