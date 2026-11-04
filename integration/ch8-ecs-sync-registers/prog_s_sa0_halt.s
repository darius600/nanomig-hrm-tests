; prog_s_sa0_halt.s -- X-HALT-ALL mutation: SA0 (BCON=$00A0, s=$0010, e=$0030)
; with MARK1's write omitted. Must fail PC-MARK (no marker appears at the port), while
; the rest of the program, including DONE, still runs.
BCON            equ     $00A0
SVAL            equ     $0010
EVAL            equ     $0030
MARK_OMIT       equ     1
        include "prog_s.s"
