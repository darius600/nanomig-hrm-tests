; Precondition 5c contrast: run A, with a ninth character driven after the
; eighth, and the read of character 7 delayed until after that ninth character
; has been received -- p.256 predicts OVRUN sets in that RESULT value, so 5c
; fails, while every poll still sees its RBF. C5DELAY is a delay-loop
; iteration count tuned against this build's own measured cadence (not a
; page-defined number; see SPECIFICATION.md) to land past the ninth
; character's own completion, well inside the run's own cycle budget.
MUT_LONG        equ 1
MUT_5C          equ 1
C5DELAY         equ 2000
        include "rx_core.s"
