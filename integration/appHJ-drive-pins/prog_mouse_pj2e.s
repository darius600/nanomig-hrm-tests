; prog_mouse_pj2e.s -- PJ2's frame-end contrast (SPECIFICATION.md 4.5): the sampling loop
; of prog_mouse.s learns the frame's last line from one frame wrap before it
; starts sampling and does not sample that line, so a stored line is missing at
; the frame's end. See prog_mouse.s.
PJ2SKIPEND      equ     1
        include "prog_mouse.s"
