; P1/P2 main contrasting case: N=32396, but SERPER is written with N+1
; (=32397) and the run is graded against N=32396's expectation.
NVAL            equ 32396
WCOUNT          equ 1
BND_TSRE        equ 200000
MUT_NPLUS1      equ 1
        include "tx_core.s"
