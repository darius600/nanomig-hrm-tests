; prog_s_sc_halt.s -- X-HALT-ALL mutation: SC (BCON=$01A0, s=$0010, e=$0040)
; with MARK1's write omitted. Must fail PC-MARK (no marker appears at the port), while
; the rest of the program, including DONE, still runs.
BCON            equ     $01A0
SVAL            equ     $0010
EVAL            equ     $0040
MARK_OMIT       equ     1
        include "prog_s.s"
