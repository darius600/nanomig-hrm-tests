; prog_s_sb0_halt.s -- X-HALT-ALL mutation: SB0 (BCON=$00A0, s=$0020, e=$0030)
; with MARK1's write omitted. Must fail PC-MARK (no marker appears at the port), while
; the rest of the program, including DONE, still runs.
BCON            equ     $00A0
SVAL            equ     $0020
EVAL            equ     $0030
MARK_OMIT       equ     1
        include "prog_s.s"
