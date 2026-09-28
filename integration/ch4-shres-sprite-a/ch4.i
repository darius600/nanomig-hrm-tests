; ch4.i -- ch4-shres-sprite-a's own constants: custom-chip register offsets
; not already in nanomig.i (Appendix B, Register Summary Address Order,
; printed pages 289-292) and this package's chip-RAM data addresses.
; CUSTOM, DMACON, INTENA, COLOR00 are nanomig.i's own.
;
; SPECIFICATION.md section 3.2: everything is in Chip memory below $80000,
; word aligned (p.104). Sprite and bitplane pointers keep the high three
; bits of a Chip RAM address (p.110; p.285, p.291), and the Copper location
; register has two additional bits under the Enhanced Chip Set (p.309).

; -- Appendix B, address order --
COP1LCH         equ     $080
COP1LCL         equ     $082
COPJMP1         equ     $088
DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094
BPL1PTH         equ     $0E0
BPL1PTL         equ     $0E2
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
BPLCON0         equ     $100
BPLCON1         equ     $102
BPLCON2         equ     $104
BPL1MOD         equ     $108
DENISEID        equ     $07C
COLOR16         equ     $1A0

; -- this package's chip-RAM data addresses (below $80000, word aligned) --
SPR0_DATA       equ     $002000         ; sprite 0: POS,CTL,145 pairs,$0000,$0000 (588 bytes)
SPR1_DATA       equ     $002400         ; sprite 1: same layout, different POS
; sprites 2-7's own safe null structures (p.114: "all eight sprite pointers
; must be initialized to either a real sprite or a safe null sprite"), each
; independently identifiable so PC3 can require every one of the six to be
; read on its own address, not any one of them standing in for the rest:
NULL2_DATA      equ     $002800         ; $0000,$0000 -- sprite 2's safe null structure
NULL3_DATA      equ     $002804         ; sprite 3's own
NULL4_DATA      equ     $002808         ; sprite 4's own
NULL5_DATA      equ     $00280C         ; sprite 5's own
NULL6_DATA      equ     $002810         ; sprite 6's own
NULL7_DATA      equ     $002814         ; sprite 7's own
BPL_BUF         equ     $010000         ; 32768 words, each $FF00 (64 KB)
COP_LIST        equ     $020000         ; the Copper list of section 3.4

; the two-word result area of section 3.2 ("Result area: two words"):
; RESULTS+0 holds the DENISEID readback (recorded, not graded, step 9);
; RESULTS+2 holds the fixture constant (step 9). The completion marker of
; step 10 is nanomig.i's own DONE macro (DONE_ADDR), which is what
; run_until_write() actually waits for.
