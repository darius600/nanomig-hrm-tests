; cookiecut_celsewhere.s -- A5 mutation: channel C points somewhere other
; than the destination, so no read of the destination's own address
; should precede its write.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$0FCA,BLTCON0(a0)     ; USEA+USEB+USEC+USED, LF=$CA
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTBMOD(a0)
        move.w  #0,BLTCMOD(a0)
        move.w  #0,BLTDMOD(a0)
        SETPTR  BLTAPTH,BLTAPTL,MASKBUF
        SETPTR  BLTBPTH,BLTBPTL,SRC
        SETPTR  BLTCPTH,BLTCPTL,SRC2   ; mutation: C reads elsewhere, not DST
        SETPTR  BLTDPTH,BLTDPTL,DST
        BLTEN_ON
        move.w  #(1<<6)|4,BLTSIZE(a0)
        WAITBLT
        DONE

        org     MASKBUF
        dc.w    $F0F0,$F0F0,$F0F0,$F0F0
        org     SRC
        dc.w    $AAAA,$AAAA,$AAAA,$AAAA
        org     DST
        dc.w    $5555,$5555,$5555,$5555
        org     SRC2
        dc.w    $9999,$9999,$9999,$9999
