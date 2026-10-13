; prog_s_sc_nobpl.s -- X-NOBPL mutation for SC (BCON=$01A0, s=$0010, e=$0040) with
; PLANE holding only 32 lines of pattern instead of 256 (SHORT_PLANE).
; Bitplane DMA still runs normally; the fetch continues unmodified past
; line 32 into the harness's zero-filled memory, so those lines carry no
; white. Must fail P3/P4-LINES [SC] with a live, reduced but nonempty
; white-carrying population (not an emptied one).
BCON            equ     $01A0
SVAL            equ     $0010
EVAL            equ     $0040
SHORT_PLANE     equ     1
        include "prog_s.s"
