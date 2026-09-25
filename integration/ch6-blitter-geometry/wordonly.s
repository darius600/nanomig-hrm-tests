; wordonly.s -- A11: the blitter operates on words, so the least
; significant bit of a pointer is ignored (6-dma-channels.c79). BLTAPT is
; set to SRC+1, an odd byte address.
        include "nanomig.i"
        include "geom.i"

        org     PROGRAM_ORG
start:
        lea     CUSTOM,a0
        move.w  #$09F0,BLTCON0(a0)     ; USEA+USED, LF=$F0 (D=A)
        move.w  #$0000,BLTCON1(a0)
        move.w  #$FFFF,BLTAFWM(a0)
        move.w  #$FFFF,BLTALWM(a0)
        move.w  #0,BLTAMOD(a0)
        move.w  #0,BLTDMOD(a0)
        ifnd    WRONGWORD
        SETPTR  BLTAPTH,BLTAPTL,SRC+1  ; odd address, truncates to SRC
        else
        SETPTR  BLTAPTH,BLTAPTL,SRC+3  ; mutation: truncates to SRC+2 instead
        endc
        SETPTR  BLTDPTH,BLTDPTL,DST
        BLTEN_ON
        move.w  #(1<<6)|1,BLTSIZE(a0)
        WAITBLT
        DONE

        org     SRC
        dc.w    $ABCD,$1234
