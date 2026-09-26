; ch5_regs.i -- register and memory-layout equates shared by every
; program in this package. Included after nanomig.i, before any org (pure
; equates, no code, safe anywhere). Register addresses are Appendix B
; (p.289-291) and Appendix A (p.265-266, p.278, p.280), transcribed in
; SPECIFICATION.md's "Conventions/System" section.

ADKCON          equ     $09E    ; write
ADKCONR         equ     $010    ; read

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

; Data-set regions: one per channel, spaced 4 KB apart, all inside the
; identity-mapped chip RAM and below RESULTS ($0C0000), so a channel's own
; fetches are told apart from every other region by address alone (PC3's
; "own region" reference frame).
CH0_DATA        equ     $088000
CH1_DATA        equ     $089000
CH2_DATA        equ     $08A000
CH3_DATA        equ     $08B000

; Result area (RESULTS = $0C0000, nanomig.i). FIXTURE_STORE is the
; program-stored constant asserted before anything else is read (the
; fixture requirement); ADKR_STORE/DMACR_STORE are where step 5/6 store
; the words read back from ADKCONR/DMACONR (PC1/PC2); MARKER is the fixed
; address every session/sweep marker is written to, each write carrying
; its own distinct word, per the marker convention.
FIXTURE_STORE   equ     RESULTS+0
ADKR_STORE      equ     RESULTS+2
DMACR_STORE     equ     RESULTS+4
MARKER          equ     RESULTS+$10
