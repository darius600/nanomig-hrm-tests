; p6_k123.s -- P6: three loops of 200 iterations, one store each. K1 reads
; CIAA PRB, K2 reads CIAB PRB, K3 (contrast) reads a Chip-memory byte
; instead, padded so its own iteration time is not a multiple of 10*T_CPU.
; SPECIFICATION.md P6.
;
; Optional symbols (one wrapper each; the normal program defines none):
;   MUT_SHORT  K1 runs 199 iterations instead of 200
;   MUT_K1CHIP K1 reads K3's Chip-memory byte instead of CIAA PRB
;   MUT_K3MIX  K3 alternates, store by store, a read of CIAA PRB and a read of the Chip-memory byte
;   K3NOPS     number of NOPs in K3's loop body (default 2)
                include "nanomig.i"
                include "ciaregs.i"

P6_K1           equ     RESULTS+8       ; 200 words
P6_K2           equ     P6_K1+200*2     ; 200 words
P6_K3           equ     P6_K2+200*2     ; 200 words

; K3's padding is NOPs (4 CPU-clock periods each), so that the loop body's
; iteration time is not a multiple of 10*T_CPU (Pre6.1, which the run checks
; against the measured I3). A whole extra dbra loop would only ever shift I3
; by a multiple of 10 CPU-clock periods (one taken DBcc is 10 clocks) and
; land back on a multiple of T_E; plain NOPs do not share that property.
        ifnd    K3NOPS
K3NOPS  equ     2
        endc

        org     PROGRAM_ORG

start:
        SETUP
        ifd     MUT_SHORT
        RSAMPLE1 199,P6_K1,CIAA_PRB
        else
        ifd     MUT_K1CHIP
        RSAMPLE1_CHIP 200,P6_K1
        else
        RSAMPLE1 200,P6_K1,CIAA_PRB
        endc
        endc
        RSAMPLE1 200,P6_K2,CIAB_PRB

        lea     P6_K3,a1
        ifd     MUT_K3MIX
        move.w  #99,d2
.k3:    moveq   #0,d0
        move.b  CIAA_PRB,d0
        move.w  d0,(a1)+
        moveq   #0,d0
        move.b  CHIPBYTE_K3,d0
        move.w  d0,(a1)+
        dbra    d2,.k3
        else
        move.w  #199,d2
.k3:    moveq   #0,d0
        move.b  CHIPBYTE_K3,d0
        move.w  d0,(a1)+
        rept    K3NOPS
        nop
        endr
        dbra    d2,.k3
        endc

        FINISH
