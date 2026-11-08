; prog_line_rawx.s -- MUTATION support for the drawn-line assertion (page
; 190's "before normalization, dx is the absolute difference between the
; endpoint x coordinates", 6-line-mode.c39): prog_line.s with MUTATE_RAWX
; defined. abs() is skipped for x only: the raw x2-x1 = -7 is fed into
; page 192's min/max together with abs(y2-y1) = 35, giving dx = 35 and
; dy = -7. BLTAPT = -98, BLTAMOD = -168, BLTBMOD = -28; SIGN, BLTSIZE
; ($0902), BLTCON0 and the octant code are unchanged.

MUTATE_RAWX equ 1
        include "prog_line.s"
