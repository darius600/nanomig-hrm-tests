; P8.65 mode 11, CIAA, with timer A's latch 4 instead of 3: the window is as long as the other run's, but timer A
; offers fewer underflows to timer B. See p865_ciaa_mode11.s.
LATCHA          equ     4
                include "p865_ciaa_mode11.s"
