; prog_t_k.s -- Program T, variant T-k (SPECIFICATION.md 4.4): LISTT waits
; for line 255 (WAIT $FF01,$FFFE) after its last mark and then holds 900
; MOVEs in front of the terminal $FFFF,$FFFE. The Copper cannot reach the
; terminal wait before the end of the field (p20: four memory cycle times per
; MOVE) and is restarted in the middle of the list (p25). Contrasting case for
; the third clause of precondition PT-7.
T_LONGLIST      equ     1
                include "prog_t.s"
