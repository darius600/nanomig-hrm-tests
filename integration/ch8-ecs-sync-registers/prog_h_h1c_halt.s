; prog_h_h1c_halt.s -- X-HALT-ALL mutation: H1c (N=$00B0, B=$0820) with MARK1's
; write omitted. Must fail PC-MARK [H1c] (no marker appears at the port), while the rest of the
; program, including DONE, still runs.
N               equ     $00B0
B               equ     $0820
MARK_OMIT       equ     1
        include "prog_h.s"
