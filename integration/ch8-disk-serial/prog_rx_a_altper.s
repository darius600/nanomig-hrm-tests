; MUTATION (b) contrast: run A, but SERPER <- $80B9 (N=185) instead of
; $8173 (N=371). The RxD driver is unchanged (still the same T=372*C bit
; time), so the receiver now samples about twice per driven bit (p.255:
; N+1 color clocks between samples), and reaches nine samples well before
; the driver's own ninth bit begins.
MUT_LONG        equ 1
MUT_ALTPER      equ 1
        include "rx_core.s"
