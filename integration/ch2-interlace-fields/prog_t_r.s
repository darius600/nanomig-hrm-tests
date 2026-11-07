; prog_t_r.s -- Program T, variant T-r (SPECIFICATION.md 4.4): T-n with line 262 writing
; HTOTAL = $E8, so that the last line of a 263-line field is 233 colour clocks long. Stimulus record
; for the range clause of P3 on the last lines (selector P3(range)).
T_BEAM          equ     1
T_CHAIN         equ     1
CH_LAST_E8      equ     1
                include "prog_t.s"
