; Precondition 4e contrast, FAST=1 run: after the first transfer the pointer and
; length are written again and the transfer is run a second time over the
; same buffer, so the disk DMA fetches every word of it twice.
MUT_FAST1       equ 1
MUT_ONCE        equ 1
BND_POLL        equ 200000
        include "fast_core.s"
