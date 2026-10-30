; Completion contrast: FAST=0 with a poll bound of 50 iterations, far below the
; length of the transfer, so the DSKBLK poll runs to its own bound
; (POLL_TIMEOUT set by the poll itself) and the program still reaches
; MARK=DONE.
BND_POLL        equ 50
        include "fast_core.s"
