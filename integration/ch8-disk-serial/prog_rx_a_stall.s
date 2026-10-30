; Completion contrast: run A. After the eighth character a bounded poll waits
; for a ninth that the driver never sends, so the poll runs to its own bound
; (POLL_TIMEOUT set by the poll itself) and the program still reaches
; MARK=DONE.
MUT_LONG        equ 1
MUT_STALL       equ 1
        include "rx_core.s"
