; prog_line_mut.s -- MUTATION support for the drawn-line assertion:
; prog_line.s with MUTATE_UNNORM defined, feeding the raw unswapped deltas
; (dx0=7 as major, dy0=35 as minor) into BLTAPT/BLTAMOD/BLTBMOD/BLTSIZE
; instead of the normalized dx=35/dy=7. BLTSIZE's height field becomes 8
; instead of 36, so the line comes up 28 rows short and neither the pixel
; count nor the last-pixel check can pass.

MUTATE_UNNORM equ 1
        include "prog_line.s"
