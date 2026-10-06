; P8.65-MODE01-CIAB-GREENCTL, CIAA (CRB/timer B, INMODE=01): an interval with no CNT edge, then an
; interval with one key code's eight edges. See p75ctl_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRB
TLO             equ     CIAA_TBLO
THI             equ     CIAA_TBHI
ICR             equ     CIAA_ICR
                include "p75ctl_body.s"
