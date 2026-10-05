; p4_ciab_tb_l20_yspread.s -- see p4_body.s. The ICR samples are Chip-memory reads taken after
; one NOP in every second iteration.
MUT_YSPREAD      equ     1
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
LATCHVAL        equ     20
CRVAL           equ     $03
                include "p4_body.s"
