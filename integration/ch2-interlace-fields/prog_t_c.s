; prog_t_c.s -- Program T, variant T-c (SPECIFICATION.md 4.4): the first
; instruction of LISTT is WAIT (0,$00). Contrasting case for precondition
; PT-4.
T_WAIT0_ZERO    equ     1
                include "prog_t.s"
