; prog_t_p.s -- Program T, variant T-p (SPECIFICATION.md 4.4): DIWSTRT = $2D81, the display
; window starts one line late (line 45). Contrasting case for the start clause of precondition PT-2.
T_DIWSTRT       equ     $2D81
                include "prog_t.s"
