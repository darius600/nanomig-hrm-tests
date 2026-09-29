; prog_t2s.s -- P3B (SPECIFICATION.md section 3): each byte is output as a
; two's-complement value independent of byte position, graded on a table
; with distinct codes in both byte positions (2.6: T2's own equal-byte
; words "do not depend on byte order", so T2 alone cannot show this).
;
; Runs as its own program, independent of prog_audio.s and its own
; mutations (A1/A2/A4/P1-P4): T2S's word content is poked directly into
; chip RAM by the C++ test (poke_word(), the same mechanism prog_audio.s's
; tables use) before boot(), and P3B's own contrasting case (the two
; words' bytes swapped) is pure data, so no assembly-time symbol is
; needed for it either.
;
;   MUT_FIXTURE  KFIX stores $DEAD instead of $BEEF (FIXTURE's own contrasting case)

        include "nanomig.i"
        include "ch5regs.i"

        org     PROGRAM_ORG

T2S_ADDR equ    $002000         ; 2 words, below $080000 (2.1)
KFIX    equ     RESULTS+$20     ; FIXTURE: a program-stored known value, checked by
                                 ; the test before it relies on anything else this
                                 ; program stores (BRIEF.md). Stored last, right
                                 ; before DONE, so it cannot perturb T2S's own DMA-
                                 ; restart timing (SPECIFICATION.md section 5).

; Waits for one VERTB pulse (INTREQR bit 5), clearing it afterwards, with
; a bounded fallback loop counter so the wait can never hang (2.3
; "Bounded waits"; how this program waits is not graded).
WAITVBL macro
                move.l  #200000,d7
.wv\@           btst    #5,INTREQR(a0)
                bne.s   .wvd\@
                dbra    d7,.wv\@
.wvd\@          move.w  #$0020,INTREQ(a0)
                endm

start:
        CLEAR_OVL
        lea     CUSTOM,a0

        ; ---- preamble (2.3) ----
        move.w  #$7FFF,INTENA(a0)
        move.w  #$7FFF,INTREQ(a0)
        move.w  #$7FFF,DMACON(a0)
        move.w  #$7FFF,ADKCON(a0)

        ; ---- channels 1-3 stay off (p.266 "000000=no output") ----
        move.w  #0,AUD1VOL(a0)
        move.w  #0,AUD2VOL(a0)
        move.w  #0,AUD3VOL(a0)

        ; ---- table T2S (P3B) ----
        move.l  #T2S_ADDR,AUD0LCH(a0)  ; one 32-bit write sets both halves (p.139)
        move.w  #2,AUD0LEN(a0)
        move.w  #64,AUD0VOL(a0)        ; p.140: 64 = maximum volume
        move.w  #128,AUD0PER(a0)       ; p.140/p.266 minimum is 124; well above it
        move.w  #$8201,DMACON(a0)      ; SET, DMAEN, AUD0EN (p.144)
        WAITVBL
        move.w  #$0001,DMACON(a0)      ; clears AUD0EN only (p.144)

        ; ---- FIXTURE: a known constant (BRIEF.md), stored last so it
        ; cannot perturb T2S's own DMA-restart timing above ----
        ifd     MUT_FIXTURE
        move.w  #$DEAD,KFIX            ; MUT_FIXTURE: a different constant
        else
        move.w  #$BEEF,KFIX
        endc

        DONE

; No table data is assembled into this binary: T2S's two words are poked
; directly into chip RAM by the C++ test's run_t2s() (poke_word() at
; T2S_ADDR, before boot()), the single source of truth for their content,
; the P3B mutation included, so that no copy of the data can drift from
; what the test actually checks.
