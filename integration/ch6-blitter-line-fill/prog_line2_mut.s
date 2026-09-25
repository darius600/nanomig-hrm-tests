; prog_line2_mut.s -- MUTATION support for the second drawn-line assertion
; (leading id 6-line-mode.c40): prog_line2.s with MUTATE_NOABS defined,
; feeding the raw signed y2-y1=-35 (not abs'd) into the min/max
; normalization. BLTSIZE's height field becomes 8 instead of 36, so the
; line comes up short exactly as prog_line_mut.s's MUTATE_UNNORM does, by a
; different broken derivation (skipped abs(), not a skipped major/minor
; swap).

MUTATE_NOABS equ 1
        include "prog_line2.s"
