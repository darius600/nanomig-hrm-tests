; Precondition 5e contrast, run B: every poll iteration adds a delay of T/4 or
; more (see prog_rx_a_5e.s).
MUT_SLOWPOLL    equ 1
SLOWCOUNT       equ 120
        include "rx_core.s"
