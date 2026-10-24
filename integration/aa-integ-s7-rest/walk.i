; walk.i -- the 40 cases of run UW (SPECIFICATION.md section 4): per case the words
; BPLHPTH, BPLHPTL, SPRHPTH, SPRHPTL written after the marker of line $50 + n for line $51 + n.
; Pointer bit j = 0..14 is DB(j+1) of the low word, j = 15..19 is DB(j-15) of the high word.
; Case n = 0..19 walks a one at bit n; n = 20..39 walks a zero at bit n-20. The sprite pointer
; carries the case of index (n + 20) mod 40. WALK_MB / WALK_MS move case 7 to bit 8 (mutants).
WALKTAB:
        ; case 0, line $51: bit-plane one bit 0, sprite zero bit 0
        dc.w    $0000,$0002,$FFFF,$FFFD
        ; case 1, line $52: bit-plane one bit 1, sprite zero bit 1
        dc.w    $0000,$0004,$FFFF,$FFFB
        ; case 2, line $53: bit-plane one bit 2, sprite zero bit 2
        dc.w    $0000,$0008,$FFFF,$FFF7
        ; case 3, line $54: bit-plane one bit 3, sprite zero bit 3
        dc.w    $0000,$0010,$FFFF,$FFEF
        ; case 4, line $55: bit-plane one bit 4, sprite zero bit 4
        dc.w    $0000,$0020,$FFFF,$FFDF
        ; case 5, line $56: bit-plane one bit 5, sprite zero bit 5
        dc.w    $0000,$0040,$FFFF,$FFBF
        ; case 6, line $57: bit-plane one bit 6, sprite zero bit 6
        dc.w    $0000,$0080,$FFFF,$FF7F
        ; case 7, line $58: bit-plane one bit 7, sprite zero bit 7
        ifd     WALK_MB
        dc.w    $0000,$0200
        else
        dc.w    $0000,$0100
        endc
        ifd     WALK_MS
        dc.w    $FFFF,$FDFF
        else
        dc.w    $FFFF,$FEFF
        endc
        ; case 8, line $59: bit-plane one bit 8, sprite zero bit 8
        dc.w    $0000,$0200,$FFFF,$FDFF
        ; case 9, line $5A: bit-plane one bit 9, sprite zero bit 9
        dc.w    $0000,$0400,$FFFF,$FBFF
        ; case 10, line $5B: bit-plane one bit 10, sprite zero bit 10
        dc.w    $0000,$0800,$FFFF,$F7FF
        ; case 11, line $5C: bit-plane one bit 11, sprite zero bit 11
        dc.w    $0000,$1000,$FFFF,$EFFF
        ; case 12, line $5D: bit-plane one bit 12, sprite zero bit 12
        dc.w    $0000,$2000,$FFFF,$DFFF
        ; case 13, line $5E: bit-plane one bit 13, sprite zero bit 13
        dc.w    $0000,$4000,$FFFF,$BFFF
        ; case 14, line $5F: bit-plane one bit 14, sprite zero bit 14
        dc.w    $0000,$8000,$FFFF,$7FFF
        ; case 15, line $60: bit-plane one bit 15, sprite zero bit 15
        dc.w    $0001,$0000,$FFFE,$FFFF
        ; case 16, line $61: bit-plane one bit 16, sprite zero bit 16
        dc.w    $0002,$0000,$FFFD,$FFFF
        ; case 17, line $62: bit-plane one bit 17, sprite zero bit 17
        dc.w    $0004,$0000,$FFFB,$FFFF
        ; case 18, line $63: bit-plane one bit 18, sprite zero bit 18
        dc.w    $0008,$0000,$FFF7,$FFFF
        ; case 19, line $64: bit-plane one bit 19, sprite zero bit 19
        dc.w    $0010,$0000,$FFEF,$FFFF
        ; case 20, line $65: bit-plane zero bit 0, sprite one bit 0
        dc.w    $FFFF,$FFFD,$0000,$0002
        ; case 21, line $66: bit-plane zero bit 1, sprite one bit 1
        dc.w    $FFFF,$FFFB,$0000,$0004
        ; case 22, line $67: bit-plane zero bit 2, sprite one bit 2
        dc.w    $FFFF,$FFF7,$0000,$0008
        ; case 23, line $68: bit-plane zero bit 3, sprite one bit 3
        dc.w    $FFFF,$FFEF,$0000,$0010
        ; case 24, line $69: bit-plane zero bit 4, sprite one bit 4
        dc.w    $FFFF,$FFDF,$0000,$0020
        ; case 25, line $6A: bit-plane zero bit 5, sprite one bit 5
        dc.w    $FFFF,$FFBF,$0000,$0040
        ; case 26, line $6B: bit-plane zero bit 6, sprite one bit 6
        dc.w    $FFFF,$FF7F,$0000,$0080
        ; case 27, line $6C: bit-plane zero bit 7, sprite one bit 7
        dc.w    $FFFF,$FEFF,$0000,$0100
        ; case 28, line $6D: bit-plane zero bit 8, sprite one bit 8
        dc.w    $FFFF,$FDFF,$0000,$0200
        ; case 29, line $6E: bit-plane zero bit 9, sprite one bit 9
        dc.w    $FFFF,$FBFF,$0000,$0400
        ; case 30, line $6F: bit-plane zero bit 10, sprite one bit 10
        dc.w    $FFFF,$F7FF,$0000,$0800
        ; case 31, line $70: bit-plane zero bit 11, sprite one bit 11
        dc.w    $FFFF,$EFFF,$0000,$1000
        ; case 32, line $71: bit-plane zero bit 12, sprite one bit 12
        dc.w    $FFFF,$DFFF,$0000,$2000
        ; case 33, line $72: bit-plane zero bit 13, sprite one bit 13
        dc.w    $FFFF,$BFFF,$0000,$4000
        ; case 34, line $73: bit-plane zero bit 14, sprite one bit 14
        dc.w    $FFFF,$7FFF,$0000,$8000
        ; case 35, line $74: bit-plane zero bit 15, sprite one bit 15
        dc.w    $FFFE,$FFFF,$0001,$0000
        ; case 36, line $75: bit-plane zero bit 16, sprite one bit 16
        dc.w    $FFFD,$FFFF,$0002,$0000
        ; case 37, line $76: bit-plane zero bit 17, sprite one bit 17
        dc.w    $FFFB,$FFFF,$0004,$0000
        ; case 38, line $77: bit-plane zero bit 18, sprite one bit 18
        dc.w    $FFF7,$FFFF,$0008,$0000
        ; case 39, line $78: bit-plane zero bit 19, sprite one bit 19
        dc.w    $FFEF,$FFFF,$0010,$0000
