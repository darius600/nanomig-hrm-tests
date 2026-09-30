; P4 base program, ECS, PAL (S1): BEAMCON0 written (LPENDIS into a known
; state), BPLCON0 never written -- LPEN stays at its power-up value.
ECS_MODE        equ     1
OMIT_BPLCON0    equ     1
        include "prog_lpen_core.s"
