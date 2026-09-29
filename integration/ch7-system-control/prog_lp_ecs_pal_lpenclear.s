; prog_lp_ecs_pal_lpenclear.s -- P3 contrasting case "LPEN clear": BPLCON0
; is $0000 with the same pulses (SPECIFICATION.md, P3 "Contrasting cases").
ECS_BUILD       equ     1
LPENCLEAR       equ     1
        include "prog_lp.s"
