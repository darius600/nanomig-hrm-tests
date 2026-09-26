; prog_s_sc0.s -- Run SC0: BCON=$0020 (PAL only, VARHSYEN=0), s=$0010,
; e=$0040: contrast of P4 for SC.
BCON    equ     $0020
SVAL    equ     $0010
EVAL    equ     $0040
        include "prog_s.s"
