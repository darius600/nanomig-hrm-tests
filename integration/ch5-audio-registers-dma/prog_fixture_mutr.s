; prog_fixture_mutr.s -- FIXTURE-R's own mutation (stimulus red): the stored
; constant at R is $DEAD instead of $BEEF; the word stored at RESULTS is
; unchanged.

FIXTURE_BREAK_R equ     1
        include "prog_fixture.s"
