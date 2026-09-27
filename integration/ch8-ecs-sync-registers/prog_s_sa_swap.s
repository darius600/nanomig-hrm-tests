; prog_s_sa_swap.s -- X-A4DROP mutation: SA-shaped run (BCON=$01A0) with
; the HSYNC pulse repositioned past the landmark (s=$00C8, e=$00D8; the
; landmark sits at raw H count ~$B0, SPECIFICATION.md sec.4). Used only to
; feed A4-DROPPED's own comparator a pulse position the page gives no
; layout or direction for (page note 7), to show the dropped-line count
; can move off zero.
BCON    equ     $01A0
SVAL    equ     $00C8
EVAL    equ     $00D8
        include "prog_s.s"
