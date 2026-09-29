; prog_p4_f3lpendis.s -- P4 precondition 3's own contrast: F3's own E60
; write is also $2020 (LPENDIS set in the control field too), so X' should
; no longer equal Y'.
F3LPENDIS       equ     1
        include "prog_p4.s"
