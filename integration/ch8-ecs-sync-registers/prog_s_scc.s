; prog_s_scc.s -- P4C mutation: SC with HSSTRT changed as well (BCON=$01A0,
; s=$0020, e=$0040). HSSTOP moves and HSSTRT moves, so P4(c) (the leading
; edge stays) must fail alone.
BCON            equ     $01A0
SVAL            equ     $0020
EVAL            equ     $0040
        include "prog_s.s"
