; prog_s_sa0_shift.s -- X-SHIFT mutation for SA0 (BCON=$00A0, s=$0010, e=$0030): in
; lines 0 to 127 of PLANE, the white span starts one data word earlier and keeps
; its width, so the landmark's distance to the sync edges is no longer the
; same in every graded line. Must fail P3/P4(a) [SA0] and nothing else.
BCON            equ     $00A0
SVAL            equ     $0010
EVAL            equ     $0030
SHIFT_ROWS      equ     1
        include "prog_s.s"
