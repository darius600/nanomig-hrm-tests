; prog_s_sb0.s -- Run SB0: BCON=$0020 (PAL only, VARHSYEN=0), s=$0020,
; e=$0030: contrast of P3 for SB.
BCON    equ     $0020
SVAL    equ     $0020
EVAL    equ     $0030
        include "prog_s.s"
