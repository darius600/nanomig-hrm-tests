; arbshrink.s -- A8b: arbitrary-region copy where the source spans more
; words than the destination's true data (6-copying-arbitrary-regions.c2,
; ch6-copy.c3): a 3-word source (20 true bits: 2 leading + 16 + 2
; trailing) shifted right 2 bits compacts its meaningful content into 2
; words. The A channel points at the SAME source as B (ch6-copy.c11: the
; same shift value, ASH=BSH=2) so D = A AND B (LF=$C0) reproduces B's own
; shifted data, masked as if masking the B source data (ch6-copy.c12).
; Blit size is still the larger span, 3 (ch6-copy.c4): the third fetch is
; what lets the shifter correctly finish combining the last real source
; word into the second (and last meaningful) destination word.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$2DC0,BLTCON0(a0)     ; ASH=2, USEA+USEB+USED, LF=$C0 (A AND B)
        move.w  #$2000,BLTCON1(a0)     ; BSH=2
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,SRC     ; A and B fetch the same source
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTDPTH,BLTDPTL,DST
        BLTEN_ON
        ifnd    WRONGSIZE
        move.w  #(1<<6)|3,BLTSIZE(a0)  ; height 1, width 3 (the larger, source's own span)
        else
        move.w  #(1<<6)|2,BLTSIZE(a0)  ; mutation: truncated to 2
        endc
        WAITBLT
        DONE

        org     SRC
        dc.w    $0003,$FFFF,$C000
