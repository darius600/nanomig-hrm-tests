; P8.7 CIAB, Pre8.7's own contrast (MUT_NOLSB): the LSB (TODLO) is never
; written, so TOD never restarts counting (p344: "The clock will not start
; again until after a write to the LSB event register"). See p87_ciab.s.
MUT_NOLSB       equ     1
                include "p87_ciab.s"
