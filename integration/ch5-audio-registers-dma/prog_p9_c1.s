; prog_p9_c1.s -- P9's contrasting case (stimulus red): exchange BLTCON0
; between blits 1/2, and the source between blits 4/5.

BC0_1   equ     $01FF
BC0_2   equ     $0100
ASRC4   equ     ZB
ASRC5   equ     FW
        include "prog_p9.s"
