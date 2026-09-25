; prog_line_mut.s -- MUTATION support for the drawn-line assertion:
; prog_line.s with MUTATE_UNNORM defined, feeding the raw unswapped deltas
; into the registers instead of the normalized major/minor axis. BLTSIZE's
; height field becomes 8 instead of 36, so the line comes up 28 rows short
; and neither the pixel count nor the last-pixel check can pass.

MUTATE_UNNORM equ 1
        include "prog_line.s"
