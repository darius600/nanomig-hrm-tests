; prog_fixture_mut.s -- prog_fixture.s with BROKEN set: the stored
; constant is $DEAD instead of $BEEF, so the harness's fixture check must
; fail on it. Not an HRM claim.

BROKEN  equ     1
        include "prog_fixture.s"
