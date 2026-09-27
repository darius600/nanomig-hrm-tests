; prog_s_sb0.s -- Run SB0: BCON=$00A0 (VARBEAMEN+PAL, VARHSYEN=0), s=$0020,
; e=$0030: contrast of P3 for SB.
BCON    equ     $00A0
SVAL    equ     $0020
EVAL    equ     $0030
        include "prog_s.s"
