; copctl.i -- shared constants and macros for ch2-copper-control's own
; programs. Package-local; not part of the harness contract (nanomig.i is).
;
; Custom registers Appendix B gives but nanomig.i does not define, needed by
; this package's own CPU-side (a0-relative) accesses. Copper-list *content*
; is written as literal hex per SPECIFICATION.md 2.2 and never needs these
; symbols.
COPCON          equ     $02E
COP1LCH         equ     $080
COP2LCH         equ     $084
COPJMP1         equ     $088
COPJMP2         equ     $08A

; DFF register not in nanomig.i (not a custom DMA register): any write here
; clears the overlay, so the low $000000-$07FFFF region becomes reachable
; for both CPU and DMA access. All this package's Copper lists and
; DMA-channel data regions sit below $80000 (SPECIFICATION.md 2.1), so
; every program clears the overlay first.
CIAAPRA         equ     $BFE001

CLEAR_OVL       macro
                move.b  #0,CIAAPRA
                endm

; Common prologue (SPECIFICATION.md 2.1). a0 must already hold CUSTOM.
CLEAR_DMA_INT   macro
                move.w  #$07FF,DMACON(a0)
                move.w  #$7FFF,INTENA(a0)
                move.w  #$3FFF,INTREQ(a0)
                endm

; This package's own low-memory area ($000000-$07FFFF), reachable only
; after CLEAR_OVL: Copper lists and DMA-channel data regions. Each program
; uses only the labels it needs; a fresh NanomigHarness/mem_ instance is
; created per run, so the same addresses are reused safely across programs.
LIST1           equ     $001000
LIST2           equ     $001200
LIST3           equ     $001400
LIST4           equ     $001600
DATA1           equ     $002000
DATA2           equ     $004000

; Ordinary identity-mapped chip RAM scratch: CPU-only cells (no DMA pointer
; ever addresses these), so no overlay handling needed. RESULTS/DONE_ADDR
; are nanomig.i's.
MARKBUF         equ     $0C1000         ; M1(+0), M2(+2), MS(+4, P4), MPC(+6, P2)
RECBUF          equ     $0C4000         ; beam/poll sample records, 8 bytes each

; One beam/poll-sample record (SPECIFICATION.md 2.4, extended with INTREQR
; per 2.6 where a proposition needs it -- harmless extra data otherwise):
; INTREQR, then VPOSR/VHPOSR read as one long word, then VPOSR read again --
; 4 words, 8 bytes, advancing a6. The re-read lets the C++ side keep a
; record only when both VPOSR reads agree on bit 0 (V8): VPOSR and VHPOSR
; are two separate bus reads (p.216), so a record whose V crosses 255->256
; or 312->0 between them can otherwise tear. a0 must hold CUSTOM.
SAMPLE          macro
                move.w  INTREQR(a0),(a6)+
                move.l  VPOSR(a0),(a6)+
                move.w  VPOSR(a0),(a6)+
                endm

; Same 4-word record layout as SAMPLE, but the first word is a constant
; (not a real INTREQR read): for a proposition that does not grade INTREQR,
; this halves the per-record register-read cost, needed to keep the
; anchor-B2 density (>= 8 records/line) with only a `dbra` loop around it.
FAST_SAMPLE     macro
                clr.w   (a6)+
                move.l  VPOSR(a0),(a6)+
                move.w  VPOSR(a0),(a6)+
                endm

; Precondition-(c)-style mutation: a fixed delay inserted between samples,
; sparse enough to fail B2/B3 (SPECIFICATION.md 2.4's own contrasting
; case). Clobbers d6.
SLOWDELAY       macro
                move.w  #60,d6
.sd\@:          dbra    d6,.sd\@
                endm

; ---- P3 and P5-P12's own additions (the lines above are still all
; P1/P2/P4 use). --------------------------------------------------------

; CIA-A DDRA (SPECIFICATION.md 2.1, p.339): P3 and P5-P12's own overlay
; clear writes this before CIAAPRA, unlike CLEAR_OVL above, which P1/P2/P4
; already use without it.
CIAADDRA        equ     $BFE201

