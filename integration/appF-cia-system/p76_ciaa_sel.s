; P7.6, CIAA, CRA bit 6 = 1 with timer A stopped (CR <- $40): the same run as p7_ciaa_inmode.s, the same
; key code at the same time, with SPMODE = output. R-KEY is sent by the C++ side. See p75_body.s.
DO_KEY          equ     1
START_BRACKET   equ     1
                include "nanomig.i"
                include "ciaregs.i"
CR              equ     CIAA_CRA
TLO             equ     CIAA_TALO
THI             equ     CIAA_TAHI
ICR             equ     CIAA_ICR
STARTVAL        equ     $40
                include "p75_body.s"
