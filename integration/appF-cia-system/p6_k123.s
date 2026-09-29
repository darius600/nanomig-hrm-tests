; p6_k123.s -- P6: three loops of 200 iterations, one store each. K1 reads
; CIAA PRB, K2 reads CIAB PRB, K3 (contrast) reads a Chip-memory byte
; instead, padded so its own iteration time is not a multiple of 10*T_CPU.
; SPECIFICATION.md P6.
                include "nanomig.i"
                include "ciaregs.i"

P6_K1           equ     RESULTS+8       ; 200 words
P6_K2           equ     P6_K1+200*2     ; 200 words
P6_K3           equ     P6_K2+200*2     ; 200 words

; K3's own padding: two NOPs (4 CPU-clock periods each, SPECIFICATION.md
; P6), chosen so the loop body's iteration time is not a multiple of
; 10*T_CPU on either core (a builder-tuned value, guarded by Pre6.1, which
; the run checks against the measured I3, per core). A whole extra dbra
; loop would only ever shift I3 by a multiple of 10 CPU-clock periods (one
; taken DBcc is 10 clocks), landing back on a multiple of T_E; plain NOPs
; do not share that property.

        org     PROGRAM_ORG

start:
        SETUP
        RSAMPLE1 200,P6_K1,CIAA_PRB
        RSAMPLE1 200,P6_K2,CIAB_PRB

        lea     P6_K3,a1
        move.w  #199,d2
.k3:    moveq   #0,d0
        move.b  CHIPBYTE_K3,d0
        move.w  d0,(a1)+
        nop
        nop
        dbra    d2,.k3

        DONE
