; prog_control.s -- prog.s with BROKEN set: the first stored value is off
; by one, so the harness's value check must fail on it.

BROKEN  equ     1
        include "prog.s"
