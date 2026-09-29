; prog_h_h1_halt.s -- X-HALT-H mutation: H1 (N=$00B0, B=$08A0) with MARK1's
; write omitted. Must fail PC-MARK (no marker appears at the port), while
; the rest of the program, including DONE, still runs.
N               equ     $00B0
B               equ     $08A0
MARK_OMIT       equ     1
        include "prog_h.s"
