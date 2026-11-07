; prog_t_n.s -- Program T, variant T-n (SPECIFICATION.md 4.4): BEAMCON0 = $0080
; (VARBEAMEN) and HTOTAL = $E2 from the CPU, then two Copper lists (listt_chain.i) that
; alone write HTOTAL = $E3 or $E2 in every line 0 to 262, alternating from one line to
; the next, mark every line as the normal list does, and carry the phase from one field
; into the next through COP1LC: the lines alternate across the field boundaries and in
; the last lines of every field, whether the field has 262 or 263 lines. The CPU writes
; nothing after the setup. Contrasting case for P3 and P3(pattern)
; (SPECIFICATION.md 5, P3).
T_BEAM          equ     1
T_CHAIN         equ     1
                include "prog_t.s"
