; prog_s_sa_nobpl.s -- X-NOBPL mutation: SA (BCON=$01A0, s=$0010, e=$0030)
; with DMACON <- $8280 in step 4 (BPLEN not set). Must fail P3/P4-LINES:
; with no bitplane DMA the display shows no white pixel, so no line
; carries the landmark M, and the graded-line population is empty.
BCON            equ     $01A0
SVAL            equ     $0010
EVAL            equ     $0030
NOBPL           equ     1
        include "prog_s.s"
