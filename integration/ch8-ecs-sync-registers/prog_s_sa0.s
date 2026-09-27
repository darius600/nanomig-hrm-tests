; prog_s_sa0.s -- Run SA0: BCON=$00A0 (VARBEAMEN+PAL, VARHSYEN=0), s=$0010,
; e=$0030: contrast of P3/P4 for SA.
BCON    equ     $00A0
SVAL    equ     $0010
EVAL    equ     $0030
        include "prog_s.s"
