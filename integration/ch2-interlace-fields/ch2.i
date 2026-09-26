; ch2.i -- custom-chip register offsets that nanomig.i does not define, from
; Appendix B (Register Summary Address Order), plus the fixed CIA-A address
; used only to release the reset-time ROM overlay so a program can install
; its own interrupt vector (see prog_i.s). Offsets from CUSTOM ($DFF000).

COPCON          equ     $02E
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088
DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094
BPL1PTH         equ     $0E0
BPL1PTL         equ     $0E2
BPLCON0         equ     $100
BPLCON1         equ     $102
BPLCON2         equ     $104
BPL1MOD         equ     $108
BPL2MOD         equ     $10A
SPR0PTH         equ     $120
SPR0PTL         equ     $122
SPR1PTH         equ     $124
SPR1PTL         equ     $126
SPR2PTH         equ     $128
SPR2PTL         equ     $12A
SPR3PTH         equ     $12C
SPR3PTL         equ     $12E
SPR4PTH         equ     $130
SPR4PTL         equ     $132
SPR5PTH         equ     $134
SPR5PTL         equ     $136
SPR6PTH         equ     $138
SPR6PTL         equ     $13A
SPR7PTH         equ     $13C
SPR7PTL         equ     $13E
COLOR01         equ     $182
COLOR17         equ     $1A2
COLOR18         equ     $1A4
COLOR19         equ     $1A6

; CIA-A: any write to its address range clears the reset-time ROM/RAM
; overlay (minimig.v:861-862, sel_cia_a decoded in gary.v from address bits
; [23:16]==$BF and [12]==0, matching the real machine's $BFxxx1 CIA-A
; range). This is not a custom chip register; it is the ordinary,
; HRM-documented mechanism (the CIA-A PRA OVL bit) a real 68000 program
; uses before it can write anything into low memory, e.g. an interrupt
; vector at $6C -- while the overlay is active, $000000-$07FFFF reads the
; Kickstart shadow and swallows writes (nanomig_harness.h's own address
; space notes). Programs here use it only for that purpose.
CIAAPRA         equ     $BFE001
