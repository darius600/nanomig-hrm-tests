; Precondition 1c contrast: N=371, W=4, with the UARTBRK force inserted
; between the lead-in and MARK=0 (predicts two TxD changes before the
; window, so 1c must fail).
NVAL            equ 371
WCOUNT          equ 4
WMULTI          equ 1
BND_TSRE        equ 15000
MUT_BRK         equ 1
        include "tx_core.s"
