; Uniform-grid contrast: N=0, W=1. SERPER is rewritten with 3 right after the
; SERDAT write, while the word is being sent, so the bit intervals after the
; write are 4 colour clocks long instead of 1.
NVAL            equ 0
WCOUNT          equ 1
BND_TSRE        equ 200
MUT_MIDSERPER   equ 1
MIDVAL          equ 3
        include "tx_core.s"
