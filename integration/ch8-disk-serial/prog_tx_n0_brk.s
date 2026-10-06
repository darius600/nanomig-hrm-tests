; Precondition contrast (TxD idle before MARK=0): N=0, W=1, with the UARTBRK force
; inserted between the lead-in and MARK=0 (predicts two TxD changes before the
; window).
NVAL            equ 0
WCOUNT          equ 1
BND_TSRE        equ 200
MUT_BRK         equ 1
        include "tx_core.s"
