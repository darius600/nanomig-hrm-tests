; Completion contrast: N=0, W=1, declaring a poll that ended at its own bound
; (POLL_TIMEOUT set) before MARK=DONE.
NVAL            equ 0
WCOUNT          equ 1
BND_TSRE        equ 200
MUT_FORCETO     equ 1
        include "tx_core.s"
