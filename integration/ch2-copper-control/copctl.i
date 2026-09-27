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
MARKBUF         equ     $0C1000         ; M1, M2, MH1, MH2, ... one word each
RECBUF          equ     $0C4000         ; beam/poll sample records, 6 bytes each

; One beam/poll-sample record (SPECIFICATION.md 2.4, extended with INTREQR
; per 2.6 where a proposition needs it -- harmless extra data otherwise):
; INTREQR (word) then VPOSR/VHPOSR read as one long word, 6 bytes total,
; advancing a6. a0 must hold CUSTOM.
SAMPLE          macro
                move.w  INTREQR(a0),(a6)+
                move.l  VPOSR(a0),(a6)+
                endm

; Same 6-byte record layout as SAMPLE, but the first word is a constant
; (not a real INTREQR read): for a proposition that does not grade INTREQR,
; this halves the per-record register-read cost, needed to keep the
; anchor-B2 density (>= 8 records/line) with only a `dbra` loop around it.
FAST_SAMPLE     macro
                clr.w   (a6)+
                move.l  VPOSR(a0),(a6)+
                endm

; Precondition-(c)-style mutation: a fixed delay inserted between samples,
; sparse enough to fail B2/B3 (SPECIFICATION.md 2.4's own contrasting
; case). Clobbers d6.
SLOWDELAY       macro
                move.w  #60,d6
.sd\@:          dbra    d6,.sd\@
                endm
