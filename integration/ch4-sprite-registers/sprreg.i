; sprreg.i -- ch4-sprite-registers's own constants: custom-chip register
; offsets not already in nanomig.i (Appendix B, Register Summary Address
; Order, printed pages 289-292), the CIA-A overlay registers (p.223,
; p.339), and this package's chip-RAM addresses (SPECIFICATION.md 3.2).

; -- CIA-A: clearing the memory overlay (SPECIFICATION.md 3.2, p.223, p.339) --
; "It writes $03 to ddra ($BFE201, ... 1=output) and then $00 to pra
; ($BFE001), whose bit 0 is OVL". Both are byte registers at odd addresses.
CIAADDRA        equ     $BFE201
CIAAPRA         equ     $BFE001

CLEAR_OVL       macro
                move.b  #$03,CIAADDRA
                move.b  #$00,CIAAPRA
                endm

; -- Appendix B, address order (pp.289-292), not already in nanomig.i --
COPCON          equ     $02E
COP1LCH         equ     $080
COP1LCL         equ     $082
COP2LCH         equ     $084
COP2LCL         equ     $086
COPJMP1         equ     $088
COPJMP2         equ     $08A
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
SPR0POS         equ     $140
SPR0CTL         equ     $142
SPR0DATA        equ     $144
SPR0DATB        equ     $146
SPR4POS         equ     $160
SPR4CTL         equ     $162
SPR4DATA        equ     $164
SPR4DATB        equ     $166
COLOR01         equ     $182
COLOR17         equ     $1A2
COLOR18         equ     $1A4
COLOR19         equ     $1A6
COLOR25         equ     $1B2
COLOR26         equ     $1B4
COLOR27         equ     $1B6
BLTCON0L        equ     $05A            ; ECS: low bits of BLTCON0 (p.309)
BLTSIZV         equ     $05C            ; ECS: blit height in rows (p.308)
BLTSIZH         equ     $05E            ; ECS: blit width in words; starts the blit (p.308)
BEAMCON0        equ     $1DC            ; ECS: beam counter control (p.298, p.305)

; -- SPECIFICATION.md 3.2's own byte addresses, below $80000 (this
; package's scope: P4-P11 only; RULER/SWP/SREF are P1-P3's, not built,
; see SPECIFICATION.md's build notes) --
COPLIST_L1      equ     $020000
COPLIST_L2      equ     $020800
NULL_SPRITE     equ     $021000         ; $0000,$0000: the safe null sprite (p.111, p.114)
S1_DATA         equ     $025000
S2_DATA         equ     $027000
S4_DATA         equ     $028000
SWP_DATA        equ     $029000         ; sprite-2 structure (P1-P3)
SREF_DATA       equ     $02A000         ; reference sprite-0 structure (P1-P3)
BLIT_BASE       equ     $030000
RULER_BUF       equ     $040000         ; bitplane buffer; words $FF00 (P1-P3)
ZERO_BUF        equ     $050000         ; bitplane buffer; every word $0000 (SPECIFICATION.md's
                                        ; table). The harness's chip-RAM model starts zeroed
                                        ; (nanomig_harness.h, NanomigHarness's mem_ constructor),
                                        ; and no assertion this package builds inspects a
                                        ; bitplane pixel (every run below uses the plain colour
                                        ; set, black on black, 3.6), so no program writes it.

; -- SPECIFICATION.md 3.2's own results-area words --
MARK            equ     RESULTS+$0100   ; one word per event, offsets fixed per program below
DECOY           equ     RESULTS+$0200   ; one word, preloaded $A5A5 by the C++ side
; RESULT is nanomig.i's own RESULTS ($0C0000). RESULT+0 holds each program's
; fixture constant; the propositions' own RESULT+n offsets follow
; SPECIFICATION.md section 4 (RESULT+2 = DMACONR, etc.).
