; Precondition 5d contrast: run A, but the INTREQ clear in step 5 is left
; out -- the poll for the next character sees RBF=1 at once, and 5d fails.
MUT_LONG        equ 1
MUT_NOCLEAR     equ 1
        include "rx_core.s"
