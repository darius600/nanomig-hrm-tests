; prog_h_h1_fixture.s -- FIXTURE mutation: H1 (N=$00B0, B=$08A0) with
; FIXTURE_MUTANT set, storing $DEAD instead of $C3A5 to FIXCONST. The
; unchanged fixture check (peek_logical(FIXCONST) == $C3A5) must go red,
; while the markers, the samples and DONE are the normal run's.
N               equ     $00B0
B               equ     $08A0
FIXTURE_MUTANT  equ     1
        include "prog_h.s"
