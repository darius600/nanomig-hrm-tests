; prog_s_sa_a4width.s -- X-A4WIDTH mutation: SA (BCON=$01A0, s=$0010,
; e=$0030) with every line's data words changed to 14 black, 5 white,
; 1 black. Must fail A4's width clause alone: each line still carries one
; transition into white and one out of white, and the white span is one data
; word narrower than in the other five runs.
BCON            equ     $01A0
SVAL            equ     $0010
EVAL            equ     $0030
A4_WIDTH        equ     1
        include "prog_s.s"
