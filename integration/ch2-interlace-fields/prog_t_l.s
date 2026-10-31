; prog_t_l.s -- Program T, variant T-l (SPECIFICATION.md 4.4): BEAMCON0 = $0080
; (VARBEAMEN) and HTOTAL = $E2 from the CPU; the Copper list writes HTOTAL = $E8
; at line 100 and $E2 at line 101: measured, line 101 is 256 colour clocks long. Every
; mark stays in the list. Contrasting case for precondition PT-5.
T_HTOTAL        equ     1
                include "prog_t.s"
