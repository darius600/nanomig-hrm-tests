; prog_lpen_m8_fx.s -- prog_lpen.s with the V8 bit of the vertical counter
; set (VPOSW) for 100 reads of the measurement loop, inside the field that
; carries the F2 pulse and before that pulse, on the fx68k core (the beam is at
; line 72 of that field at the 4800th read).
M_AT    equ 4800
M_LEN   equ 100
        include "prog_lpen.s"
