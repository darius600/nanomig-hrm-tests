; prog_i_v.s -- Program I, variant I-v (SPECIFICATION.md 3.5): both lists,
; relocated, wait for line 255 (WAIT $FF01,$FFFE) and then hold 5,200 MOVEs
; in front of the terminal $FFFF,$FFFE. The Copper cannot reach the terminal
; wait before the end of the field (p20: four memory cycle times per MOVE) and
; is restarted in the middle of the list (p25). Contrasting case for the third
; clause of precondition PI-2.
I_LONGLIST      equ     1
                include "prog_i.s"
