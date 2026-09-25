; prog_bitplane_planes6_aligned.s -- prog_bitplane.s with BP_PLANES6 and
; ALIGNED both set: six low-resolution bitplanes with the
; whole-4-clock-aligned counting loop. A6's own mutation.

BP_PLANES6      equ     1
ALIGNED         equ     1
        include "prog_bitplane.s"
