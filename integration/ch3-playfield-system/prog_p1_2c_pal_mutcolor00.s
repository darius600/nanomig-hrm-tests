; MUTATION support: P1 main-check contrast for config A -- COLOR00 = $0358
; (C0 with blue bit 0 cleared) instead of changing bit 9.
MUT_COLOR00     equ     1
        include "prog_playfield_core.s"
