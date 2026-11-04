; Uniform-grid contrast: N=32396, W=1. SERPER is rewritten with 16198 right
; after the SERDAT write, while the word is being sent, so the bit intervals
; after the write are half as long as the one before it.
NVAL            equ 32396
WCOUNT          equ 1
BND_TSRE        equ 200000
MUT_MIDSERPER   equ 1
MIDVAL          equ 16198
        include "tx_core.s"
