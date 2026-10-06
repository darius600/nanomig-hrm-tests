; Completion contrast: FAST=1, declaring a poll that ended at its own bound
; (POLL_TIMEOUT set) before MARK=DONE.
MUT_FAST1       equ 1
MUT_FORCETO     equ 1
BND_POLL        equ 200000
        include "fast_core.s"
