; Precondition 1d contrast: N=371, W=4, polling SERDATR bit 12 (TSRE)
; before writing each next word instead of INTREQR bit 0 (TBE) -- so each
; MARK=k now comes only after the stop change, not before it.
NVAL            equ 371
WCOUNT          equ 4
WMULTI          equ 1
BND_TSRE        equ 15000
MUT_BACKTOBACK  equ 1
        include "tx_core.s"
