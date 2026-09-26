; p2_mutc.s -- p2.s with MUT_C set: P2.c's contrast (SPECIFICATION.md
; section 4): a trap-0 handler (RTE only) is installed and TRAP #0 runs
; between steps 6 and 7, adding stacking writes inside the window.
MUT_C           equ     1
        include "p2.s"
