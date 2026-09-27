; Precondition 5c contrast: run A, but character 0 is read and cleared only
; after s_2 -- p.256 predicts OVRUN sets, so 5c fails. C5DELAY is a
; poll-iteration count tuned against this build's own measured cadence (not a
; page-defined number; see SPECIFICATION.md) to land past s_2 (~2 char
; steps, ~96000 cycles) without also running past s_3.
MUT_LONG        equ 1
MUT_5C          equ 1
C5DELAY         equ 9000
        include "rx_core.s"
