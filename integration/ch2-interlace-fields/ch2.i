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

; CIA-A: PRA bit 0 is OVL (p339, Appendix F CIAA address map: "BFE001 pra
; ... OVL"). p223: "The OVL bit in one of the 8520 Chips disables the
; overlay." p339 gives DDRA (port A's direction register) as "1-output
; (set to 0x03)". Programs here write DDRA before PRA, then clear PRA's
; OVL bit, only so they can write an interrupt vector at $6C.
CIAAPRA         equ     $BFE001
CIAADDRA        equ     $BFE201
