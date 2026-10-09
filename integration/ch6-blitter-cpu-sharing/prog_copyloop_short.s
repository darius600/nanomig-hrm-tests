; prog_copyloop_short.s -- ORDER mutation: prog_copyloop.s with SHORT set, a
; CPU copy loop of 100 words instead of 500.

SHORT   equ     1
        include "prog_copyloop.s"
