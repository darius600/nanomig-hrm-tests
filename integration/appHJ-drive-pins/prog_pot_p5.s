; prog_pot_p5.s -- register contrast of P5 (SPECIFICATION.md 5.6): for the samples of
; that pin the program stores POTGOR with bit 8 (the pin's own DAT bit) replaced
; by bit 10, the DAT bit of the other pot pin of the same port. See pot_body.s.
WRONGREG        equ     0
WRONGBIT        equ     8
OTHERBIT        equ     10
        include "pot_body.s"
