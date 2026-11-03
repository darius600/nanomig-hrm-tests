; prog_lpen_m4_tg.s -- prog_lpen.s with the V8 and V9 bits of the vertical
; counter set (VPOSW) for the reads of the measurement loop that lie before
; the pulse of F4, on the TG68K core (the reads of F4 from line 22 up to its
; pulse are the 14234th to the 15216th of the loop, and line 101 follows the
; 15225th; the window starts at the 14235th and covers 1000 reads).
M_AT    equ 14235
M_LEN   equ 1000
M_ON    equ $8003
        include "prog_lpen.s"
