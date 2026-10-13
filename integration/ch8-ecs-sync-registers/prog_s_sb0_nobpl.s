; prog_s_sb0_nobpl.s -- X-NOBPL mutation for SB0 (BCON=$00A0, s=$0020, e=$0030) with
; PLANE holding only 32 lines of pattern instead of 256 (SHORT_PLANE).
; Bitplane DMA still runs normally; the fetch continues unmodified past
; line 32 into the harness's zero-filled memory, so those lines carry no
; white. Must fail P3/P4-LINES [SB0] with a live, reduced but nonempty
; white-carrying population (not an emptied one).
BCON            equ     $00A0
SVAL            equ     $0020
EVAL            equ     $0030
SHORT_PLANE     equ     1
        include "prog_s.s"
