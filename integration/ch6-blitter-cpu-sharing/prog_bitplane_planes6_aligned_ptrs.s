; prog_bitplane_planes6_aligned_ptrs.s -- prog_bitplane.s with BP_PLANES6,
; ALIGNED and BP_ALLPTR set: six low-resolution bitplanes with the
; whole-4-clock-aligned counting loop, all six bitplane pointers set to Chip
; memory above the low addresses. A8's live-input mutation.

BP_PLANES6      equ     1
ALIGNED         equ     1
BP_ALLPTR       equ     1
        include "prog_bitplane.s"
