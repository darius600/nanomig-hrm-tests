; prog_line.s -- ch6-blitter-line-fill: the line-mode claims (page 189-191).
; Sets up a line-mode blit for endpoints (10,5)-(3,40) per the p.191
; "REGISTER SUMMARY FOR LINE MODE" and the octant from Fig 6-8/Table 6-3
; (p.190): dy(35) > dx(7) so Y is the major axis, X falls (10->3), Y rises
; (5->40). The drawn line lands in a 36-word row buffer, one word per row
; y=5..40, bit index = X (words never cross a boundary since X stays within
; 3..10), for the harness to read back directly with peek_logical.
;
; MUTATE_UNNORM, if defined, feeds the raw, unswapped deltas (dx0=7 as the
; major axis, dy0=35 as the minor) into BLTAPT/BLTAMOD/BLTBMOD/BLTSIZE
; instead of the normalized dmax=35/dmin=7 -- BLTSIZE's height field becomes
; dx0+1=8 instead of 36, so the line comes up 28 rows short.
; MUTATE_FIXTURE, if defined, stores a wrong constant for the fixture.

        include "nanomig.i"
        org     PROGRAM_ORG

LINE_ROW0 equ   $082400         ; 36 words, one per row y=5..40

start:
        lea     RESULTS,a1
        ifd     MUTATE_FIXTURE
        move.w  #$DEAD,(a1)+           ; fixture (mutated): wrong constant
        else
        move.w  #$BEEF,(a1)+           ; fixture: known constant, checked first
        endif

        ; clear the 36-word row buffer so an unwritten row reads back zero
        move.l  #LINE_ROW0,a1
        moveq   #35,d0
.clr:   move.w  #0,(a1)+
        dbra    d0,.clr

        lea     CUSTOM,a0
        move.w  #$8240,DMACON(a0)      ; set DMAEN | BLTEN
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #$8000,BLTADAT(a0)     ; single pixel bit, shifted into place by ASH
        move.w  #$FFFF,BLTBDAT(a0)     ; solid texture (always draw)
        move.w  #0,BLTCDAT(a0)         ; background starts clear (defensive)
        move.w  #$0002,BLTCMOD(a0)     ; +1 word per row (Y rises with address)
        move.w  #$0002,BLTDMOD(a0)
        move.w  #$5FEA,BLTCON0(a0)     ; ASH=$5 (bit index 15-ash=10=X1), USEA|USEB|USEC|USED, LF=$EA (D=C|(A&B))

        ifd     MUTATE_UNNORM
        move.w  #$0001,BLTCON1(a0)     ; LINE, X falls, Y rises, SIGN=0 (unnormalized accumulator is positive)
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
        move.w  #$0041,BLTCON1(a0)     ; LINE, X falls, Y rises, SIGN=1 (initial accumulator is negative)
        move.l  #$FFFFFFD6,d0          ; BLTAPT = 2*(2*dmin-dmax) = 2*(14-35) = -42
        move.w  d0,BLTAPTL(a0)
        swap    d0
        move.w  d0,BLTAPTH(a0)
        move.l  #$FFFFFF90,d0          ; BLTAMOD = 2*2*(dmin-dmax) = 2*2*(7-35) = -112
        move.w  d0,BLTAMOD(a0)
        move.l  #28,d0                 ; BLTBMOD = 2*2*dmin = 2*2*7 = 28
        move.w  d0,BLTBMOD(a0)
        move.l  #LINE_ROW0,d0
        move.w  d0,BLTCPTL(a0)
        swap    d0
        move.w  d0,BLTCPTH(a0)
        move.l  #LINE_ROW0,d0
        move.w  d0,BLTDPTL(a0)
        swap    d0
        move.w  d0,BLTDPTH(a0)
        move.w  #$0902,BLTSIZE(a0)     ; height=dmax+1=36, width=2 -- starts the blit
        endif

        move.w  DMACONR(a0),d3         ; p.186: the first test of the done bit may be wrong -- discard it
.wait:  move.w  DMACONR(a0),d3
        btst.l  #14,d3                 ; BBUSY
        bne.s   .wait

        DONE
