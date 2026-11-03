; prog_lpen_m8_tg.s -- prog_lpen.s with the V8 bit of the vertical counter
; set (VPOSW) for 100 reads of the measurement loop, inside the field that
; carries the F2 pulse and before that pulse, on the TG68K core (the reads of
; F2 before its pulse are the 6462nd to the 7356th of the loop).
M_AT    equ 6900
M_LEN   equ 100
        include "prog_lpen.s"
