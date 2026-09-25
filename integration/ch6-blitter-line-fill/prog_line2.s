; prog_line2.s -- ch6-blitter-line-fill: a second line-mode stimulus,
; endpoints (3,40)-(10,5) -- the same two points as prog_line.s's line, but
; with y2 < y1. Page 190: "dx = abs(x2-x1); dy = abs(y2-y1)". For
; prog_line.s's (10,5)-(3,40), y2-y1 = 40-5 = +35, so abs is the identity and
; that stimulus cannot tell "dy = abs(y2-y1)" from "dy = y2-y1" (the x claim
; does not share the problem: x2-x1 = -7 there already). Here y2-y1 =
; 5-40 = -35, so only a genuine abs() gives the right magnitude.
;
; Built the same way as prog_line.s, from the pages alone:
;   - x1=3, so ASH = x1 modulo 15 = 3 (p.192).
;   - dx0 = abs(x2-x1) = abs(10-3) = 7, dy0 = abs(y2-y1) = abs(5-40) = 35
;     (p.190); normalized dx = max(dx0,dy0) = 35, dy = min(dx0,dy0) = 7
;     (p.191/192) -- the same magnitudes as prog_line.s's line, since
;     normalization only uses abs values.
;   - Octant: direction is x rising (3->10) and y falling (40->5) -- steep,
;     up-and-right -- Figure 6-8 places that in sector (1); Table 6-3 gives
;     code bits 001 for octant 1 (p.190), at BLTCON1 bits 4-2 (p.268).
;   - BLTCON0 = ASH<<12 | $BCA, same derivation as prog_line.s: $3BCA.
;   - BLTAPT = 4*dy-2*dx = -42, BLTAMOD = 4*(dy-dx) = -112, BLTBMOD = 4*dy =
;     28 (p.192, same magnitudes as prog_line.s); SIGN=1 since BLTAPT<0.
;   - BLTSIZE = (dx+1)<<6 | 2 = $0902 (same length, 36 rows).
;   - BLTCON1 = SIGN(1)<<6 | code(001)<<2 | LINE(1) = $0045.
;
; The first pixel of this line is (3,40): BLTCPT/BLTDPT must point at the
; word containing it (p.191/192). It reuses prog_line.s's row buffer layout
; (address = LINE_ROW0 + 2*(y-5)), so the first pixel is at LINE_ROW0+70
; (row 35). BLTCMOD/BLTDMOD stay the page's plain positive "width of the
; bitplane in bytes" ($0002), same as prog_line.s: p.268's octant table
; gives octant 1 (this line) AUL=1 ("always up or left"), where octant 5/6
; (prog_line.s's line, AUL=0) walk the major axis down; the octant's AUL/SUD
; bits -- not the sign of CMOD/DMOD -- are what the blitter uses to decide
; the major-axis step direction, so the address correctly walks from row 35
; back down to row 0 with the same positive modulo. A correct abs()'d result
; lands on the identical two
; graded extremes prog_line.s uses: bit 5 at row 0 (y=5, x=10), bit 12 at
; row 35 (y=40, x=3) -- p.180/181's bit-15-minus-x mapping, same as
; prog_line.s. Two guard words (y=4, y=41) bracket the buffer.
;
; MUTATE_NOABS, if defined, feeds the raw signed y2-y1=-35 (not abs'd) into
; the min/max normalization alongside dx0=7: dx_bad = max(7,-35) = 7 (wrong,
; should be 35), dy_bad = min(7,-35) = -35 (wrong sign and magnitude).
; BLTSIZE's height field becomes dx_bad+1 = 8 instead of 36, the same
; observable failure as prog_line_mut.s's MUTATE_UNNORM, reached by a
; different broken derivation (skipping abs() instead of skipping the
; major/minor swap).

        include "nanomig.i"
        org     PROGRAM_ORG

LINE_ROW0     equ     $082400         ; same 36-word buffer as prog_line.s
LINE_GUARD_LO equ     LINE_ROW0-2     ; y=4, must stay zero
LINE_GUARD_HI equ     LINE_ROW0+72    ; y=41, must stay zero
LINE_ROW35    equ     LINE_ROW0+70    ; y=40: first pixel of this line

start:
        ; clear the guard words and the 36-word row buffer
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
        move.w  #$0002,BLTCMOD(a0)     ; width of the bitplane in bytes (p.192); octant's AUL bit steps it down
        move.w  #$0002,BLTDMOD(a0)
        move.w  #$3BCA,BLTCON0(a0)     ; ASH=$3=3=x1 mod 15, bits11-8=1011, LF=$CA (AB+AC)

        ifd     MUTATE_NOABS
        move.w  #$0045,BLTCON1(a0)     ; LINE, octant code 001, SIGN=1 (still negative with the bad values)
        move.l  #$FFFFFF66,d0          ; BLTAPT = 4*dy_bad-2*dx_bad = 4*(-35)-2*7 = -154
        move.w  d0,BLTAPTL(a0)
        swap    d0
        move.w  d0,BLTAPTH(a0)
        move.l  #$FFFFFF58,d0          ; BLTAMOD = 4*(dy_bad-dx_bad) = 4*(-35-7) = -168
        move.w  d0,BLTAMOD(a0)
        move.l  #$FFFFFF74,d0          ; BLTBMOD = 4*dy_bad = 4*(-35) = -140
        move.w  d0,BLTBMOD(a0)
        move.l  #LINE_ROW35,d0
        move.w  d0,BLTCPTL(a0)
        swap    d0
        move.w  d0,BLTCPTH(a0)
        move.l  #LINE_ROW35,d0
        move.w  d0,BLTDPTL(a0)
        swap    d0
        move.w  d0,BLTDPTH(a0)
        move.w  #$0202,BLTSIZE(a0)     ; height=dx_bad+1=8 (wrong), width=2
        else
        move.w  #$0045,BLTCON1(a0)     ; LINE, octant code 001, SIGN=1 (BLTAPT<0)
        move.l  #$FFFFFFD6,d0          ; BLTAPT = 4*dy-2*dx = 4*7-2*35 = -42
        move.w  d0,BLTAPTL(a0)
        swap    d0
        move.w  d0,BLTAPTH(a0)
        move.l  #$FFFFFF90,d0          ; BLTAMOD = 4*(dy-dx) = 4*(7-35) = -112
        move.w  d0,BLTAMOD(a0)
        move.l  #28,d0                 ; BLTBMOD = 4*dy = 4*7 = 28
        move.w  d0,BLTBMOD(a0)
        move.l  #LINE_ROW35,d0
        move.w  d0,BLTCPTL(a0)
        swap    d0
        move.w  d0,BLTCPTH(a0)
        move.l  #LINE_ROW35,d0
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
