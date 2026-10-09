; prog_t_j.s -- Program T, variant T-j (SPECIFICATION.md 4.4): LISTT ends
; in `dc.w $0000,$0000` (a MOVE to register $000, which the Copper cannot
; write, p21 and p26) in front of the $FFFF,$FFFE that stays in the list.
; Contrasting case for precondition PT-7: the restart of every field still
; occurs once, but it does not follow the fetch of the list's terminal wait.
T_BADMOVE       equ     1
                include "prog_t.s"
