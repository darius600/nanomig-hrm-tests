; prog_bitplane_aligned_p2.s -- prog_bitplane.s with 2 low-resolution
; bitplanes and the whole-4-clock-aligned counting loop (A9).

BP_NPLANES      equ     2
ALIGNED         equ     1
        include "prog_bitplane.s"
