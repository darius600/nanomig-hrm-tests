; prog_line.s -- ch6-blitter-line-fill: the line-mode claims (page 189-192).
; Sets up a line-mode blit for endpoints (10,5)-(3,40), built from the pages
; alone:
;   - ASH (BLTCON0 bits 15-12) = x1 modulo 15 = 10 (p.192).
;   - dx0 = abs(x2-x1) = 7, dy0 = abs(y2-y1) = 35 (p.190); normalized
;     dx = max(dx0,dy0) = 35, dy = min(dx0,dy0) = 7 (p.191/192).
;   - Octant: Figure 6-8 places a line falling in x (10->3) and rising in y
;     (5->40) -- steep, down-and-left -- in sector (5); Table 6-3 gives code
;     bits 010 for octant 5 (p.190), placed at BLTCON1 bits 4-2 (p.268).
;   - BLTCON0 bits 11-8 = 1,0,1,1 (p.268; matches p.192's SRCA/SRCC/SRCD=1,
;     SRCB=0); BLTCON0 LF byte = AB+AC (p.191/192, non-exclusive-or line),
;     which is $CA by the same minterm derivation p.181 works through for
;     AB+AC ("our classic cookie-cut function ... or $CA"). So BLTCON0 =
;     $ABCA.
;   - BLTAPT = 4*dy-2*dx = -42, BLTAMOD = 4*(dy-dx) = -112, BLTBMOD = 4*dy =
;     28 (p.192); SIGN (BLTCON1 bit 6) = 1 since BLTAPT < 0 (p.192).
;   - BLTSIZE bits 15-6 = dx+1 = 36, bits 5-0 = 2 (p.192): $0902.
;   - BLTCON1 = SIGN(1)<<6 | code(010)<<2 | LINE(1) = $0049.
; The drawn line lands in a 36-word row buffer, one word per row y=5..40,
; address = LINE_ROW0 + 2*(y-5), for the harness to read back directly with
; peek_logical. Two guard words (y=4, y=41) bracket the buffer and must stay
; zero -- nothing in the claims says the blit stops exactly at height 36
; unless that boundary is actually checked.
;
; Because x sits at bit 15-x within a word, not bit x (p.180: "the least
; significant (rightmost) four bits ... shifted in"; p.181: the most
; significant bits are the first, leftmost pixels), the first row's bit is
; 15-10=5 and the last row's bit is 15-3=12, rising (not falling) as x falls.
;
; MUTATE_UNNORM, if defined, feeds the raw, unswapped deltas (dx0=7 as the
; major axis, dy0=35 as the minor) into BLTAPT/BLTAMOD/BLTBMOD/BLTSIZE
; instead of the normalized dx=35/dy=7 -- BLTSIZE's height field becomes
; dx0+1=8 instead of 36, so the line comes up 28 rows short. ASH and the
; octant code are unaffected (they come from x1 and the real geometry, not
; the normalization step), so BLTCON0 stays $ABCA; only BLTCON1's SIGN bit
; changes (the swapped-in accumulator is positive).
; MUTATE_FIXTURE, if defined, stores a wrong constant for the fixture.

        include "nanomig.i"
        org     PROGRAM_ORG

LINE_ROW0     equ     $082400         ; 36 words, one per row y=5..40
LINE_GUARD_LO equ     LINE_ROW0-2     ; y=4, must stay zero
LINE_GUARD_HI equ     LINE_ROW0+72    ; y=41, must stay zero

start:
        lea     RESULTS,a1
        ifd     MUTATE_FIXTURE
        move.w  #$DEAD,(a1)+           ; fixture (mutated): wrong constant
        else
        move.w  #$BEEF,(a1)+           ; fixture: known constant, checked first
        endif

        ; clear the guard words and the 36-word row buffer so an unwritten
        ; row (or a line that overruns the window) reads back zero
        move.w  #0,LINE_GUARD_LO
        move.l  #LINE_ROW0,a1
        moveq   #35,d0
.clr:   move.w  #0,(a1)+
        dbra    d0,.clr
        move.w  #0,LINE_GUARD_HI

        lea     CUSTOM,a0
        move.w  #$8240,DMACON(a0)      ; set DMAEN | BLTEN
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$8000,BLTADAT(a0)     ; single pixel bit, shifted into place by ASH
        move.w  #$FFFF,BLTBDAT(a0)     ; solid texture (always draw)
        move.w  #0,BLTCDAT(a0)         ; background starts clear (defensive)
        move.w  #$0002,BLTCMOD(a0)     ; +1 word per row (y rises with address)
        move.w  #$0002,BLTDMOD(a0)
        move.w  #$ABCA,BLTCON0(a0)     ; ASH=$A=10=x1 mod 15, bits11-8=1011, LF=$CA (AB+AC)

        ifd     MUTATE_UNNORM
        move.w  #$0009,BLTCON1(a0)     ; LINE, octant code 010, SIGN=0 (unnormalized accumulator is positive)
        move.l  #126,d0                ; BLTAPT = 2*(2*dmin'-dmax'), dmax'=dx0=7, dmin'=dy0=35: 2*(70-7)
        move.w  d0,BLTAPTL(a0)
        swap    d0
        move.w  d0,BLTAPTH(a0)
        move.l  #112,d0                ; BLTAMOD = 2*2*(dmin'-dmax') = 2*2*(35-7)
        move.w  d0,BLTAMOD(a0)
        move.l  #140,d0                ; BLTBMOD = 2*2*dmin' = 2*2*35
        move.w  d0,BLTBMOD(a0)
        move.l  #LINE_ROW0,d0
        move.w  d0,BLTCPTL(a0)
        swap    d0
        move.w  d0,BLTCPTH(a0)
        move.l  #LINE_ROW0,d0
        move.w  d0,BLTDPTL(a0)
        swap    d0
        move.w  d0,BLTDPTH(a0)
        move.w  #$0202,BLTSIZE(a0)     ; height=dx0+1=8 (wrong), width=2
        else
        move.w  #$0049,BLTCON1(a0)     ; LINE, octant code 010, SIGN=1 (initial accumulator is negative)
        move.l  #$FFFFFFD6,d0          ; BLTAPT = 4*dy-2*dx = 4*7-2*35 = -42
        move.w  d0,BLTAPTL(a0)
        swap    d0
        move.w  d0,BLTAPTH(a0)
        move.l  #$FFFFFF90,d0          ; BLTAMOD = 4*(dy-dx) = 4*(7-35) = -112
        move.w  d0,BLTAMOD(a0)
        move.l  #28,d0                 ; BLTBMOD = 4*dy = 4*7 = 28
        move.w  d0,BLTBMOD(a0)
        move.l  #LINE_ROW0,d0
        move.w  d0,BLTCPTL(a0)
        swap    d0
        move.w  d0,BLTCPTH(a0)
        move.l  #LINE_ROW0,d0
        move.w  d0,BLTDPTL(a0)
        swap    d0
        move.w  d0,BLTDPTH(a0)
        move.w  #$0902,BLTSIZE(a0)     ; height=dx+1=36, width=2 -- starts the blit
        endif

        move.w  DMACONR(a0),d3         ; p.186: the first test of the done bit may be wrong -- discard it
.wait:  move.w  DMACONR(a0),d3
        btst.l  #14,d3                 ; BBUSY
        bne.s   .wait

        DONE
