; prog_i_s.s -- Program I, variant I-s (SPECIFICATION.md 3.5): both lists end
; in `dc.w $0000,$0000` (a MOVE to register $000, which the Copper cannot
; write, p21 and p26) in front of the $FFFF,$FFFE that stays in the list.
; Contrasting case for precondition PI-2: the restart of every field still
; occurs once, but it does not follow the fetch of the list's terminal wait.
I_BADMOVE       equ     1
                include "prog_i.s"
