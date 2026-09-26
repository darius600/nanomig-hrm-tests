; ch4.i -- ch4-shres-sprite-a's own constants: custom-chip register offsets
; not already in nanomig.i (Appendix B, Register Summary Address Order,
; printed pages 289-292) and this package's chip-RAM data addresses.
; CUSTOM, DMACON, INTENA, COLOR00 are nanomig.i's own.
;
; SPECIFICATION.md 3.2's "Chip memory below $80000" is cited to p.104 for
; word alignment only, not for the $80000 figure itself. This harness's
; Chip RAM is identity-mapped (physical == logical) at $080000-$1FFFFF
; regardless of the CIA-A overlay bit (nanomig_harness.h); addresses below
; $080000 are identity-mapped too, but only once a CPU write to CIA-A has
; cleared the overlay (gary.v:159,166; minimig.v:862) -- otherwise a read
; there is redirected to the kickstart shadow. Placing this package's data
; at $080000+ needs no such step and is still ordinary, word-aligned Chip
; RAM on the 2 MB machine the harness runs.

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

; -- this package's chip-RAM data addresses (identity-mapped, word aligned) --
SPR0_DATA       equ     $082000         ; sprite 0: POS,CTL,145 pairs,$0000,$0000 (588 bytes)
SPR1_DATA       equ     $082400         ; sprite 1: same layout, different POS
NULL_DATA       equ     $082800         ; $0000,$0000 -- sprites 2-7's safe null structure
BPL_BUF         equ     $090000         ; 32768 words, each $FF00 (64 KB)
COP_LIST        equ     $0A0000         ; the Copper list of section 3.4

; the two-word result area of section 3.2 ("Result area: two words"):
; RESULTS+0 holds the DENISEID readback (recorded, not graded, step 9); the
; completion marker of step 10 is nanomig.i's own DONE macro (DONE_ADDR),
; which is what run_until_write() actually waits for.
