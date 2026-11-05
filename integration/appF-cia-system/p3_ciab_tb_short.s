; p3_ciab_tb_short.s -- see p3_body.s. P3-POP's contrast: CIAB/timer B with 599 samples
; instead of 600, so that each of the two streams stores one word fewer.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
M               equ     $80
LATCHVAL        equ     60
CRVAL           equ     $07
NSAMP           equ     599
                include "p3_body.s"
