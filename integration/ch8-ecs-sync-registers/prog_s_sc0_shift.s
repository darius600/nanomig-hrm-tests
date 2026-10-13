; prog_s_sc0_shift.s -- X-SHIFT mutation for SC0 (BCON=$00A0, s=$0010, e=$0040): in
; lines 0 to 127 of PLANE, the white span starts one data word earlier and keeps
; its width, so the landmark's distance to the sync edges is no longer the
; same in every graded line. Must fail P3/P4(a) [SC0] and nothing else.
BCON            equ     $00A0
SVAL            equ     $0010
EVAL            equ     $0040
SHIFT_ROWS      equ     1
        include "prog_s.s"
