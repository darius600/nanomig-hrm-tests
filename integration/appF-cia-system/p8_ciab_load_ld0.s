; P8.4, CIAB, contrast (MUT_LD0): CR<-$00 instead of $10 ("writing a 0 has
; no effect"). See p_load_body.s.
MUT_LD0         equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRB
TLO             equ     CIAB_TBLO
THI             equ     CIAB_TBHI
ICR             equ     CIAB_ICR
                include "p_load_body.s"
