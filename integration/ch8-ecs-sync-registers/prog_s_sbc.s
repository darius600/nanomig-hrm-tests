; prog_s_sbc.s -- P3C mutation: SB with HSSTOP changed as well (BCON=$01A0,
; s=$0020, e=$0040). HSSTRT moves and HSSTOP moves, so P3(c) (the trailing
; edge stays) must fail alone.
BCON            equ     $01A0
SVAL            equ     $0020
EVAL            equ     $0040
        include "prog_s.s"
