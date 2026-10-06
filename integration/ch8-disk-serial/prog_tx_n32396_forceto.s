; Completion contrast: N=32396, W=1, declaring a poll that ended at its own bound
; (POLL_TIMEOUT set) before MARK=DONE.
NVAL            equ 32396
WCOUNT          equ 1
BND_TSRE        equ 200000
MUT_FORCETO     equ 1
        include "tx_core.s"
