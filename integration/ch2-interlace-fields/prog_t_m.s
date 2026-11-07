; prog_t_m.s -- Program T, variant T-m (SPECIFICATION.md 4.4): BEAMCON0 = $0080
; (VARBEAMEN) and HTOTAL = $E2 from the CPU; the Copper list writes HTOTAL = $E3
; in every even line and $E2 in every odd line, each write early in its line at
; (v,$40), for the lines 0 to 254 only; every WAIT (v,$E0) and its mark stays in the
; list, and the marks of the lines 255 to 262 follow as in T-0 (listt_high.i). The list
; writes HTOTAL on the lines 0 ... 254 by construction, so the write of line 254 stays in
; force above it and the list restarts at the same phase in every field.
; Contrasting case for P3 and P3(pattern) (SPECIFICATION.md 5, P3): the lines
; 0 to 254 alternate, the lines above 254 and the field boundaries do not
; (selector P3(restart)).
T_BEAM          equ     1
T_HTALT         equ     1
                include "prog_t.s"
