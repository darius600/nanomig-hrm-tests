; prog_mouse_sw1.s -- register-order contrast of phase 1 (SPECIFICATION.md 4.6):
; the sampling loop of prog_mouse.s stores JOY1DAT and JOY0DAT in the other order
; for the 512 samples of that phase. See prog_mouse.s.
SWAPPHASE       equ     1
        include "prog_mouse.s"
