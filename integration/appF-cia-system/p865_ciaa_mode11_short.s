; P8.65 mode 11, CIAA, with a wait of 200 dbra iterations instead of 2200, so W is shorter than the
; window P8.65 requires. See p865_ciaa_mode11.s.
WAITCNT         equ     200
                include "p865_ciaa_mode11.s"
