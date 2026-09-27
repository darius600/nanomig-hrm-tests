; Precondition 5e contrast: run A, but every poll iteration adds a delay of
; T/4 or more (SLOWCOUNT tuned against this build's own measured per-iteration
; cadence, not a page-defined number; see SPECIFICATION.md), so the G5
; bracket is no longer narrower than T/4 and 5e fails.
MUT_LONG        equ 1
MUT_SLOWPOLL    equ 1
SLOWCOUNT       equ 120
        include "rx_core.s"
