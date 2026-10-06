; P8.7 CIAB, run X, Pre8.7's own contrast (MUT_NOLSB): the LSB (TODLO) of the setup's TOD<-0 write is
; never written, so TOD never restarts counting (p344: "The clock will not start again until after a
; write to the LSB event register"). See p87_ciab_body.s.
MUT_NOLSB       equ     1
RUN_X           equ     1
                include "p87_ciab_body.s"
