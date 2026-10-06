; Precondition contrast (TxD idle before MARK=0): N=32396, W=1, with the UARTBRK force
; inserted between the lead-in and MARK=0 (predicts two TxD changes before the
; window).
NVAL            equ 32396
WCOUNT          equ 1
BND_TSRE        equ 200000
MUT_BRK         equ 1
        include "tx_core.s"
