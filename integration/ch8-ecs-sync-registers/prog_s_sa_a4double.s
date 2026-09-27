; prog_s_sa_a4double.s -- X-A4DOUBLE mutation: SA (BCON=$01A0, s=$0010,
; e=$0030) with every line's data words 14-19 changed to
; $FFFF,$FFFF,$0000,$FFFF,$FFFF,$FFFF. Must fail A4: each graded line now
; carries two transitions into white and two out of white instead of one
; of each, while every line still carries white (the population is not
; emptied) and the program still reaches MARK1.
BCON            equ     $01A0
SVAL            equ     $0010
EVAL            equ     $0030
A4_DOUBLE       equ     1
        include "prog_s.s"
