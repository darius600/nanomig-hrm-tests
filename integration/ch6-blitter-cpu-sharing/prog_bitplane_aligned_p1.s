; prog_bitplane_aligned_p1.s -- prog_bitplane.s with 1 low-resolution
; bitplane and the whole-4-clock-aligned counting loop (A9).

BP_NPLANES      equ     1
ALIGNED         equ     1
        include "prog_bitplane.s"
