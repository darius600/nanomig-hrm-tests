; MUTATION support: P1 fixture contrast -- the program stores a start word other than $5A01
; (16-colour high resolution, PAL DIWSTOP).
HIRES_MODE      equ     1
PALETTE16       equ     1
MUT_BAD_START   equ     1
        include "prog_playfield_core.s"
