; playfield.i -- register addresses this package needs beyond nanomig.i,
; from the Hardware Reference Manual's Appendix B (pp.289-293), offsets from
; CUSTOM ($DFF000). nanomig.i already defines DMACONR, VPOSR, VHPOSR,
; INTENAR, INTREQR, DMACON, INTENA, INTREQ, COLOR00, CUSTOM.

COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088
DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094
BPL1PTH         equ     $0E0
BPL1PTL         equ     $0E2
BPL2PTH         equ     $0E4
BPL2PTL         equ     $0E6
BPL3PTH         equ     $0E8
BPL3PTL         equ     $0EA
BPL4PTH         equ     $0EC
BPL4PTL         equ     $0EE
BPLCON0         equ     $100
BPLCON1         equ     $102
BPLCON2         equ     $104
BPL1MOD         equ     $108
BPL2MOD         equ     $10A
COLOR01         equ     $182
COLOR02         equ     $184
COLOR03         equ     $186
COLOR04         equ     $188
COLOR05         equ     $18A
COLOR06         equ     $18C
COLOR07         equ     $18E
COLOR08         equ     $190
COLOR09         equ     $192
COLOR10         equ     $194
COLOR11         equ     $196
COLOR12         equ     $198
COLOR13         equ     $19A
COLOR14         equ     $19C
COLOR15         equ     $19E
VBSTRT          equ     $1CC
VBSTOP          equ     $1CE
BEAMCON0        equ     $1DC

; Where a program's read/marker bursts are stored. Chosen clear of
; DONE_ADDR ($0C0FFE, from nanomig.i) so a burst never overwrites it before
; DONE is reached.
MEASURE_BASE    equ     $0C1000

; Physical addresses below $40000 (even, per SPECIFICATION.md "Common
; definitions") where the test (not the program -- the overlay blocks a CPU
; write there, see nanomig_harness.h's address-space notes) preloads a
; Copper list or bitplane data with NanomigHarness::poke_word/poke_long
; before boot(). Each program that expects such a preload only points a
; register at the fixed address; it never writes the bytes there itself.
START_ADDR      equ     $0C0FF0         ; the start word every program stores first (SPECIFICATION.md)
COP_LIST_ADDR   equ     $020000
BPL_PLANE1_ADDR equ     $028000
BPL_PLANE2_ADDR equ     $02D000
BPL_PLANE3_ADDR equ     $032000
BPL_PLANE4_ADDR equ     $037000
