; sprb_data.i -- data tables for ch4-shres-sprite-b's own program.
;
; Included at the END of prog_main.s, after all code, so these tables land
; immediately after the program in the flat binary instead of opening a
; huge gap: sprb.i's constants have no ORG of their own, so a data table
; placed before `org PROGRAM_ORG` would sit at address 0, and vasm's
; -Fbin output then spans the whole gap up to PROGRAM_ORG.

; ---- sprite colour descriptor table (SPECIFICATION.md 3.2) ---------------
;
; 50 source lines (5 bands of 10), low word then high word per line,
; identical for both sprites; the same table is copied to SPR0DAT+4 and
; SPR1DAT+4. Two terminator words end it. Pixel data does not vary by
; band -- only the colour registers (COPTAB below) do.
SPRDESC:
    rept 5
                dc.w    $FFFF,$0000, $FFFF,$0000       ; L1, L1
                dc.w    $0000,$FFFF, $0000,$FFFF       ; L2, L2
                dc.w    $FFFF,$FFFF, $FFFF,$FFFF       ; L3, L3
                dc.w    $D9D1,$2F94, $D9D1,$2F94       ; LA, LA
                dc.w    $0000,$0000, $0000,$0000       ; gap, gap
    endr
                ; terminator (PC2 contrast CR5: content mismatch after the
                ; last graded line, no sprite pixel depends on it)
                ifd     MUT_CR5
                dc.w    $0001,$0000
                else
                dc.w    $0000,$0000
                endif
SPRDESC_END:

