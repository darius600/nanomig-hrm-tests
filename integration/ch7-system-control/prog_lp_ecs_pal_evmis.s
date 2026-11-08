; prog_lp_ecs_pal_evmis.s -- P3 record-layout precondition contrast: the V2
; record of every field is stored before the V1 record, each with its own
; event id.
ECS_BUILD       equ     1
EVMIS           equ     1
        include "prog_lp.s"
