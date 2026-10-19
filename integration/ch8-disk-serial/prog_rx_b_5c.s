; Precondition 5c contrast, run B: the read of character 7 is delayed until
; after the ninth character driven (see prog_rx_a_5c.s).
MUT_5C          equ 1
C5DELAY         equ 2000
        include "rx_core.s"
