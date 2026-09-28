; P7.0, CIAB, contrasting case (MUT_ST_CONTRAST): CR<-$01 during the first
; delay makes u2<>u1. See p_start_body.s.
MUT_ST_CONTRAST equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAB_CRA
TLO             equ     CIAB_TALO
THI             equ     CIAB_TAHI
ICR             equ     CIAB_ICR
ICRMASK         equ     ICR_TA
                include "p_start_body.s"
