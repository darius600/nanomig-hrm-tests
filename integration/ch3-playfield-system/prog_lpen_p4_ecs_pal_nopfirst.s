; MUTATION support: P4-b start contrast -- one NOP in front of the P4 base program
; (ECS, PAL, BPLCON0 never written). The test sets the initial PC to the word after
; the NOP, so the CPU never fetches the first word of the loaded image.
ECS_MODE        equ     1
OMIT_BPLCON0    equ     1
MUT_NOP_FIRST   equ     1
        include "prog_lpen_core.s"
