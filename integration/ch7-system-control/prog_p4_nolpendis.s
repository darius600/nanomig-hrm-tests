; prog_p4_nolpendis.s -- P4's own contrasting case (SPECIFICATION.md,
; "Contrasting case"): F1's own E60 write is $0020 instead of $2020
; (LPENDIS never set). X should then equal Y, both (0,100).
NOLPENDIS       equ     1
        include "prog_p4.s"
