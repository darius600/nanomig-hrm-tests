; MUTATION support: P3-LIVE contrast on the repository's chipset -- the last
; burst of the L0 (ECS, PAL) program does 100 plain reads, then 500 reads each
; preceded by a write to VHPOSW.
ECS_MODE        equ     1
MUT_VHPOSW      equ     1
        include "prog_lpen_core.s"
