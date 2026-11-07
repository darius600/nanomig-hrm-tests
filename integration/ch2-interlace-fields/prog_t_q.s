; prog_t_q.s -- Program T, variant T-q (SPECIFICATION.md 4.4): T-n with no list changing COP1LC,
; so the phase restarts at line 0 in every field and the lines still alternate inside every field,
; the lines above 254 and the last lines included. Stimulus record for the boundary clause of P3
; (selector P3(boundary)).
T_BEAM          equ     1
T_CHAIN         equ     1
CH_NOSWITCH     equ     1
                include "prog_t.s"
