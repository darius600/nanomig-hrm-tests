; prog_fixture_mut.s -- FIXTURE mutation: prog_fixture.s with
; FIXTURE_MUTANT set, storing $DEAD instead of $BEEF. The unchanged
; fixture check (RESULTS == $BEEF) must go red.

FIXTURE_MUTANT  equ     1
        include "prog_fixture.s"
