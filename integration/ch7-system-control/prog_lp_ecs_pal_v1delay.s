; prog_lp_ecs_pal_v1delay.s -- P3 precondition 2 (read-time check) contrast:
; a ~30-line delay is added before every field's V1 read.
ECS_BUILD       equ     1
V1DELAY         equ     1
        include "prog_lp.s"
