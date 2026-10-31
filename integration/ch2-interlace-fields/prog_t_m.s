; prog_t_m.s -- Program T, variant T-m (SPECIFICATION.md 4.4): BEAMCON0 = $0080
; (VARBEAMEN) and HTOTAL = $E2 from the CPU; the Copper list writes HTOTAL = $E3
; in every even line and $E2 in every odd line, each write early in its line at
; (v,$40); every WAIT (v,$E0) and its mark stays in the list. Contrasting case
; for P3 (SPECIFICATION.md 4.6, P3).
T_HTOTAL        equ     1
T_HTALT         equ     1
                include "prog_t.s"
