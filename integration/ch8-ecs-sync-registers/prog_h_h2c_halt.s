; prog_h_h2c_halt.s -- X-HALT-ALL mutation: H2c (N=$00C0, B=$0820) with MARK1's
; write omitted. Must fail PC-MARK [H2c] (no marker appears at the port), while the rest of the
; program, including DONE, still runs.
N               equ     $00C0
B               equ     $0820
MARK_OMIT       equ     1
        include "prog_h.s"
