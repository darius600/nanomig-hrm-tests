; prog_t_t.s -- Program T, variant T-t (SPECIFICATION.md 4.4): T-n as a chain of ten states, one of
; which restarts with the phase of the length it ended on, once (listt_chain_slip.i). Stimulus record for
; the single-phase clause of P3(pattern) and the boundary clause of P3 (selector P3(slip)).
T_BEAM          equ     1
T_CHAIN         equ     1
CH_SLIP         equ     1
                include "prog_t.s"
