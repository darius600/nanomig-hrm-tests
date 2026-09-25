; prog_line_mut.s -- MUTATION support for LINE-RAW-DELTA and LINE-NORMALIZE:
; prog_line.s with EP1 changed to (0,0)-(6,2), whose raw and normalized
; dx/dy (6,2,6,2) differ from the hardcoded expected 7,35,35,7.

MUTATE_EP1 equ 1
        include "prog_line.s"
