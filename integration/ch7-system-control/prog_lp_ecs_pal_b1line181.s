; prog_lp_ecs_pal_b1line181.s -- P3-PEN-POSITION contrast: the B1/B2 WAIT pair
; sits one line lower (line 181 instead of 180), so the pen trigger that
; follows B1 in F5 lands on line 181; the schedule follows B1 through the
; same code path as in the normal build.
ECS_BUILD       equ     1
B1LINE181       equ     1
        include "prog_lp.s"
