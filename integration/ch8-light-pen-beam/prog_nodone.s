; prog_nodone.s -- FIXTURE-DONE mutation: the program hangs after M4
; without storing DONE_VALUE at DONE_ADDR.
NMH_KNODONE     equ     1
        include "prog.s"
