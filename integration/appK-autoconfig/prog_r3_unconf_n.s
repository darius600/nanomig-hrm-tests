; prog_r3_unconf_n.s -- green control for P3-NOTOTHER/P3-BASE
; (SPECIFICATION.md section 5.2, P3, "6. Contrasting cases"): the board
; left unconfigured (steps c and d skipped, as R3u), read with R3n's own
; write/readback pair at $00400000.

MUT_R3U equ 1
MUT_R3N equ 1
        include "prog_r3.s"
