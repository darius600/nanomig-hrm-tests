; prog_s_sa_halt.s -- X-HALT-S mutation: SA (BCON=$01A0, s=$0010, e=$0030)
; with BRA.S * inserted immediately before the HSSTRT write. Must fail
; PC-MARK: MARK1 is never written.
BCON            equ     $01A0
SVAL            equ     $0010
EVAL            equ     $0030
HALT_BEFORE     equ     1
        include "prog_s.s"
