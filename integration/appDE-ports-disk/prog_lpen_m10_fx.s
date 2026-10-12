; prog_lpen_m10_fx.s -- prog_lpen.s with the V8 and V9 bits of the vertical
; counter set (VPOSW) for 100 reads of the measurement loop, inside F10 and
; before its line 100, on the fx68k core (the beam is between lines 22 and
; 99 of that field at the 34500th read).
M_AT    equ 34500
M_LEN   equ 100
M_ON    equ $8003
        include "prog_lpen.s"
