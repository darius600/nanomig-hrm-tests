; prog_pot_pp2.s -- PP2's own contrast (SPECIFICATION.md 5.5): pin 5's (POT0X)
; C1 write sets POTGO bit 0 = 1 instead of 0. See pot_body.s.
PP2MUT          equ     1
        include "pot_body.s"
