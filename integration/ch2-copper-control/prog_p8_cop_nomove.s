; prog_p8_cop_nomove.s -- P8 COP case, device contrast: the MOVE replaced
; by $0180,$0F00 (COLOR00). Bit 4 stays 0. See prog_p8_cop.s.
P8_COP_NOMOVE   equ     1
        include "prog_p8_cop_base.s"
