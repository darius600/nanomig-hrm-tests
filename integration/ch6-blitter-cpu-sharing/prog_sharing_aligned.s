; prog_sharing_aligned.s -- prog_sharing.s with ALIGNED set: the BASELINE
; (no blitter DMA) build with the whole-4-clock-aligned counting loop.
; SYSDMA-C29's mutation and A6's own no-DMA reference period.

ALIGNED equ     1
        include "prog_sharing.s"
