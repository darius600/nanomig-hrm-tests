; P7.5-CIAA-INMODE and P7.5-CIAB-GREENCTL, CIAA (CRA/timer A, CNT): an interval with no CNT edge, then an
; interval with one key code's eight edges. See p75ctl_body.s.
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRA
TLO             equ     CIAA_TALO
THI             equ     CIAA_TAHI
ICR             equ     CIAA_ICR
                include "p75ctl_body.s"
