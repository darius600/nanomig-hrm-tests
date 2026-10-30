; Completion contrast: N=32396, W=1. After the transmission a bounded poll
; waits for a received character (RBF) that never comes, so the poll runs to
; its own bound (POLL_TIMEOUT set by the poll itself) and the program still
; reaches MARK=DONE.
NVAL            equ 32396
WCOUNT          equ 1
BND_TSRE        equ 200000
MUT_STALL       equ 1
        include "tx_core.s"
