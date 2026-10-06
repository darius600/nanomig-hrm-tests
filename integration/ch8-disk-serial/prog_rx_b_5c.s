; Precondition 5c contrast, run B: character 0 is read and cleared only after
; character 1 has also arrived (see prog_rx_a_5c.s).
MUT_5C          equ 1
C5DELAY         equ 2000
        include "rx_core.s"
