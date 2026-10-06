; MUTATION support: P3-L contrast -- the VERTB polling loop is replaced by a
; fixed delay; no INTREQR read loop exists in the program.
ECS_MODE        equ     1
MUT_NOPOLL      equ     1
        include "prog_lpen_core.s"
