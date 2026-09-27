; Precondition 4d contrast: FAST=1, but the second DSKLEN write (p.247
; step 4, "This actually starts the DMA") is left out -- DMAON never sets,
; so there are no fetches at all.
MUT_FAST1       equ 1
MUT_NODSKLEN2   equ 1
BND_POLL        equ 200000
        include "fast_core.s"
