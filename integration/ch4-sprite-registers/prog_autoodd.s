; prog_autoodd.s -- run AUTO-ODD (SPECIFICATION.md P8): prog_auto.s with the
; Copper's SPR0PTL MOVE carrying $5001 instead of $5000. See prog_auto.s.
MUT_ODD         equ     1
        include "prog_auto.s"
