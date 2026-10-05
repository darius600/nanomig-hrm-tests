; P7.7, CIAB, Pre7.7's contrast: the $81 run waits a different number of delay iterations than
; the $01 run, so the two runs' bracket-to-bracket times differ. See p_unused_body.s.
UNDELAY         equ     $04E8
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
FIRSTVAL        equ     $80
SECONDVAL       equ     $81
                include "p_unused_body.s"
