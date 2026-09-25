; fig64.s -- Figure 6-4 (p.181) extraction recipe: channel B reads exactly
; two consecutive source words per row and channel D writes two per row
; (6-shifts-and-masks.c32, ch6-shifts.f1). Channel A is disabled and
; preloaded all-ones (BLTADAT), masked by BLTAFWM/BLTALWM at the row
; edges, per the page's own recipe: BLTCON0=$07CA (USEB+USEC+USED, A
; disabled, minterm AB+A'C), BLTCON1=$5000 (B shift 5).
;
; This grades only the structural claim (reads/writes per row), not the
; figure's printed final rows: those conflict with the figure's own drawn
; mask by one bit (the drawn second-word mask is 7 bits, "11111110
; 00000000", matching a 23-bit extraction; the printed final rows take 8
; bits from the source). That conflict is recorded, not graded.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$07CA,BLTCON0(a0)     ; ASH=0, USEB+USEC+USED, LF=$CA
        move.w  #$5000,BLTCON1(a0)     ; BSH=5
        move.w  #$FFFF,BLTADAT(a0)     ; A disabled, preloaded all-ones
        move.w  #$07FF,BLTAFWM(a0)
        move.w  #$FFF0,BLTALWM(a0)
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTCMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTCPTH,BLTCPTL,DST
        SETPTR  BLTDPTH,BLTDPTL,DST
        BLTEN_ON
        ifnd    WRONGWIDTH
        move.w  #(3<<6)|2,BLTSIZE(a0)  ; height 3, width 2 words/row
        else
        move.w  #(3<<6)|3,BLTSIZE(a0)  ; mutation: 3 words/row
        endc
        WAITBLT
        DONE

        org     SRC
        dc.w    $FFFF,$FE00,$FFFF,$FE00,$FFFF,$FE00,$FFFF,$FE00,$FFFF
        org     DST
        dc.w    $5555,$5555,$5555,$5555,$5555,$5555,$5555,$5555,$5555
