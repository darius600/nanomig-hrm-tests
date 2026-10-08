; prog_pot_p8.s -- register contrast of P8 (SPECIFICATION.md 5.6): for the samples of
; that pin the program stores POTGOR with bit 14 (the pin's own DAT bit) replaced
; by bit 12, the DAT bit of the other pot pin of the same port. See pot_body.s.
WRONGREG        equ     3
WRONGBIT        equ     14
OTHERBIT        equ     12
        include "pot_body.s"