CLEAR_OVL_DDR   macro
                move.b  #$03,CIAADDRA
                move.b  #0,CIAAPRA
                endm

; Custom registers Appendix B gives but nanomig.i and the block above do
; not define, needed by P3 and P5-P12's own CPU-side (a0-relative)
; accesses. Copper-list *content* is written as literal hex or with these
; same symbols per SPECIFICATION.md 2.2; neither needs new symbols beyond
; this set.
DSKBYTR         equ     $01A
DSKPTH          equ     $020
DSKPTL          equ     $022
DSKLEN          equ     $024
DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094
AUD0LCH         equ     $0A0
AUD0LCL         equ     $0A2
AUD0LEN         equ     $0A4
AUD0PER         equ     $0A6
AUD0VOL         equ     $0A8
AUD1LCH         equ     $0B0
AUD1LCL         equ     $0B2
AUD1LEN         equ     $0B4
AUD1PER         equ     $0B6
AUD1VOL         equ     $0B8
AUD2LCH         equ     $0C0
AUD2LCL         equ     $0C2
AUD2LEN         equ     $0C4
AUD2PER         equ     $0C6
AUD2VOL         equ     $0C8
AUD3LCH         equ     $0D0
AUD3LCL         equ     $0D2
AUD3LEN         equ     $0D4
AUD3PER         equ     $0D6
AUD3VOL         equ     $0D8
BPL1PTH         equ     $0E0
BPL1PTL         equ     $0E2
BPL2PTH         equ     $0E4
BPL2PTL         equ     $0E6
BPLCON0         equ     $100
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
COLOR02         equ     $184
COLOR03         equ     $186

; Wait until a SAMPLE-built record shows V within [lo,hi] -- the same
; wait-for-line idiom P1/P2/P4 already write inline, factored out for the
; programs below (P3, P5-P12) that use it more than once. a6 must point
; just past the record SAMPLE/FAST_SAMPLE last wrote; d7 counts records.
; Clobbers d0-d1.
; Positional macro parameters (vasm's mot syntax: \1, \2, ... -- named
; parameters are not used here since SLOWDELAY/\@ is the only form already
; proven in this file).
WAIT_V          macro
.wv\@:          SAMPLE
                addq.l  #1,d7
                move.l  -6(a6),d0
                move.l  d0,d1
                lsr.l   #8,d1
                and.l   #$1FF,d1
                cmpi.w  #\1,d1
                blt.s   .wv\@
                cmpi.w  #\2,d1
                bgt.s   .wv\@
                endm

WAIT_V_FAST     macro
.wvf\@:         FAST_SAMPLE
                addq.l  #1,d7
                move.l  -6(a6),d0
                move.l  d0,d1
                lsr.l   #8,d1
                and.l   #$1FF,d1
                cmpi.w  #\1,d1
                blt.s   .wvf\@
                cmpi.w  #\2,d1
                bgt.s   .wvf\@
                endm

; Busy-wait until INTREQR's bit \1 reads 1 (a plain poll, not a
; SAMPLE-built record; SPECIFICATION.md's own "poll INTREQR until bit N =
; 1" idiom, e.g. P3 step 6, P8's VB case). Reads the whole word into d0 and
; tests the bit there (BTST to memory is byte-only on the 68000, which
; would need the low-byte address for bits 0-7; this sidesteps that). a0
; must hold CUSTOM. Clobbers d0.
WAIT_INTREQ_SET macro
.wis\@:         move.w  INTREQR(a0),d0
                btst    #\1,d0
                beq.s   .wis\@
                endm

; Interrupt-level handler infrastructure (P9, P11; SPECIFICATION.md 2.6).
; Each handler record is 4 words (level, INTREQR, VPOSR/VHPOSR as one long
; word), 8 bytes, written through HPTR (a long pointer cell) and counted in
; HCOUNT. A program using this initialises HPTR = HANDLERBUF and HCOUNT = 0
; before installing any handler or enabling interrupts.
HANDLERBUF      equ     $0C8000
HPTR            equ     $0C7000
HCOUNT          equ     $0C7004
