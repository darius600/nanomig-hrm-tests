; prog_fixture_mut.s -- FIXTURE's own mutation (stimulus red): the stored
; constant at RESULTS is $DEAD instead of $BEEF; the word stored at R is
; unchanged.

FIXTURE_BREAK   equ     1
        include "prog_fixture.s"
