; prog_mouse_pj3.s -- PJ3's own contrast (SPECIFICATION.md 4.5): the sampling
; loop of prog_mouse.s writes JOYTEST once inside phase 0's baseline window.
; See prog_mouse.s.
PJ3TEST         equ     1
        include "prog_mouse.s"
