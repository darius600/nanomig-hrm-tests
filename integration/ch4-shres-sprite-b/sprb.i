; sprb.i -- shared constants, macros and data tables for
; ch4-shres-sprite-b's own program (prog_main.s and its mutant wrappers).
; Package-local; not part of the harness contract (nanomig.i is).
;
; Custom registers Appendix B gives but nanomig.i does not define, needed
; for this package's own CPU-side (a0-relative) writes. The Copper list's
; own MOVE targets are written as literal register addresses in COPTAB
; below (SPECIFICATION.md 3.5) and never need a symbol.
DIWSTRT         equ     $08E
DIWSTOP         equ     $090
DDFSTRT         equ     $092
DDFSTOP         equ     $094
BPLCON0         equ     $100
BPLCON1         equ     $102
BPLCON2         equ     $104
BPL1MOD         equ     $108
COP1LCH         equ     $080
COPJMP1         equ     $088
DENISEID        equ     $07C    ; Appendix B p.290; PC0 reads this back (p.299)

; CIA-A PRA bit 0 is the OVL bit: on reset "an additional copy of the system
; ROM responds starting at memory location $00000000. The system RAM that
; would normally be located at this address is not available. The OVL bit
; in one of the 8520 Chips disables the overlay" (p. 223; bit location,
; Appendix F: p. 339 lists pra at $BFE001, p. 351 names bit 0 "OVL ...
; memory overlay bit"). The program clears it first, before anything else,
; because COPLIST/SPR0DAT/SPR1DAT/BPLBUF (SPECIFICATION.md 3.2) all sit
; below $080000.
CIAAPRA         equ     $BFE001

CLEAR_OVL       macro
                move.b  #0,CIAAPRA
                endm

; SPECIFICATION.md 3.2 -- chip-memory layout.
COPLIST         equ     $010000
SPR0DAT         equ     $011000
SPR1DAT         equ     $011200
NULLSPR         equ     $011400
MARKER          equ     $011500
BPLBUF          equ     $020000

; ---- mutation-selected constants (SPECIFICATION.md section 5/6) ----------
;
; Exactly one of MUT_CR1 ... MUT_CR13 (CR7 and CR12-CR13 included) is defined (equ'd to 1) by a mutant wrapper
; before it includes prog_main.s; prog_main.s itself (assembled standalone)
; defines none of them, giving the baseline/normal stimulus.

; PC1 contrast CR4: BPLCON0 = $9000 (HIRES=1, SHRES=0) instead of $1040.
                ifd     MUT_CR4
BPLCON0VAL      equ     $9000
                else
BPLCON0VAL      equ     $1040
                endif

; PC4 contrast CR8: MARKER is written after three frame starts instead of six.
                ifd     MUT_CR8
FRAME_COUNT     equ     3
                else
FRAME_COUNT     equ     6
                endif

; REF contrast CR11: bits 3-2 of every component of every COLOR16-31 word
; the Copper writes are cleared (word & $F333), so no sprite sample is lit
; after the test's 2-bit mask while sprite DMA, the pattern set and the
; whole Copper list run as before.
                ifd     MUT_CR11
CMASK           equ     $F333
                else
CMASK           equ     $FFFF
                endif

; SU contrast CR13: sprite 1's POS word holds HSTART 221 ($6E, CTL bit 0 = 1)
; instead of 217 ($6C).
                ifd     MUT_CR13
SPR1POSVAL      equ     $506E
                else
SPR1POSVAL      equ     $506C
                endif

; PC0 contrasts (INTENAR comes from nanomig.i): the register read into RESULTS+2 (VPOSR slot, CR12) and
; RESULTS+4 (DENISEID slot, CR7) is INTENAR instead.
                ifd     MUT_CR12
IDREG_VPOSR     equ     INTENAR
                else
IDREG_VPOSR     equ     VPOSR
                endif
                ifd     MUT_CR7
IDREG_DENISE    equ     INTENAR
                else
IDREG_DENISE    equ     DENISEID
                endif

DMACONVAL       equ     $83A0

; FIXTURE contrast CR9: RESULTS stores $DEAD instead of $BEEF.
                ifd     MUT_CR9
FIXTUREVAL      equ     $DEAD
                else
FIXTUREVAL      equ     $BEEF
                endif

; Data tables (SPRDESC, COPTAB) are in sprb_data.i, included at the end of
; prog_main.s (after all code) so they land right after the program in the
; flat binary instead of opening a gap back to address 0.
