; prog_r1_nocal.s -- power-LED calibration contrast (section 5.1.6): the
; write that switches the LED to bright keeps CIAAPRA bit 1 = 1 (p.351), so
; both calibration phases show the dim level; RESET at A is replaced by NOP
; so that the run completes.

MUT_NOCAL   equ 1
MUT_NORESET equ 1
        include "prog_r1.s"
