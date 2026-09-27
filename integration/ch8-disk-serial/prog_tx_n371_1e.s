; Precondition 1e contrast: N=371, W=4, leaving out the SERDAT write for
; word 2 while still polling for its transfer -- the poll for word 3's
; transfer never sees TBE again and ends at its bound.
NVAL            equ 371
WCOUNT          equ 4
WMULTI          equ 1
BND_TSRE        equ 15000
MUT_MISSING2    equ 1
        include "tx_core.s"
