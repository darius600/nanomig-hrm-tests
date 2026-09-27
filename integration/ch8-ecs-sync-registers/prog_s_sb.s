; prog_s_sb.s -- Run SB: BCON=$01A0 (VARHSYEN+VARBEAMEN+PAL), s=$0020, e=$0030.
BCON    equ     $01A0
SVAL    equ     $0020
EVAL    equ     $0030
        include "prog_s.s"
