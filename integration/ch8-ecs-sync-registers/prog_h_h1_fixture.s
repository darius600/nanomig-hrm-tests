; prog_h_h1_fixture.s -- FIXTURE mutation: H1 (N=$00B0, B=$08A0) with
; FIXTURE_MUTANT set, storing $DEAD instead of $A5A5 to MARK1. The
; unchanged fixture check (peek_logical(MARK1) == $A5A5) must go red,
; while the rest of the program still reaches DONE.
N               equ     $00B0
B               equ     $08A0
FIXTURE_MUTANT  equ     1
        include "prog_h.s"
