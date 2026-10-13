; prog_s_sa_a4double.s -- X-A4DOUBLE mutation: SA (BCON=$01A0, s=$0010,
; e=$0030) with every line's data words changed to 6 black, 6 white, 2 black,
; 6 white. Must fail A4's one-transition clause alone: each graded line now
; carries two transitions into white and two out of white instead of one of
; each, the first white span is as wide as the normal one, every line still
; carries white, and the program still reaches DONE.
BCON            equ     $01A0
SVAL            equ     $0010
EVAL            equ     $0030
A4_DOUBLE       equ     1
        include "prog_s.s"
