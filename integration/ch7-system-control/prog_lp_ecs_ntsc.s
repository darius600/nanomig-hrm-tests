; prog_lp_ecs_ntsc.s -- P3 under ECS, NTSC. Also reused, run against a
; PAL-configured harness, as P3 precondition 6's own contrast (BEAMCON0
; bit 5 opposite the selected standard): see
; test_ch7_system_control.cpp's own BEAMCON0_MISMATCH case.
ECS_BUILD       equ     1
NTSC_BUILD      equ     1
        include "prog_lp.s"
