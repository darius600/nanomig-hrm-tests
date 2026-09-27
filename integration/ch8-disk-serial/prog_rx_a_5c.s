; Precondition 5c contrast: run A, but character 0 is read and cleared only
; after character 1 has also arrived -- p.256 predicts OVRUN sets, so 5c
; fails. C5DELAY is a poll-iteration count tuned against this build's own
; measured cadence (not a page-defined number; see SPECIFICATION.md) to
; land past character 1's own completion, well inside the run's own cycle
; budget.
MUT_LONG        equ 1
MUT_5C          equ 1
C5DELAY         equ 2000
        include "rx_core.s"
