; prog_h_h2_halt.s -- X-HALT-ALL mutation: H2 (N=$00C0, B=$08A0) with MARK1's
; write omitted. Must fail PC-MARK [H2] (no marker appears at the port), while the rest of the
; program, including DONE, still runs.
N               equ     $00C0
B               equ     $08A0
MARK_OMIT       equ     1
        include "prog_h.s"
