; prog_lpen_m8_tg.s -- prog_lpen.s with the V8 bit of the vertical counter
; set (VPOSW) for 100 reads of the measurement loop, inside the field that
; carries the F2 pulse and before that pulse, on the TG68K core (the beam is
; at line 68 of that field at the 7800th read).
M_AT    equ 7800
M_LEN   equ 100
        include "prog_lpen.s"
