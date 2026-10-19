; R1 contrast: N=32396, W=1, with the ROM overlay cleared and one CPU read
; of Chip word $000100 after the lead-in.
NVAL            equ 32396
WCOUNT          equ 1
BND_TSRE        equ 200000
MUT_R1READ      equ 1
        include "tx_core.s"
