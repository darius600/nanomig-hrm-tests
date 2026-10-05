; p2c_ciab_tb_k4.s -- P2's live control (CIAB, timer B): the CPU stands in for the
; timer's output on the pin (PBON=0 in every CR value). See p2_body.s.
        include "nanomig.i"
        include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
N               equ     7
M               equ     $80
DDRB_INIT       equ     $08
CIAB_PREWRITE   equ     1
CPU_DRIVE       equ     1
S1VAL           equ     $0D
S2VAL           equ     $0D
S3VAL           equ     $0D
S4VAL           equ     $0C
H1VAL           equ     $05
H3VAL           equ     $05
STOPVAL         equ     $04
NORAISE4        equ     1
        include "p2_body.s"
