; prog_diskrate_f1mut.s -- prog_diskrate.s with MUT_F1 set: F1's and
; B4's own contrasting case (ECS/PAL only). Before run F, VTOTAL/HTOTAL/
; BEAMCON0 are rewritten so the refresh groups recur about 114 color
; clocks apart instead of 227.5 (2.4).
MUT_F1  equ     1
        include "prog_diskrate.s"
