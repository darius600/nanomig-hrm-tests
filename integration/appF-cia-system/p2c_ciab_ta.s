; p2c_ciab_ta.s -- P2's live control (CIAB, timer A): the CPU stands in for the
; timer's output on the pin (PBON=0 in every CR value). See p2_body.s.
        include "nanomig.i"
        include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
N               equ     6
M               equ     $40
DDRB_INIT       equ     $08
CIAB_PREWRITE   equ     1
CPU_DRIVE       equ     1
S1VAL           equ     $0D
S2VAL           equ     $0D
S3VAL           equ     $0D
S4VAL           equ     $0D
H1VAL           equ     $05
H3VAL           equ     $05
STOPVAL         equ     $04
        include "p2_body.s"
