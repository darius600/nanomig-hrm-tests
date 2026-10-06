; P8.65 mode 11, CIAA, with a wait of 2300 dbra iterations instead of 2200: W lies in the window P8.65
; requires, but is longer than the other run's. See p865_ciaa_mode11.s.
WAITCNT         equ     2300
                include "p865_ciaa_mode11.s"
