; prog_bitplane_aligned_p3.s -- prog_bitplane.s with 3 low-resolution
; bitplanes and the whole-4-clock-aligned counting loop (A9).

BP_NPLANES      equ     3
ALIGNED         equ     1
        include "prog_bitplane.s"
