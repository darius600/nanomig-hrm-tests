; prog_p6_c1.s -- P6's own contrasting case: in field 3 (the fourth field
; of the capture, 0-indexed), once the beam line lies in $64-$C8, the CPU
; executes MOVE.L #L6B,$DFF080 (COP1LC = L6b) once, bracketed by markers,
; with no strobe. See prog_p6.s.

P6_C1           equ     1
        include "prog_p6.s"
