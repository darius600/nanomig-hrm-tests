; prog_pot_p6.s -- register contrast of P6 (SPECIFICATION.md 5.6): for the samples of
; that pin the program stores POTGOR with bit 10 (the pin's own DAT bit) replaced
; by bit 8, the DAT bit of the other pot pin of the same port. See pot_body.s.
WRONGREG        equ     1
WRONGBIT        equ     10
OTHERBIT        equ     8
        include "pot_body.s"