; ---- Copper list table (SPECIFICATION.md 3.5) -----------------------------
;
; 306 words, copied verbatim into COPLIST. Every address/data pair below is
; a literal transcription of section 3.5 and the colour-set table of 3.3;
; nothing here is computed. Word offsets from COPLIST (0-indexed), matching
; the C++ test's own constants:
;   0..31    sprite pointers (SPR0..7 PTH/L)
;   32..35   BPL1PTH/L
;   36..67   COLOR00-15, pattern set
;   68..99   COLOR16-31, section T group (S0 / CR1->S2 / CR2->S1)
;   100..101 WAIT line 58
;   102..133 COLOR00-15, dark set
;   134..135 WAIT line 88 (CR6 -> $5C07 instead of $5807)
;   136..167 COLOR16-31, S1
;   168..169 WAIT line 98
;   170..201 COLOR16-31, S2
;   202..203 WAIT line 108
;   204..235 COLOR16-31, S3
;   236..237 WAIT line 118
;   238..269 COLOR16-31, band-4 group (S0x / CR3->S0-c18)
;   270..271 WAIT line 132
;   272..303 COLOR00-15, pattern set (again)
;   304..305 WAIT $FFFF (never completes)
COPTAB:
                dc.w    $0120,$0001, $0122,$1000       ; SPR0PTH/L <- SPR0DAT
                dc.w    $0124,$0001, $0126,$1200       ; SPR1PTH/L <- SPR1DAT
                dc.w    $0128,$0001, $012A,$1400       ; SPR2PTH/L <- NULLSPR
                dc.w    $012C,$0001, $012E,$1400       ; SPR3PTH/L <- NULLSPR
                dc.w    $0130,$0001, $0132,$1400       ; SPR4PTH/L <- NULLSPR
                dc.w    $0134,$0001, $0136,$1400       ; SPR5PTH/L <- NULLSPR
                dc.w    $0138,$0001, $013A,$1400       ; SPR6PTH/L <- NULLSPR
                dc.w    $013C,$0001, $013E,$1400       ; SPR7PTH/L <- NULLSPR
                dc.w    $00E0,$0002, $00E2,$0000       ; BPL1PTH/L <- BPLBUF

                ; COLOR00-15, pattern set
                dc.w    $0180,$0000, $0182,$0CCC, $0184,$0000, $0186,$0000
                dc.w    $0188,$0333, $018A,$0FFF, $018C,$0333, $018E,$0333
                dc.w    $0190,$0000, $0192,$0CCC, $0194,$0000, $0196,$0000
                dc.w    $0198,$0000, $019A,$0CCC, $019C,$0000, $019E,$0000

                ; COLOR16-31, section T (S0 baseline; CR1->S2; CR2->S1)
                ifd     MUT_CR1
                dc.w    $01A0,$0AF0, $01A2,$0E34, $01A4,$0278, $01A6,$06BC
                dc.w    $01A8,$0BC1, $01AA,$0F05, $01AC,$0349, $01AE,$078D
                dc.w    $01B0,$08D2, $01B2,$0C16, $01B4,$005A, $01B6,$049E
                dc.w    $01B8,$09E3, $01BA,$0D27, $01BC,$016B, $01BE,$05AF
                else
                ifd     MUT_CR2
                dc.w    $01A0,$05AF, $01A2,$09E3, $01A4,$0D27, $01A6,$016B
                dc.w    $01A8,$06BC, $01AA,$0AF0, $01AC,$0E34, $01AE,$0278
                dc.w    $01B0,$078D, $01B2,$0BC1, $01B4,$0F05, $01B6,$0349
                dc.w    $01B8,$049E, $01BA,$08D2, $01BC,$0C16, $01BE,$005A
                else
                dc.w    $01A0,$005A, $01A2,$049E, $01A4,$08D2, $01A6,$0C16
                dc.w    $01A8,$016B, $01AA,$05AF, $01AC,$09E3, $01AE,$0D27
                dc.w    $01B0,$0278, $01B2,$06BC, $01B4,$0AF0, $01B6,$0E34
                dc.w    $01B8,$0349, $01BA,$078D, $01BC,$0BC1, $01BE,$0F05
                endif
                endif

                dc.w    $3A07,$FFFE                     ; WAIT line 58

                ; COLOR00-15, dark set
                dc.w    $0180,$0000, $0182,$0000, $0184,$0000, $0186,$0000
                dc.w    $0188,$0000, $018A,$0000, $018C,$0000, $018E,$0000
                dc.w    $0190,$0000, $0192,$0000, $0194,$0000, $0196,$0000
                dc.w    $0198,$0000, $019A,$0000, $019C,$0000, $019E,$0000

                ; WAIT line 88 (CR6 -> $5C07 instead of $5807)
                ifd     MUT_CR6
                dc.w    $5C07,$FFFE
                else
                dc.w    $5807,$FFFE
                endif

                ; COLOR16-31, S1
                dc.w    $01A0,$05AF, $01A2,$09E3, $01A4,$0D27, $01A6,$016B
                dc.w    $01A8,$06BC, $01AA,$0AF0, $01AC,$0E34, $01AE,$0278
                dc.w    $01B0,$078D, $01B2,$0BC1, $01B4,$0F05, $01B6,$0349
                dc.w    $01B8,$049E, $01BA,$08D2, $01BC,$0C16, $01BE,$005A

                dc.w    $6207,$FFFE                     ; WAIT line 98

                ; COLOR16-31, S2
                dc.w    $01A0,$0AF0, $01A2,$0E34, $01A4,$0278, $01A6,$06BC
                dc.w    $01A8,$0BC1, $01AA,$0F05, $01AC,$0349, $01AE,$078D
                dc.w    $01B0,$08D2, $01B2,$0C16, $01B4,$005A, $01B6,$049E
                dc.w    $01B8,$09E3, $01BA,$0D27, $01BC,$016B, $01BE,$05AF

                dc.w    $6C07,$FFFE                     ; WAIT line 108

                ; COLOR16-31, S3
                dc.w    $01A0,$0F05, $01A2,$0349, $01A4,$078D, $01A6,$0BC1
                dc.w    $01A8,$0C16, $01AA,$005A, $01AC,$049E, $01AE,$08D2
                dc.w    $01B0,$0D27, $01B2,$016B, $01B4,$05AF, $01B6,$09E3
                dc.w    $01B8,$0E34, $01BA,$0278, $01BC,$06BC, $01BE,$0AF0

                dc.w    $7607,$FFFE                     ; WAIT line 118

                ; COLOR16-31, band 4 (S0x baseline; CR3 -> S0-c18)
                ifd     MUT_CR3
                dc.w    $01A0,$005A, $01A2,$049E, $01A4,$041E, $01A6,$0C16
                dc.w    $01A8,$016B, $01AA,$05AF, $01AC,$052F, $01AE,$0D27
                dc.w    $01B0,$014B, $01B2,$058F, $01B4,$050F, $01B6,$0D07
                dc.w    $01B8,$0349, $01BA,$078D, $01BC,$070D, $01BE,$0F05
                else
                dc.w    $01A0,$0FA5, $01A2,$07AD, $01A4,$0BE1, $01A6,$0F25
                dc.w    $01A8,$0DA7, $01AA,$05AF, $01AC,$09E3, $01AE,$0D27
                dc.w    $01B0,$0EB4, $01B2,$06BC, $01B4,$0AF0, $01B6,$0E34
                dc.w    $01B8,$0F85, $01BA,$078D, $01BC,$0BC1, $01BE,$0F05
                endif

                dc.w    $8407,$FFFE                     ; WAIT line 132

                ; COLOR00-15, pattern set (again)
                dc.w    $0180,$0000, $0182,$0CCC, $0184,$0000, $0186,$0000
                dc.w    $0188,$0333, $018A,$0FFF, $018C,$0333, $018E,$0333
                dc.w    $0190,$0000, $0192,$0CCC, $0194,$0000, $0196,$0000
                dc.w    $0198,$0000, $019A,$0CCC, $019C,$0000, $019E,$0000

                dc.w    $FFFF,$FFFE                     ; WAIT, never completes
COPTAB_END:
