; prog_mouse_pj2.s -- PJ2's frame-boundary contrast (SPECIFICATION.md 4.5):
; the sampling loop of prog_mouse.s does not sample line 255, so a stored
; sample ends at VHPOSR bits 15-8 = $00 without a frame start. See prog_mouse.s.
PJ2SKIP         equ     1
        include "prog_mouse.s"
