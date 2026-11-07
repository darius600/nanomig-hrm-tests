; prog_t_n.s -- Program T, variant T-n (SPECIFICATION.md 4.4): BEAMCON0 = $0080
; (VARBEAMEN) and HTOTAL = $E2 from the CPU, the normal list (no HTOTAL write in
; it); the CPU's sampling loop writes HTOTAL = $E3 or $E2 in every new line it
; sees, alternating from one line to the next across the field boundaries and
; in the last lines of a field, which the Copper cannot reach. Contrasting case
; for P3 and P3(pattern) (SPECIFICATION.md 5, P3).
T_HTOTAL        equ     1
T_HTCPU         equ     1
                include "prog_t.s"
