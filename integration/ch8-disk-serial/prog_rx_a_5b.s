; Precondition 5b contrast: run A, but the UARTBRK set step is left out --
; the two TxD levels 5b anchors on are then the same level.
MUT_LONG        equ 1
MUT_NOBRK       equ 1
        include "rx_core.s"
