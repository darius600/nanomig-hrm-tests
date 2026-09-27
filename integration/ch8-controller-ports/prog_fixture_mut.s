; prog_fixture_mut.s -- prog_fixture.s with BROKEN set: stores a different
; constant, so the fixture's own value check must fail on it.

BROKEN  equ     1
        include "prog_fixture.s"
