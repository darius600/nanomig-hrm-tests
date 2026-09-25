; prog_line_fixture_mut.s -- MUTATION support for FIXTURE-LINE: prog_line.s
; with MUTATE_FIXTURE defined, storing $DEAD instead of $BEEF as the first
; result word, so the fixture's fixed expectation ($BEEF) must go red.

MUTATE_FIXTURE equ 1
        include "prog_line.s"
