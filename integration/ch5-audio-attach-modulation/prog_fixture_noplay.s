; prog_fixture_noplay.s -- prog_fixture.s with NOPLAY set: AUD0EN stays clear,
; so the activity check must fail on it. Not an HRM claim.

NOPLAY  equ     1
        include "prog_fixture.s"
