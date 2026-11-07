; prog_t_j.s -- Program T, variant T-j (SPECIFICATION.md 4.4): LISTT holds
; `dc.w $0000,$0000` (a MOVE to register $000, which the Copper cannot
; write, p21 and p26) behind the mark of line 254, in front of the marks of the
; lines 255 and above and of the terminal $FFFF,$FFFE.
; A diagnostic of the third clause of PT-7's comparator, not a contrast of
; PT-7: the restart of every field still occurs once, but it does not follow
; the fetch of the words the Copper reads when it has run through the list as far as
; the field's lines let it.
T_BADMOVE       equ     1
                include "prog_t.s"
