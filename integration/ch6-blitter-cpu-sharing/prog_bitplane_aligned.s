; prog_bitplane_aligned.s -- prog_bitplane.s with ALIGNED set: four
; low-resolution bitplanes with the whole-4-clock-aligned counting loop.
; A6's own in-fetch-window period.

ALIGNED equ     1
        include "prog_bitplane.s"
