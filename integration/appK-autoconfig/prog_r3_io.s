; prog_r3_io.s -- green control for P3-NOTOTHER/P3-BASE (SPECIFICATION.md
; section 5.2, P3, "6. Contrasting cases"): the board configured normally
; at B1, then read at $00A00000 (Figure K-1, p.386: Zorro II I/O
; expansion space) instead of the base it was not given.

MUT_R3N    equ 1
MUT_IOBASE equ 1
        include "prog_r3.s"
