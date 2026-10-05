; p4_ciab_tb_l21_nostart.s -- P4-PRE's contrast: the run of p4_ciab_tb_l21 with CR<-$02 (not started). See p4_body.s.
                include "nanomig.i"
                include "ciaregs.i"
PRB             equ     CIAB_PRB
DDRB            equ     CIAB_DDRB
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
LATCHVAL        equ     21
CRVAL           equ     $02
                include "p4_body.s"
